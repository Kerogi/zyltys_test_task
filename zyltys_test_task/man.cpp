#include <iostream> 
#include <string>

#include "string_utils.h"
using namespace std;

int main() {
	string text("hello world");
	cout << text << endl;
	cout << "words: "<< string_utils::count_words(text) << endl;
	return 0;
}