#include "string_utils_mt.h"



#include <vector>

#include <algorithm>
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
		return { { text_start, text_length } };
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
		chunk_t chunk{ text_start + chunk_start_pos, chunk_size };
		chunks.push_back(chunk);
	};

	return chunks;
}


int word_count_mt(const string_t & text, size_t max_parralel_tasks)
{
	return 0;
}


}