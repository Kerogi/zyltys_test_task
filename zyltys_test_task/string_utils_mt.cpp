#include "string_utils_mt.h"
#include "misc_utils.h"

#include <map>
#include <vector>
#include <algorithm>
#include <type_traits>
#include <future>
#include <chrono>
#include <mutex>
#include <iostream>
#include <iomanip>
#include <thread>
#include <queue>
namespace string_utils_mt
{
using namespace string_utils;
using namespace string_utils::detail;

chunk_list_t split_text_to_chunks_by_word(const string_utils::string_t& text, size_t default_chunk_size) {
	if (text.empty()) return chunk_list_t();

	const  char_t* text_start = text.c_str();
	size_t text_length = text.length();

	chunk_list_t chunks;
	size_t chunk_min_threshhold = 3;
	//size_t def_chunk_size = std::max(text.length() / max_parralel_tasks, chunk_min_threshhold);
	size_t chunk_size = default_chunk_size;
	if (chunk_size >= text_length) {
		return { { text_start, 0, text_length } };
	}

	for (size_t chunk_start_pos = 0; chunk_start_pos < text_length; chunk_start_pos += chunk_size) {
		for (size_t def_chunk_size_mult = 0; ; ++def_chunk_size_mult) {
			chunk_size = default_chunk_size + default_chunk_size * def_chunk_size_mult;
			if (chunk_start_pos + chunk_size >= text_length) {
				chunk_size = text_length - chunk_start_pos;
				break;
			}
			while (chunk_size && is_a_word(*(text_start + chunk_start_pos + chunk_size))) --chunk_size;
			if (chunk_size) break;
		}
		chunk_t chunk{ text_start, chunk_start_pos, chunk_size };
		chunks.push_back(chunk);
	};

	return chunks;
}

namespace single_threaded {
	template<typename ItT, typename FuncT, typename FuncRetT = std::result_of<typename FuncT(typename ItT::value_type)>::type>
	std::list<FuncRetT> map_list(ItT begin, ItT end, FuncT func) {
		std::list<FuncRetT > mapped_results;
		for (; begin != end; ++begin) {
			mapped_results.push_back(func(*begin));
		}
		return mapped_results;
	}
}

namespace async_map 
{
	template<typename ItT, typename FuncT, typename FuncRetT  = std::result_of<typename FuncT(typename ItT::value_type)>::type >
	std::list<FuncRetT> map_list(ItT begin, ItT end, FuncT func) {
		std::list< std::future<FuncRetT> > prommised_results;
		for (; begin != end; ++begin) {
			prommised_results.push_back(std::async(std::launch::async, func, *begin));
		}

		std::list<FuncRetT> mapped_results;
		for (auto& f : prommised_results) mapped_results.push_back(f.get());
		return mapped_results;
	}
}

namespace concurent
{
	using job = std::function<void()>;
	class thread_pool {
		std::vector<std::thread> threads;

		std::mutex mt_jobs;
		std::condition_variable cond_has_jobs;
		std::queue<job> jobs_queue;

		bool done = false;
		void thread_func() {
			while (true) {
				std::unique_lock<std::mutex> lock(mt_jobs);
				cond_has_jobs.wait(lock, [&] { return done || !jobs_queue.empty(); });
				while (!jobs_queue.empty()) {
					job& j = jobs_queue.front();
					j();
					jobs_queue.pop();

				}
				if (done) break;
			}
		}

		size_t add_jobs(const std::vector<job>& jobj_vec)
		{
			std::unique_lock<std::mutex> lock(mt_jobs);
			for(auto&& j: jobj_vec)
				jobs_queue.push(j);
			size_t new_size = jobs_queue.size();
			lock.unlock();
			cond_has_jobs.notify_one();
			return new_size;
		}

	public:
		thread_pool(size_t pool_size) {
			for (size_t i = 0; i < pool_size; ++i) {
				threads.emplace_back(&thread_pool::thread_func, this);
			}
		}
		~thread_pool() 
		{
			if (!done) {
				{
					std::lock_guard<std::mutex> lock(mt_jobs);
					done = true;
				}
				cond_has_jobs.notify_all();
				for (auto& t : threads) {
					t.join();
				}
			}
		}

