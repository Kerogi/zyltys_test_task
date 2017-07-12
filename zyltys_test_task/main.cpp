#include <iostream> 
#include <iomanip>
#include "string_utils.h"
#include "test_utils.h"

using namespace std;

int main() {
	cout << "word count tests: " << boolalpha << word_count_tests(cout) << endl;
	cout << "get longest word tests: " << boolalpha << get_longest_word_tests(cout) << endl;
	cout << "get longest run tests: " << boolalpha << get_longest_symbol_run_tests(cout) << endl;
	cout << "get most bloat words tests: " << boolalpha << get_most_bloat_words_tests(cout) << endl;
	cout << "get reverse words tests: " << boolalpha << reverse_words_tests(cout) << endl;
	return 0;
}