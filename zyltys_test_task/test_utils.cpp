#include <string>
#include <iomanip>
#include <vector>
#include <cassert>
#include <sstream>

#include "test_utils.h"
#include "string_utils.h"
#include "lorem_ipsum.h"

using namespace std;

bool test_word_count(std::ostream & os, const std::string & s, const int expected_wc, const size_t max_show_text_length=30) {
	//arrange
	/* <empty> */

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

bool word_count_tests(std::ostream & os) {
	struct test_case {
		int expected_word_count;
		const char *text_to_test;
	};
	vector<test_case> test_cases = {
		{ 2, "hello world" },
		{ 2, "hello world      " },
		{ 2, "        hello world" },
		{ 10, "h e l l o w o r l d" },
		{ 1, "h" },
		{ 1, "hello" },
		{ 4, "hello             world to you" },
		{ 0, " " },
		{ 0, "                " },
		{ 0, "" },
		{ 2, "hello             world\0 to you" },
		{ 123, lorem_ipsum_wc123 }
	};
	for (const test_case& tc : test_cases) {
		if (!test_word_count(os, tc.text_to_test, tc.expected_word_count)) {
			os << "ERROR test failed" << endl;
			return false;
		}
	}
	return true;
}



template<class ItemT>
ostream& operator<<(ostream& os, const list< ItemT > & list) {
	os << '[';
	if (!list.empty()) {
		bool first = true;
		for (const ItemT& item : list) {
			if (!first) os << ", ";
			os << item;
			first = false;
		}
	}
	os << ']';
	return os;
}

bool test_get_longest_words(std::ostream & os, const std::string & s, const string_utils::string_list_t & expected_longest_words, const size_t max_show_text_length = 30) {
	//arrange
	/* <empty> */

	//act
	const string_utils::string_list_t actual_max_words = string_utils::get_longest_words(s);

	//assert
	if (expected_longest_words == actual_max_words) {
		os << "OK  expected longest word list == to actual, " << expected_longest_words << " == " << actual_max_words << ",    for text '" << ((s.length() <= max_show_text_length) ? s : s.substr(0, max_show_text_length - 3) + "...") << "'" << std::endl;
		return true;
	}
	else {
		os << "NOK expected longest word list != to actual, " << expected_longest_words << " != "  << actual_max_words << ",    for text '" << ((s.length() <= max_show_text_length) ? s : s.substr(0, max_show_text_length - 3) + "...") << "'" << std::endl;
		return false;
	}
}

bool get_longest_word_tests(std::ostream & os)
{
	struct test_case {
		string_utils::string_list_t expected_longest_words;
		const char *text_to_test;
	};
	vector<test_case> test_cases = {
		{ {"hello", "world"} , "hello world" },
		{ {"worlds"},  "hello worlds      " },
		{ { "hellooo" }, "        hellooo world" },
		{ { "h","e","l","l","o","w","o","r","l","d" }, "h e l l o w o r l d" },
		{ {"h"}, "h" },
		{ { "hellou" }, "hellou             world\0 to you" },
		{ {}, " " },
		{ {}, "" },
		{ {}, "               " },
	};
	string_utils::string_list_t lorem_ipsum_longest_words;
	{ // caclulatin max word in different way
		string lorem_ipsum_wc123_str(lorem_ipsum_wc123);
		istringstream iss(lorem_ipsum_wc123_str);
		size_t max_word = 0;

		do
		{
			string subs;
			iss >> subs;
			if (max_word < subs.length()) {
				lorem_ipsum_longest_words.clear();
				lorem_ipsum_longest_words.push_back(subs);
				max_word = subs.length();
			} else if (max_word == subs.length())  {
				lorem_ipsum_longest_words.push_back(subs);
			}
		} while (iss);
	}
	test_cases.push_back({lorem_ipsum_longest_words, lorem_ipsum_wc123});
	for (const test_case& tc : test_cases) {
		if (!test_get_longest_words(os, tc.text_to_test, tc.expected_longest_words)) {
			os << "ERROR test failed" << endl;
			return false;
		}
	}
	return true;
}



bool test_get_most_bloat_words(std::ostream & os, const std::string & s, const string_utils::string_list_t & expected_bloatest_words, const size_t max_show_text_length = 30) {
	//arrange
	/* <empty> */

	//act
	const string_utils::string_list_t actual_bloatest_words = string_utils::get_most_bloat_words(s);

	//assert
	if (expected_bloatest_words == actual_bloatest_words) {
		os << "OK  expected most bloat word list == to actual, " << expected_bloatest_words << " == " << actual_bloatest_words << ",    for text '" << ((s.length() <= max_show_text_length) ? s : s.substr(0, max_show_text_length - 3) + "...") << "'" << std::endl;
		return true;
	} else {
		os << "NOK expected most bloat word list != to actual, " << expected_bloatest_words << " != " << actual_bloatest_words << ",    for text '" << ((s.length() <= max_show_text_length) ? s : s.substr(0, max_show_text_length - 3) + "...") << "'" << std::endl;
		return false;
	}
}

bool get_most_bloat_words_tests(std::ostream & os)
{
	struct test_case {
		string_utils::string_list_t expected_longest_words;
		const char *text_to_test;
	};

	vector<test_case> test_cases = {
		{ { "hello" } , "hello world" },
		{ { "hellooo" }, "        hellooo world" },
		{ { "h","e","l","l","o","w","o","r","l","d" }, "h e l l o w o r l d" },
		{ { "h" }, "h" },
		{ { "aaa" }, "aaa             woorld\0 to you" },
		{ {}, " " },
		{ {}, "" },
		{ {}, "               " },
		{ {"accc"}, "abbccddbb  accc" }, 
		{ { "abbccddbb", "abbc" }, "abbccddbb  abbc 123" }, 
	};
	string_utils::string_list_t lorem_ipsum_longest_words;
	{ // caclulatin max word in different way
		string lorem_ipsum_wc123_str(lorem_ipsum_wc123);
		istringstream iss(lorem_ipsum_wc123_str);
		size_t max_word = 0;

		do
		{
			string subs;
			iss >> subs;
			if (max_word < subs.length()) {
				lorem_ipsum_longest_words.clear();
				lorem_ipsum_longest_words.push_back(subs);
				max_word = subs.length();
			}
			else if (max_word == subs.length()) {
				lorem_ipsum_longest_words.push_back(subs);
			}
		} while (iss);
	}
	//test_cases.push_back({ lorem_ipsum_longest_words, lorem_ipsum_wc123 });
	for (const test_case& tc : test_cases) {
		if (!test_get_most_bloat_words(os, tc.text_to_test, tc.expected_longest_words)) {
			os << "ERROR test failed" << endl;
			return false;
		}
	}
	return true;
}

bool test_reversed_words(std::ostream & os, const std::string & s, const string_utils::string_list_t & expected_reversed_words, const size_t max_show_text_length = 30) {
	//arrange
	/* <empty> */

	//act
	const string_utils::string_list_t actual_reversed_words = string_utils::reverse_words(s);

	//assert
	if (expected_reversed_words == actual_reversed_words) {
		os << "OK  expected most bloat word list == to actual, " << expected_reversed_words << " == " << actual_reversed_words << ",    for text '" << ((s.length() <= max_show_text_length) ? s : s.substr(0, max_show_text_length - 3) + "...") << "'" << std::endl;
		return true;
	} else {
		os << "NOK expected most bloat word list != to actual, " << expected_reversed_words << " != " << actual_reversed_words << ",    for text '" << ((s.length() <= max_show_text_length) ? s : s.substr(0, max_show_text_length - 3) + "...") << "'" << std::endl;
		return false;
	}
}

bool reverse_words_tests(std::ostream & os)
{
	struct test_case {
		string_utils::string_list_t expected_revered_words;
		const char *text_to_test;
	};

	vector<test_case> test_cases = {
		{ { "world", "hello" } , "hello world" },
		{ { "world", "hellooo" }, "        hellooo world" },
		{ { "d","l","r","o","w","o","l","l","e","h" }, "h e l l o w o r l d" },
		{ { "h" }, "h" },
		{ { "woorld", "aaa" }, "aaa             woorld\0 to you" },
		{ {}, " " },
		{ {}, "" },
		{ {}, "               " },
	};
	string_utils::string_list_t lorem_ipsum_longest_words;
	{ // caclulatin reversed word in different way
		string lorem_ipsum_wc123_str(lorem_ipsum_wc123);
		istringstream iss(lorem_ipsum_wc123_str);
		size_t max_word = 0;

		do
		{
			string subs;
			iss >> subs;
			if (max_word < subs.length()) {
				lorem_ipsum_longest_words.clear();
				lorem_ipsum_longest_words.push_back(subs);
				max_word = subs.length();
			}
			else if (max_word == subs.length()) {
				lorem_ipsum_longest_words.push_back(subs);
			}
		} while (iss);
	}
	//test_cases.push_back({ lorem_ipsum_longest_words, lorem_ipsum_wc123 });
	for (const test_case& tc : test_cases) {
		if (!test_reversed_words(os, tc.text_to_test, tc.expected_revered_words)) {
			os << "ERROR test failed" << endl;
			return false;
		}
	}
	return true;

}

bool test_get_longest_symbol_run(std::ostream & os, const std::string & s, size_t expected_longest_run, const size_t max_show_text_length = 30) {
	//arrange
	/* <empty> */

	//act
	const size_t actual_longest_run = string_utils::get_longest_symbol_run(s);

	//assert
	if (expected_longest_run == actual_longest_run) {
		os << "OK  expected longest symbol run == to actual, " << expected_longest_run << " == " << actual_longest_run << ",    for text '" << ((s.length() <= max_show_text_length) ? s : s.substr(0, max_show_text_length - 3) + "...") << "'" << std::endl;
		return true;
	}
	else {
		os << "NOK expected longest symbol run != to actual, " << expected_longest_run << " != " << actual_longest_run << ",    for text '" << ((s.length() <= max_show_text_length) ? s : s.substr(0, max_show_text_length - 3) + "...") << "'" << std::endl;
		return false;
	}
}

bool get_longest_symbol_run_tests(std::ostream & os)
{
	struct test_case {
		size_t expected_longest_run;
		const char *text_to_test;
	};

	vector<test_case> test_cases = {
		{ 2 , "hello" },
		{ 3,  "hellooo" },
		{ 1, "a" },
		{ 0, "" },
		{ 3, "aaa" },
		{ 3 , "abbccddbbaccc" },
	};

	for (const test_case& tc : test_cases) {
		if (!test_get_longest_symbol_run(os, tc.text_to_test, tc.expected_longest_run)) {
			os << "ERROR test failed" << endl;
			return false;
		}
	}
	return true;
}