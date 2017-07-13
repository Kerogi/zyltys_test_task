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
		//word_stat_t(string_t word, poitions_list_t positions)
		//	: word(word)
		//	, positions(positions)
		//{}
		bool operator==(const word_stat_t& rhs) const;
		//std::ostream& operator<<(std::ostream& os) const;
	};

	using text_stats_t = std::list<word_stat_t>;
	text_stats_t text_stats(const string_t& text);

	int word_count(const string_t& text);
	string_list_t get_longest_words(const string_t& text);
	string_list_t get_most_bloat_words(const string_t& text);
	string_list_t reverse_words(const string_t& text);
	size_t get_longest_symbol_run(const string_t& str);



	namespace detail {
		using char_t = string_t::traits_type::char_type;

		struct word_t {
			const char_t * p_start;
			size_t length;
			static word_t nullword;
		};

		bool operator!=(const word_t& left, const word_t& right);
		
		word_t get_next_word(size_t* p_text_curr_pos, const char_t * text, size_t text_length);

		bool  is_not_a_word(char_t c);
		bool  is_a_word(char_t c);
	}

};

std::ostream& operator<<(std::ostream& os, const string_utils::word_stat_t& s);

