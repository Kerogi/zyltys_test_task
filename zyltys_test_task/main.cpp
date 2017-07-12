#include <iostream> 
#include <string>
#include <iomanip>
#include "string_utils.h"
#include "lorem_ipsum.h"

using namespace std;

inline bool ASSERT_WC(const string& s, const int expected_wc, const size_t max_show_text_length=30) {
	//arrange

	//act
	int actual_wc = string_utils::count_words(s);

	//assert
	if (expected_wc == actual_wc) {
		cout << "OK  expected word count == to actual, " << setw(3) << expected_wc << " == "<< setw(3) << actual_wc << ",    for text '" << ((s.length() <= max_show_text_length) ? s : s.substr(0, max_show_text_length - 3) + "...") << "'" << endl;
		return true;
	}
	else {
		cout << "NOK expected word count != to actual, " << setw(3) << expected_wc << " != " << setw(3) << actual_wc << ",    for text '" << ((s.length()<=max_show_text_length)? s:s.substr(0, max_show_text_length-3) +"...") << "'"<< endl;
		return false;
	}
}
int main() {
	ASSERT_WC("hello world", 2);
	ASSERT_WC("hello world      ", 2);
	ASSERT_WC("        hello world", 2);
	ASSERT_WC("h e l l o w o r l d", 10);
	ASSERT_WC("h", 1);
	ASSERT_WC("hello", 1);
	ASSERT_WC("hello             world to you", 4);
	ASSERT_WC(" ", 0);
	ASSERT_WC("                ", 0);
	ASSERT_WC("", 0);
	ASSERT_WC("hello             world\0 to you", 2);
	ASSERT_WC(lorem_ipsum_wc123, 123);
	return 0;
}