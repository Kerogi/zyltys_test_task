#include <string>
#include <iomanip>
#include <vector>
#include <cassert>
#include <sstream>

#include "misc_utils.h"
#include "test_utils.h"
#include "string_utils.h"
#include "string_utils_mt.h"
#include "lorem_ipsum.h"

static  std::string h1line30(30, '=');
static  std::string h2line30(30, '-');

#define TEST_HEADER(TEST_NAME) 	std::string test_name (#TEST_NAME);{os << h1line30 << std::endl;\
								os << "\tStarting test suite for "##TEST_NAME << std::endl;\
								os << h2line30 << std::endl; }
#define TEST_FOOTER(RESULT)  { os << h2line30 << std::endl; \
								os << "\tTest suite for " << test_name << " finished " << ((RESULT)? "SUCCESSFULLY" : "UNSUCCESSFULLY" ) << std::endl; }
template <typename Arg>
size_t print_1arg(std::ostream& os, size_t ArgIdx, const char* delim, Arg&& arg) {
	using namespace quoted_string;
	if (ArgIdx > 0) os << delim;
	os << ArgIdx << ": " << arg;
	return ArgIdx;
}

template <typename... Args>
size_t print_args(std::ostream& out, const char* delim, Args&&... args)
{
	size_t idx = 0;
	size_t dummy[1 + sizeof...(args)] = { 0, print_1arg(out, idx++, delim, args)... };
	return idx;
}

template<typename Func2TestT, typename FuncRetT, typename ...FuncArgsT>
bool simple_test_return_value(std::ostream & os, Func2TestT&& func, FuncRetT expected_return_value, FuncArgsT&&... func_args) {
	//arrange
	/* <empty> */
	using namespace quoted_string;
	//act
	const FuncRetT actual_return_value = func(std::forward<FuncArgsT>(func_args)...);

	//assert
	bool assert_result = (expected_return_value == actual_return_value);
	if (!assert_result) {
		os << " - NOK\n\texpected != actual,\n\t" << expected_return_value << " != " << actual_return_value << ",\n\tfor test args (" ;
		print_args(os, "\n\t\t", std::forward<FuncArgsT>(func_args)...);
		os << ")" << std::endl;
	} else {
		os << " - OK";
	}

	return assert_result;
}

template<typename Func2TestT, typename ExpRetT, typename CmpRetT, typename ...FuncArgsT>
bool cmp_test_return_value(std::ostream & os, Func2TestT&& func, ExpRetT expected_return_value, CmpRetT cmp, FuncArgsT&&... func_args) {
	//arrange
	/* <empty> */

	//act
	const auto actual_return_value = func(std::forward<FuncArgsT>(func_args)...);

	//assert
	bool assert_result = cmp(expected_return_value, actual_return_value);
	if (!assert_result) {
		os << " - NOK\n\tcomparator pred failed with for expected "<< expected_return_value<< " and actual "<< actual_return_value <<" values,\n\tfor test args (";
		print_args(os, "\n\t\t", std::forward<FuncArgsT>(func_args)...);
		os << ")" << std::endl;
	}
	else {
		os << " - OK";
	}

	return assert_result;
}

template<typename TestValueT>
struct TestCase_t {
	using Val = TestValueT;
	using Arg = const string_utils::string_t;

	Val   expected_value;
	Arg	  text_to_test;
};

template<typename Func2TestT, typename TestCaseT>
bool run_test_cases(std::ostream& os, const std::string& test_name, const Func2TestT& func, const std::vector<TestCaseT> test_cases)
{
	size_t total_cases = test_cases.size(); 
	size_t cases_runned = 0;
	size_t cases_passed = 0; 
	size_t cases_failed = 0;
	for (const TestCaseT& tc : test_cases) {
		os << "\t\tRunning test for " << test_name << " " << ++cases_runned << " of " << total_cases;
		if (!func(os, tc.expected_value, tc.text_to_test )) {
			os << " \tERROR test failed";
			++cases_failed;
			//break;
		} else {
			++cases_passed;
		}
		os << std::endl;
	}
	if (total_cases == cases_passed) {
		os << "\t\tSUCCESS -  all " << cases_passed << " of " << total_cases <<  " test case(s) have PASSED" << std::endl;;
	} else {
		os << "\t\tFAILURE - some " << (cases_failed) << " test case(s) have NOT PASS, and some "<<(total_cases- cases_runned)<<" test case(s) have NOT RUNNED of total " << total_cases <<" test case(s)"<< std::endl;;
	}
	return total_cases == cases_passed;
}

