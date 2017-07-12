#pragma once
#include <string>
namespace string_utils
{
	using string_t = std::string;

	int word_count(const string_t& text);
	const string_t& get_longest_word(const string_t& text);
	const string_t& get_most_bloat_word(const string_t& text);
	const string_t& reverse_words(const string_t& text);
};

