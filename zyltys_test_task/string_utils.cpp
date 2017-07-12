#include "string_utils.h"
#include <cctype>
#include <cwctype>
#include <vector>
namespace string_utils 
{
using char_t = string_t::traits_type::char_type;

struct word_t {
	const char_t * const pstr;
	const size_t length;
};

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
	const char_t* p_word_start = nullptr;
	size_t word_length = 0;

	while ((text_length > pos) && (c = *(text + pos)) && string_t::traits_type::not_eof(c)) {
		if (is_a_word(c)) {
			p_word_start = (text + pos);
			break;
		}
		++pos;
	}
	auto pos_start = pos++;
	while ((text_length > pos) && (c = *(text + pos)) && string_t::traits_type::not_eof(c)) {
		if (is_not_a_word(c)) {
			word_length = pos - pos_start;
			//TODO: validate legth?
			break;
		}
		++pos;
	}
	*p_text_curr_pos = ++pos;
	return { p_word_start, word_length };
}

int string_utils::count_words(const string_t & text)
{
	if (text.empty()) return 0;
	size_t text_length = text.length();
	size_t text_length = text.length();
	std::vector<word_t> words;
	word_t cur_word;
	while(cur_word = get_next_word)
	return 0;
}

const string_t & string_utils::get_longest_word(const string_t & text)
{
	return text;
}
const string_t & get_most_bloat_word(const string_t & text)
{
	return text;
}
const string_t & reverse_words(const string_t & text)
{
	return text;
}
}