std::ostream& word_count_tests(std::ostream & os)
{
	TEST_HEADER("word count func")

	using test_case = TestCase_t <int>;

	std::vector<test_case> test_cases = {
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

	auto test_func_adapter = [&](std::ostream & os, test_case::Val val, const test_case::Arg& arg) -> bool {
		return simple_test_return_value(os, string_utils::word_count, val, arg);
	};

	bool result = run_test_cases(os, test_name, test_func_adapter, test_cases);
	TEST_FOOTER(result)
	return os;
}

std::ostream& text_stats_tests(std::ostream & os)
{
	TEST_HEADER("text stats func")

	using test_case = TestCase_t <string_utils::text_stats_t>;

	std::vector<test_case> test_cases = {
	    { { { "hello", {0}   }, { "world",  {6} } }, "hello world" },
    	{ { { "hello", {0}   }, { "world",  {6} } }, "hello world      " },
		{ { { "hello", {8}   }, { "world", {14} } }, "        hello world" },
		{ { { "world", {8,20}}, { "hello", {14} } }, "        world hello world" },
		{ {}, " " },
		{ {}, "                " },
		{ {}, "" },
		{ { { "hello", {0}   }, { "world", {18} } }, "hello             world\0 to you" }
	};

	auto test_func_adapter = [&](std::ostream & os, test_case::Val val, const test_case::Arg& arg) -> bool {
		return simple_test_return_value(os, string_utils::text_stats, val, arg);
	};

	bool result = run_test_cases(os, test_name, test_func_adapter, test_cases);
	TEST_FOOTER(result)
	return os;
}

std::ostream& get_longest_word_tests(std::ostream & os)
{
	TEST_HEADER("get longest words func")

	using test_case = TestCase_t <string_utils::string_list_t>;

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
	
	auto test_func_adapter = [&](std::ostream & os, test_case::Val val, const test_case::Arg& arg) -> bool {
		return simple_test_return_value(os, string_utils::get_longest_words, val, arg);
	};

	bool result = run_test_cases(os, test_name, test_func_adapter, test_cases);
	TEST_FOOTER(result)
	return os;
}

std::ostream& get_most_bloat_words_tests(std::ostream & os)
{
	TEST_HEADER("get most bloat words func")

	using test_case = TestCase_t <string_utils::string_list_t>;

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

	auto test_func_adapter = [&](std::ostream & os, test_case::Val val, const test_case::Arg& arg) -> bool {
		return simple_test_return_value(os, string_utils::get_most_bloat_words, val, arg);
	};

	bool result = run_test_cases(os, test_name, test_func_adapter, test_cases);
	TEST_FOOTER(result)
	return os;
}

std::ostream& reverse_words_tests(std::ostream & os)
{
	TEST_HEADER("reverse words func")

	using test_case = TestCase_t <string_utils::string_list_t>;

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

	auto test_func_adapter = [&](std::ostream & os, test_case::Val val, const test_case::Arg& arg) -> bool {
		return simple_test_return_value(os, string_utils::reverse_words, val, arg);
	};

	bool result = run_test_cases(os, test_name, test_func_adapter, test_cases);
	TEST_FOOTER(result)
	return os;
}

std::ostream& get_longest_symbol_run_tests(std::ostream & os)
{
	TEST_HEADER("get longest symbol run func")

	using test_case = TestCase_t <size_t>;

	std::vector<test_case> test_cases = {
		{ 2 , "hello" },
		{ 3,  "hellooo" },
		{ 1, "a" },
		{ 0, "" },
		{ 3, "aaa" },
		{ 3 , "abbccddbbaccc" },
	};

	auto test_func_adapter = [&](std::ostream & os, test_case::Val val, const test_case::Arg& arg) -> bool {
		return simple_test_return_value(os, string_utils::get_longest_symbol_run, val, arg);
	};

	bool result = run_test_cases(os, test_name, test_func_adapter, test_cases);
	TEST_FOOTER(result)
	return os;
}

std::ostream& split_text_to_chukns_tests(std::ostream & os)
{
	TEST_HEADER("split text to chnks func")

	using test_case = TestCase_t <string_utils::string_list_t>;

	std::vector<test_case> test_cases = {
		{ {"hello"} , "hello" },
		{ {"hellooo"},  "hellooo" },
		{ {"a"} , "a" },
		{ {}, "" },
		{ { "          ", "  aaa"}, "            aaa" },
		{ { "abbccddbbaccc" } , "abbccddbbaccc" },
		{ { "ab bcc", " ddbbaccc ", "        ", " hello", " world" } , "ab bcc ddbbaccc          hello world" }
	};

	auto cmp =  [](const string_utils::string_list_t& s_lst, const string_utils_mt::chunk_list_t& ch_lst) -> bool {
		if (s_lst.size() != ch_lst.size()) return false;
		auto sit = s_lst.begin();
		auto cit = ch_lst.begin();
		for (; sit != s_lst.end() && cit != ch_lst.end(); ++sit, ++cit) {
			if (*sit != string_utils::string_t(cit->chunk_text + cit->chunk_start, cit->chunk_length)) return false;
		}
		return true;
	};

	auto test_func_adapter = [&](std::ostream & os, test_case::Val val, const test_case::Arg& arg) -> bool {
		return cmp_test_return_value(os, string_utils_mt::split_text_to_chunks_by_word, val, cmp, arg, 10);
	};

	bool result = run_test_cases(os, test_name, test_func_adapter, test_cases);
	TEST_FOOTER(result)
	return os;
}



std::ostream& word_count_tests_mt(std::ostream & os)
{
	TEST_HEADER("word count mt func")

		using test_case = TestCase_t <int>;

	std::vector<test_case> test_cases = {
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

	auto test_func_adapter = [&](std::ostream & os, test_case::Val val, const test_case::Arg& arg) -> bool {
		return simple_test_return_value(os, string_utils_mt::word_count_mt, val, arg, 4);
	};

	bool result = run_test_cases(os, test_name, test_func_adapter, test_cases);
	TEST_FOOTER(result)
		return os;
}


std::ostream& text_stats_mt_tests(std::ostream & os)
{
	TEST_HEADER("text stats mt func")

	using test_case = TestCase_t <string_utils::text_stats_t>;

	std::vector<test_case> test_cases = {
		{ { { "hello",{ 0 } },{ "world",{ 6 } } }, "hello world" },
		{ { { "hello",{ 0 } },{ "world",{ 6 } } }, "hello world      " },
		{ { { "hello",{ 8 } },{ "world",{ 14 } } }, "        hello world" },
		{ { { "world",{ 8,20 } },{ "hello",{ 14 } } }, "        world hello world" },
		{ {}, " " },
		{ {}, "                " },
		{ {}, "" },
		{ { { "hello",{ 0 } },{ "world",{ 18 } } }, "hello             world\0 to you" }
	};

	auto test_func_adapter = [&](std::ostream & os, test_case::Val val, const test_case::Arg& arg) -> bool {
		return simple_test_return_value(os, string_utils_mt::text_stats_mt, val, arg, 4);
	};

	bool result = run_test_cases(os, test_name, test_func_adapter, test_cases);
	TEST_FOOTER(result)
		return os;
}

std::ostream& text_stats_mt_vs_st_tests(std::ostream & os)
{
	TEST_HEADER("text stats mt vs st func")
	string_utils::text_stats_t st_stats_for_lorem_ipsum = string_utils::text_stats(lorem_ipsum_wc123);
	string_utils::text_stats_t mt_stats_for_lorem_ipsum = string_utils_mt::text_stats_mt(lorem_ipsum_wc123);
	bool result = st_stats_for_lorem_ipsum == mt_stats_for_lorem_ipsum;
	//os << mt_stats_for_lorem_ipsum << std::endl;

	TEST_FOOTER(result)
		return os;
}