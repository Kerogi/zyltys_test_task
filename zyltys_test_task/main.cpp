#include <iostream> 
#include <iomanip>
#include "string_utils.h"
#include "test_utils.h"

using namespace std;

int main() {
	/*cout << word_count_tests << endl;
	cout << get_longest_word_tests << endl;
	cout << get_longest_symbol_run_tests << endl;
	cout << get_most_bloat_words_tests << endl;
	cout << reverse_words_tests << endl;
	*/
	cout << text_stats_tests << endl;

	cout << split_text_to_chukns_tests << endl;
	cout << word_count_tests_mt << endl;	
	cout << text_stats_mt_tests << endl;

	cout << word_count_mt_vs_st_tests << endl;
	cout << text_stats_mt_vs_st_tests << endl;
	return 0;
}