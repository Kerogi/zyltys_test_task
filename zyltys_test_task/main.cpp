#include <iostream> 
#include <string>
#include <iomanip>
#include "string_utils.h"
#include "lorem_ipsum.h"
#include "test_utils.h"

using namespace std;


int main() {
	ASSERT_WC(cout, "hello world", 2);
	ASSERT_WC(cout, "hello world      ", 2);
	ASSERT_WC(cout, "        hello world", 2);
	ASSERT_WC(cout, "h e l l o w o r l d", 10);
	ASSERT_WC(cout, "h", 1);
	ASSERT_WC(cout, "hello", 1);
	ASSERT_WC(cout, "hello             world to you", 4);
	ASSERT_WC(cout, " ", 0);
	ASSERT_WC(cout, "                ", 0);
	ASSERT_WC(cout, "", 0);
	ASSERT_WC(cout, "hello             world\0 to you", 2);
	ASSERT_WC(cout, lorem_ipsum_wc123, 123);
	return 0;
}