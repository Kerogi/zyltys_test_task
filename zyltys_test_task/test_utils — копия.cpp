#include <string>
#include <iomanip>
#include <vector>
#include <cassert>
#include <sstream>
#include <tuple>

#include "test_utils.h"
#include "string_utils.h"
#include "lorem_ipsum.h"


static size_t  max_show_text_length = 30;

void print_arg(std::ostream& os, const std::string& value) {
	os << "'";
	if (value.length() <= max_show_text_length) {
		os << value;
	}
	else {
		os << value.substr(0, max_show_text_length - 3) + "...";
	}
	os << "'";
}

void print_arg(std::ostream& os, const char* value) {
	print_arg(os, std::string(value));
}

template <class T>
void print_arg(std::ostream& os, T value) {
	os << value;
}

template <class T>
void print_rest_args(std::ostream& os, T value) {
	print_arg(os, std::forward<T>(value));
	os << ")";
}

template <class T, class... Rest>
void print_rest_args(std::ostream& os, T value, Rest... rest) {
	print_arg(os, std::forward<T>(value));
	os << ", ";
	print_rest_args(os, std::forward<Rest>(rest)...);
}



template<class... Args>
void print_args(std::ostream& os, Args... args)
{
	os << "(";
	print_rest_args(os, std::forward<Args>(args)...);
}


template<typename FuncToTestT, typename FuncRetT, typename ...FuncArgsT>
bool test_return_value(std::ostream & os, const FuncToTestT& func, const FuncRetT& expected_return_value, FuncArgsT&&... func_args) {
	//arrange
	/* <empty> */

	//act
	const FuncRetT actual_return_value = func(std::forward<FuncArgsT>(func_args)...);

	//assert

	bool assert_result = (expected_return_value == actual_return_value);
	if (!assert_result) {
		os << "NOK expected != to actual, " << expected_return_value << " != " << actual_return_value << ", for args ";
		print_args(os, std::forward<FuncArgsT>(func_args)...);
		os << std::endl;
	}

	return assert_result;
}

template< typename RetT, typename ...TestArgsT>
struct TestCaseT {
	RetT expected_return_value;
	std::tuple<TestArgsT...> test_function_args;
};

namespace detail
{
	template <typename F, typename Tuple, bool Done, int Total, int... N>
	struct call_impl
	{
		static void call(F f, Tuple && t)
		{
			call_impl<F, Tuple, Total == 1 + sizeof...(N), Total, N..., sizeof...(N)>::call(f, std::forward<Tuple>(t));
		}

		template <typename R>
		static R call_r(F f, Tuple && t)
		{
			return call_impl<F, Tuple, Total == 1 + sizeof...(N), Total, N..., sizeof...(N)>::call_r<typename R>(f, std::forward<Tuple>(t));
		}
	};

	template <typename F, typename Tuple, int Total, int... N>
	struct call_impl<F, Tuple, true, Total, N...>
	{
		static void call(F f, Tuple && t)
		{
			f(std::get<N>(std::forward<Tuple>(t))...);
		}

		template <typename R>
		static R call_r(F f, Tuple && t)
		{
			return f(std::get<N>(std::forward<Tuple>(t))...);
		}
	};
}

template <typename F, typename Tuple>
void call(F f, Tuple && t)
{
	typedef typename std::decay<Tuple>::type ttype;
	detail::call_impl<F, Tuple, 0 == std::tuple_size<ttype>::value, std::tuple_size<ttype>::value>::call(f, std::forward<Tuple>(t));
}

template <typename R, typename F, typename Tuple>
R call_r(F f, Tuple && t)
{
	typedef typename std::decay<Tuple>::type ttype;
	return detail::call_impl<F, Tuple, 0 == std::tuple_size<ttype>::value, std::tuple_size<ttype>::value>::call_r<typename R>(f, std::forward<Tuple>(t));
}

template<typename FuncToTestT, typename TestCasesT>
bool run_test_cases_tests(std::ostream & os, const std::string & test_name, FuncToTestT func, std::vector<TestCasesT> test_cases) {
	size_t test_count = 1;
	size_t test_total = test_cases.size();

	for (const auto& tc : test_cases) {
		os << "Running " << test_name <<" ["<< test_count << " of " << test_total << "] - ";
		auto test_runner_args = std::tuple_cat(std::tie(os, func, tc.expected_return_value), tc.test_function_args);


		if (!call_r<bool>(test_return_value, test_runner_args)) {
			os << "ERROR test failed";
			return false;
		}
		else {
			os << "OK";
		}
		os << std::endl;
	}
	return true;
}

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

	using test_case_t = TestCaseT<int, std::string>;
	std::vector<test_case_t> test_cases = {
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
	
	/*size_t test_count = 1;
	size_t test_total = test_cases.size();
	for (const auto& tc : test_cases) {
		os << "Running test " << test_count << " of " << test_total << " - ";
		if (!test_return_value(os, string_utils::word_count, tc.expected_return_value, tc.test_function_args)) {
			os << "ERROR test failed";
			return false;
		} else {
			os << "OK";
		}
		os << std::endl;
	}*/
	return run_test_cases_tests(os, "word_count", string_utils::word_count, test_cases);
}



template<class ItemT>
std::ostream& operator<<(std::ostream& os, const std::list< ItemT > & list) {
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
	std::vector<test_case> test_cases = {
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
		std::string lorem_ipsum_wc123_str(lorem_ipsum_wc123);
		std::istringstream iss(lorem_ipsum_wc123_str);
		size_t max_word = 0;

		do
		{
			std::string subs;
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
			os << "ERROR test failed" << std::endl;
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

	std::vector<test_case> test_cases = {
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
	
	for (const test_case& tc : test_cases) {
		if (!test_get_most_bloat_words(os, tc.text_to_test, tc.expected_longest_words)) {
			os << "ERROR test failed" << std::endl;
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

	std::vector<test_case> test_cases = {
		{ { "world", "hello" } , "hello world" },
		{ { "world", "hellooo" }, "        hellooo world" },
		{ { "d","l","r","o","w","o","l","l","e","h" }, "h e l l o w o r l d" },
		{ { "h" }, "h" },
		{ { "woorld", "aaa" }, "aaa             woorld\0 to you" },
		{ {}, " " },
		{ {}, "" },
		{ {}, "               " },
	};

	for (const test_case& tc : test_cases) {
		if (!test_reversed_words(os, tc.text_to_test, tc.expected_revered_words)) {
			os << "ERROR test failed" << std::endl;
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

	std::vector<test_case> test_cases = {
		{ 2 , "hello" },
		{ 3,  "hellooo" },
		{ 1, "a" },
		{ 0, "" },
		{ 3, "aaa" },
		{ 3 , "abbccddbbaccc" },
	};

	for (const test_case& tc : test_cases) {
		if (!test_get_longest_symbol_run(os, tc.text_to_test, tc.expected_longest_run)) {
			os << "ERROR test failed" << std::endl;
			return false;
		}
	}
	return true;
}