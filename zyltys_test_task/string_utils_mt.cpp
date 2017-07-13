#include "string_utils_mt.h"
#include "misc_utils.h"

#include <map>
#include <vector>
#include <algorithm>
#include <type_traits>

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
	std::list<FuncRetT> map(ItT begin, ItT end, FuncT func) {
		std::list<FuncRetT > mapped_results;
		for (; begin != end; ++begin) {
			mapped_results.push_back(func(*begin));
		}
		return mapped_results;
	}
}

int word_count_mt(const string_t & text, size_t max_parralel_tasks)
{
	size_t min_chunk_size = 3;
	chunk_list_t chunks = split_text_to_chunks_by_word(text, std::max(min_chunk_size, text.length() / max_parralel_tasks));

	auto chunk_word_count= [&](const chunk_t& chunk) {
		return word_count(string_t(chunk.chunk_text + chunk.chunk_start, chunk.chunk_length));
	};

	std::list<int> intermediate_results = single_threaded::map(chunks.begin(), chunks.end(), chunk_word_count);
	
	int wcount = 0;
	for (const auto& res : intermediate_results) {
		wcount += res;
	}

	return wcount;
}

text_stats_t text_stats_mt(const string_t & text, size_t max_parralel_tasks)
{
	size_t min_chunk_size = 3;
	chunk_list_t chunks = split_text_to_chunks_by_word(text, std::max(min_chunk_size, text.length() / max_parralel_tasks));
	
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


	std::map<string_t, std::list<word_stat_t>> grouped_by_words_stats;
	auto group_bu_words= [&grouped_by_words_stats](const stats_by_word_t& chunk_stats) {
		for (const auto& word_stat_pair : chunk_stats) {
			//todo lock
			grouped_by_words_stats[word_stat_pair.first].push_back(word_stat_pair.second);
		}
		return 0;
	};

	stats_by_word_t redused_result_stats;
	auto reduse_to_words = [&redused_result_stats](const std::pair<string_t, std::list<word_stat_t>> & word_grouped_stats) {
		poitions_list_t merged_positions;
		for (const auto& word_stats: word_grouped_stats.second) {
			merged_positions.insert(merged_positions.end(), word_stats.positions.begin(), word_stats.positions.end());
		}
		std::sort(merged_positions.begin(), merged_positions.end());
		//todo lock
		redused_result_stats[word_grouped_stats.first] = { word_grouped_stats.first, merged_positions };
		return 0;
	};

	std::list<stats_by_word_t> chunks_stats = single_threaded::map(chunks.begin(), chunks.end(), chunk_stats_by_word);


	single_threaded::map(chunks_stats.begin(), chunks_stats.end(), group_bu_words);
	single_threaded::map(grouped_by_words_stats.begin(), grouped_by_words_stats.end(), reduse_to_words);


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