		template<typename ItT, typename FuncT, typename FuncRetT = std::result_of<typename FuncT(typename ItT::value_type)>::type >
		std::list<FuncRetT> map_list(ItT begin, ItT end, FuncT&& func) {
			size_t results_count = std::distance(begin, end);
			if (results_count == 0) return std::list<FuncRetT>();
			if (results_count == 1) {
				return std::list<FuncRetT>{func(*begin)};
			}
			size_t done_results = 0;
			size_t result_index = 0;

			std::mutex				mutex_results;
			std::condition_variable results_ready;
			std::vector<FuncRetT>	results(results_count);
			std::vector<job>	    jobs;

			for (; begin != end; ++begin, ++result_index) {
				auto& arg = *begin;
				auto job = [&, result_index, arg]() {
					auto ret = func(arg);
					{
						std::unique_lock<std::mutex> lock(mutex_results);
						results[result_index] = ret; 
						++done_results;
						if (results_count == done_results) {
							lock.unlock();
							results_ready.notify_all();
						} else {
							lock.unlock();
						}
					}
				};
				jobs.push_back(job);
			} 
			add_jobs(jobs);
			std::list<FuncRetT> return_results;

			if (result_index > 0) {
				{
					std::unique_lock<std::mutex> lock(mutex_results);
					results_ready.wait(lock, [&] { return results_count == done_results; });
				}
				for (auto& res : results) return_results.push_back(res);
			}
			return return_results;
		}
	};

}
static concurent::thread_pool tp(std::max(std::thread::hardware_concurrency(), (size_t)4));

int word_count_mt(const string_t & text, size_t max_parralel_tasks)
{
	
	size_t min_chunk_size = 3;
	chunk_list_t chunks = split_text_to_chunks_by_word(text, std::max(min_chunk_size, text.length() / max_parralel_tasks));

	auto chunk_word_count= [&](const chunk_t& chunk) {
		return word_count(string_t(chunk.chunk_text + chunk.chunk_start, chunk.chunk_length));
	};
	
	std::list<int> intermediate_results = tp.map_list(chunks.begin(), chunks.end(), chunk_word_count);
	
	int wcount = 0;
	for (const auto& res : intermediate_results) {
		wcount += res;
	}

	return wcount;
}



text_stats_t text_stats_mt(const string_t & text, size_t max_parralel_tasks)
{
	size_t total_text_length = text.length();
	size_t even_chunk_size = total_text_length / max_parralel_tasks;
	size_t min_chunk_size = 100;
	size_t max_chunk_size = 1000;
	size_t chunk_size = std::min(max_chunk_size, std::max(min_chunk_size, even_chunk_size));


	auto split_text_to_chunks_by_word_t0 = std::chrono::high_resolution_clock::now();
	chunk_list_t chunks = split_text_to_chunks_by_word(text, chunk_size);
	auto split_text_to_chunks_by_word_t1 = std::chrono::high_resolution_clock::now();
	if (chunks.empty()) return text_stats_t();

	using  stats_by_word_t = std::map<string_t, word_stat_t>;

	auto chunk_stats_by_word = [&](const chunk_t& chunk) {
		text_stats_t stats_for_chunk = text_stats(string_t(chunk.chunk_text + chunk.chunk_start, chunk.chunk_length));
		stats_by_word_t stats_by_word;
		for (auto& stat : stats_for_chunk) {
			for (auto& pos : stat.positions) pos += chunk.chunk_start; //adjust local to chunk pos to global
			stats_by_word[stat.word] = stat;
		}
		return stats_by_word;
	};

	std::recursive_mutex mt;

	std::map<string_t, std::list<word_stat_t>> grouped_by_words_stats;
	auto group_bu_words = [&grouped_by_words_stats, &mt](const stats_by_word_t& chunk_stats) {
		for (const auto& word_stat_pair : chunk_stats) {
			std::lock_guard<std::recursive_mutex> lock(mt);
			grouped_by_words_stats[word_stat_pair.first].push_back(word_stat_pair.second);
		}
		return 0;
	};

	stats_by_word_t redused_result_stats;

	auto reduse_to_words = [&redused_result_stats, &mt](const std::pair<string_t, std::list<word_stat_t>> & word_grouped_stats) {
		poitions_list_t merged_positions;
		for (const auto& word_stats : word_grouped_stats.second) {
			merged_positions.insert(merged_positions.end(), word_stats.positions.begin(), word_stats.positions.end());
		}
		std::sort(merged_positions.begin(), merged_positions.end());

		//std::lock_guard<std::recursive_mutex> lock(mt);
		redused_result_stats[word_grouped_stats.first] = { word_grouped_stats.first, merged_positions };
		return 0;
	};

	//using async_map::map_list;

	auto chunks_stats_t0 = std::chrono::high_resolution_clock::now();
	std::list<stats_by_word_t> chunks_stats = tp.map_list(chunks.begin(), chunks.end(), chunk_stats_by_word);
	auto chunks_stats_t1 = std::chrono::high_resolution_clock::now();

	auto grouped_by_words_stats_t0 = std::chrono::high_resolution_clock::now();
	tp.map_list(chunks_stats.begin(), chunks_stats.end(), group_bu_words);
	auto grouped_by_words_stats_t1 = std::chrono::high_resolution_clock::now();
	
	tp.map_list(grouped_by_words_stats.begin(), grouped_by_words_stats.end(), reduse_to_words);
	auto redused_by_words_stats_t1 = std::chrono::high_resolution_clock::now();

	auto copy_t0 = std::chrono::high_resolution_clock::now();
	text_stats_t result;
	for (const auto& res_by_word : redused_result_stats) {
		result.push_back(res_by_word.second);
	}
	auto copy_t1 = std::chrono::high_resolution_clock::now();

	//reorde by appearence

	auto cmp_by_position = [](const word_stat_t& lhs, const word_stat_t& rhs) -> bool {
		return lhs.positions.front() < rhs.positions.front();
	};

	auto sort_t0 = std::chrono::high_resolution_clock::now();
	result.sort(cmp_by_position);
	auto sort_t1 = std::chrono::high_resolution_clock::now();
	{
		using namespace quoted_string;
		std::cerr<< " text: " << text << std::endl;
	}
	std::cerr << " split_text_to_chunks_by_word took: " << std::setw(10) << std::chrono::duration_cast<std::chrono::microseconds>(split_text_to_chunks_by_word_t1 - split_text_to_chunks_by_word_t0).count() << " microsec" << std::endl;
	std::cerr << " chunks_stats  took: " << std::setw(10) << std::chrono::duration_cast<std::chrono::microseconds>(chunks_stats_t1 - chunks_stats_t0).count() << " microsec" << std::endl;
	std::cerr << " grouped_by_words_stats  took: " << std::setw(10) << std::chrono::duration_cast<std::chrono::microseconds>(grouped_by_words_stats_t1 - grouped_by_words_stats_t0).count() << " microsec" << std::endl;
	std::cerr << " redused_by_words_stats_t1 took: " << std::setw(10) << std::chrono::duration_cast<std::chrono::microseconds>(redused_by_words_stats_t1 - grouped_by_words_stats_t1).count() << " microsec" << std::endl;
	std::cerr << " sort took: " << std::setw(10) << std::chrono::duration_cast<std::chrono::microseconds>(copy_t1 - copy_t0).count() << " microsec" << std::endl;
	std::cerr << " sort took: " << std::setw(10) << std::chrono::duration_cast<std::chrono::microseconds>(sort_t1 - sort_t0).count() << " microsec" << std::endl;

	return result;
}

}

std::ostream& operator<<(std::ostream& os, const string_utils_mt::chunk_t& c)
{
	using namespace quoted_string;
	os << std::string(c.chunk_text + c.chunk_start, c.chunk_length);
	return os;
}