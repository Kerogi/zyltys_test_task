#pragma once

#include <ostream>
#include <list>
#include <vector>
#include <string>

namespace quoted_string {
	inline std::ostream& operator<<(std::ostream& os, const std::string& s) {
		std::streamsize width = os.width();
		size_t max_text_length = (size_t)width;
		if (max_text_length == 0) max_text_length = 30;
		os.width(0);
		os << "'";
		if (s.length() <= max_text_length) {
			os << s.c_str();
		}
		else {
			os << s.substr(0, max_text_length - 3).c_str() << "...";
		}
		os << "'";
		os.width(width);
		return os;
	}
}


template<class ItemT>
std::ostream& operator<<(std::ostream& os, const std::list< ItemT > & list) {
	using namespace quoted_string; os << '(';
	if (!list.empty()) {
		bool first = true;
		for (const ItemT& item : list) {
			if (!first) os << ", ";
			os << item;
			first = false;
		}
	}
	os << ')';
	return os;
}


template<class ItemT>
std::ostream& operator<<(std::ostream& os, const std::vector< ItemT > & vec) {
	os << '[';
	if (!vec.empty()) {
		bool first = true;
		for (const ItemT& item : vec) {
			if (!first) os << ", ";
			os << item;
			first = false;
		}
	}
	os << ']';
	return os;
}
