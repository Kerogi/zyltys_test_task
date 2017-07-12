#pragma once

#include <ostream>
#include <list>
#include <vector>

template<class ItemT>
std::ostream& operator<<(std::ostream& os, const std::list< ItemT > & list) {
	os << '(';
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