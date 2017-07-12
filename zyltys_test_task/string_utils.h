#pragma once
#include <string>
#include <list>
namespace string_utils
{
	using string_t = std::string;
	using string_list_t = std::list<string_t>;

	int word_count(const string_t& text);
	string_list_t get_longest_word(const string_t& text);
	string_list_t get_most_bloat_word(const string_t& text);
	string_list_t reverse_words(const string_t& text);
};

