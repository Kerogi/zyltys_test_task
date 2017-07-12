#include <iostream> 
#include <iomanip>
#include "string_utils.h"
#include "test_utils.h"

using namespace std;

int main() {
	cout << "word count tests: " << boolalpha << word_count_tests(cout) << endl;
	cout << "get longest word tests: " << boolalpha << get_longest_word_tests(cout) << endl;
	return 0;
}