#pragma once

#include <ostream>

std::ostream& word_count_tests(std::ostream& os);
std::ostream& text_stats_tests(std::ostream& os);
std::ostream& get_longest_word_tests(std::ostream& os);
std::ostream& get_most_bloat_words_tests(std::ostream& os);
std::ostream& get_longest_symbol_run_tests(std::ostream& os);
std::ostream& reverse_words_tests(std::ostream& os);

std::ostream& split_text_to_chukns_tests(std::ostream& os);
std::ostream& word_count_tests_mt(std::ostream& os);
std::ostream& text_stats_mt_tests(std::ostream& os);

std::ostream& text_stats_mt_vs_st_tests(std::ostream& os);