#pragma once

#include "string_utils.h"


namespace string_utils_mt
{
	
	int word_count_mt(const string_utils::string_t& text, size_t max_parralel_tasks = 4);
	string_utils::text_stats_t text_stats_mt(const string_utils::string_t& text, size_t max_parralel_tasks = 4);

	struct chunk_t {
		const string_utils::detail::char_t* chunk_text;
		size_t chunk_start;
		size_t chunk_length;
	};
	using chunk_list_t = std::list<chunk_t>;
	chunk_list_t split_text_to_chunks_by_word(const string_utils::string_t& text, size_t default_chunk_size = 3);

};

std::ostream& operator<<(std::ostream& os, const string_utils_mt::chunk_t& s);