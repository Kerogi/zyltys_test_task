#pragma once
#include <string>
#include <iomanip>
#include <ostream>

#include "string_utils.h"

inline bool ASSERT_WC(std::ostream& os, const std::string& s, const int expected_wc, const size_t max_show_text_length = 30) {
	//arrange

	//act
	int actual_wc = string_utils::word_count(s);

	//assert
	if (expected_wc == actual_wc) {
		os << "OK  expected word count == to actual, " << std::setw(3) << expected_wc << " == " << std::setw(3) << actual_wc << ",    for text '" << ((s.length() <= max_show_text_length) ? s : s.substr(0, max_show_text_length - 3) + "...") << "'" << std::endl;
		return true;
	}
	else {
		os << "NOK expected word count != to actual, " << std::setw(3) << expected_wc << " != " << std::setw(3) << actual_wc << ",    for text '" << ((s.length() <= max_show_text_length) ? s : s.substr(0, max_show_text_length - 3) + "...") << "'" << std::endl;
		return false;
	}
}