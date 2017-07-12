#include "string_utils.h"
#include <cctype>
#include <cwctype>
#include <vector>

namespace string_utils 
{
using char_t = string_t::traits_type::char_type;

struct word_t {
	const char_t * p_start;
	size_t length;
};

bool operator!=(const word_t& left, const word_t& right)
{
	return left.p_start != right.p_start;
}

static word_t nullword = { nullptr, 0 };

bool is_not_a_word(char c) {
	return !!std::isspace(c)
		// && !!std::ispunct(c)
		;
}

bool is_not_a_word(wchar_t c) {
	return !!std::iswspace(c)
		// && !!std::iswpunct(c)
		;
}
template<class CharT>
bool is_a_word(CharT c) {
	return !(is_not_a_word(c)); 
}

word_t get_next_word(size_t* p_text_curr_pos, const char_t * text, size_t text_length) {
	char_t c;
	auto pos = *p_text_curr_pos;
	const char_t* word_p_start = nullword.p_start;
	size_t word_length = nullword.length;

	while ((text_length > pos) && (c = *(text + pos)) && string_t::traits_type::not_eof(c)) {
		if (is_a_word(c)) {
			word_p_start = (text + pos);
			break;
		}
		++pos;
	}
	auto pos_start = pos++;
	while ((text_length > pos) && (c = *(text + pos)) && string_t::traits_type::not_eof(c)) {
		if (is_not_a_word(c)) {
			word_length = pos - pos_start;
			break;
		}
		++pos;
	}
	if (!word_length && pos == text_length) {
		word_length = pos - pos_start ;
	} 
	*p_text_curr_pos = ++pos;
	return { word_p_start, word_length };
}

int string_utils::word_count(const string_t & text)
{
	if (text.empty()) return 0;
	size_t text_pos = 0;

	size_t wcount = 0;
	while (get_next_word(&text_pos, text.c_str(), text.length()) != nullword) {
		++wcount;
	}
	return wcount;
}

#include <stdio.h>

string_list_t string_utils::get_longest_words(const string_t & text)
{
	if (text.empty()) return string_list_t();
	size_t text_pos = 0;
 
	size_t max_word_length = 0;
	word_t curr_word;
	string_list_t longest_words;

	while (( curr_word = get_next_word(&text_pos, text.c_str(), text.length()) ) != nullword) {
		if (max_word_length < curr_word.length) {
			longest_words.clear();
			longest_words.push_back(string_t(curr_word.p_start, curr_word.length));
			max_word_length = curr_word.length;
		} else if (max_word_length == curr_word.length){
			longest_words.push_back(string_t(curr_word.p_start, curr_word.length));
		}
	}  
	return longest_words;

}

size_t get_longest_symbol_run(const string_t& str) {
	if (str.empty()) return 0;
	if (str.length() == 1) return 1;

	size_t current_run_length = 1;
	size_t max_run_length = 1;
	char_t prev_c = 0;

	for (const char_t& c : str) {
		if (c == prev_c) {
			++current_run_length;
		} else {
			if (max_run_length < current_run_length) {
				max_run_length = current_run_length;
			}
			current_run_length = 1;
		}
		
		prev_c = c;
	}
	if (max_run_length < current_run_length) {
		max_run_length = current_run_length;
	}
	return max_run_length;
}

string_list_t get_most_bloat_words(const string_t & text)
{
	if (text.empty()) return string_list_t();
	size_t text_pos = 0;

	size_t max_word_longest_run = 0;
	word_t curr_word;
	string_list_t longest_runs_words;

	while ((curr_word = get_next_word(&text_pos, text.c_str(), text.length())) != nullword) {
		string_t a_word = string_t(curr_word.p_start, curr_word.length);
		size_t word_longest_run = get_longest_symbol_run(a_word);
		if (max_word_longest_run < word_longest_run) {
			longest_runs_words.clear();
			longest_runs_words.push_back(string_t(curr_word.p_start, curr_word.length));
			max_word_longest_run = word_longest_run;
		}
		else if (max_word_longest_run == word_longest_run) {
			longest_runs_words.push_back(a_word);
		}
	}
	return longest_runs_words;
}
string_list_t reverse_words(const string_t & text, size_t reserve_size)
{
	if (text.empty()) return string_list_t();
	size_t text_pos = 0;
	word_t curr_word;
	std::list<word_t> words;

	while ((curr_word = get_next_word(&text_pos, text.c_str(), text.length())) != nullword) {
		words.push_front(curr_word);
	}

	string_list_t reversed_words;
	for (const word_t& word : words) {
		reversed_words.emplace_back(word.p_start, word.length);
	}
	return reversed_words;

}
}