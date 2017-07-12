#pragma once
#include <string>
#include <list>
#include <vector>
#include <ostream>

namespace string_utils
{
	using string_t = std::string;
	using string_list_t = std::list<string_t>;
	using poitions_list_t = std::vector<size_t>;
	struct word_stat_t {
		string_t			word;
		poitions_list_t		positions;
		word_stat_t(string_t word, poitions_list_t positions)
			: word(word)
			, positions(positions)
		{}
		bool operator==(const word_stat_t& rhs) const;
		std::ostream& operator<<(std::ostream& os) const;
	};

	using text_stats_t = std::list<word_stat_t>;
	text_stats_t text_stats(const string_t& text);

	int word_count(const string_t& text);
	string_list_t get_longest_words(const string_t& text);
	string_list_t get_most_bloat_words(const string_t& text);
	string_list_t reverse_words(const string_t& text, size_t reserve_size = 0);
	size_t get_longest_symbol_run(const string_t& str);

};

std::ostream& operator<<(std::ostream& os, const string_utils::word_stat_t& s);

