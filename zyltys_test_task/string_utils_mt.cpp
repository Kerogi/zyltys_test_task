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
		void add_job(job&& j)
		{
			std::unique_lock<std::mutex> lock(mt_jobs);	
			jobs_queue.push(j);
			lock.unlock();
			cond_has_jobs.notify_one();
		}

		void add_jobs(const std::vector<job>& jobj_vec)
		{
			std::unique_lock<std::mutex> lock(mt_jobs);
			for(auto&& j: jobj_vec)
				jobs_queue.push(j);
			lock.unlock();
			cond_has_jobs.notify_one();
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
			std::atomic<size_t> done_results = 0;
			size_t result_index = 0;

			std::mutex				mutex_results;
			std::condition_variable results_ready;
			std::vector<FuncRetT>	results(results_count);
			std::vector<job>	    jobs;

			for (; begin != end; ++begin, ++result_index) {
				auto& arg = *begin;
				auto job = [&, result_index, arg]() {
					auto ret = func(arg);

					results[result_index] = ret; 
					++done_results;
					{
						std::lock_guard<std::mutex> lock(mutex_results);
						if (results_count == done_results) {
							results_ready.notify_all();
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
	size_t max_chunk_size = 4000;
	size_t chunk_size = std::min(max_chunk_size, std::max(min_chunk_size, even_chunk_size));

	chunk_list_t chunks = split_text_to_chunks_by_word(text, chunk_size);

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
	

	std::list<stats_by_word_t> chunks_stats = tp.map_list(chunks.begin(), chunks.end(), chunk_stats_by_word);
	
	stats_by_word_t redused_result_stats;

	for (const auto& chunk_stats : chunks_stats) {
		for (const auto& word_stat_pair : chunk_stats) {
			word_stat_t& ws = redused_result_stats[word_stat_pair.first];
			ws.word = word_stat_pair.second.word;
			ws.positions.insert(ws.positions.end(), word_stat_pair.second.positions.begin(), word_stat_pair.second.positions.end());
		}
	}




	text_stats_t result;
	for (const auto& res_by_word : redused_result_stats) {
		result.push_back(res_by_word.second);
	}


	//reorde by appearence

	auto cmp_by_position = [](const word_stat_t& lhs, const word_stat_t& rhs) -> bool {
		return lhs.positions.front() < rhs.positions.front();
	};


	result.sort(cmp_by_position);



	return result;
}

}

std::ostream& operator<<(std::ostream& os, const string_utils_mt::chunk_t& c)
{
	using namespace quoted_string;
	os << std::string(c.chunk_text + c.chunk_start, c.chunk_length);
	return os;
}