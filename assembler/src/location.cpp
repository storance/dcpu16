#include "location.hpp"

#include <boost/format.hpp>

using namespace std;

namespace dcpu { namespace assembler {
	location::location(const std::string &source, uint32_t line, uint32_t column)
		: source(source), line(line), column(column) {}

	location::location() : source(""), line(0), column(0) {}

	bool location::operator==(const location& other) const {
		return source == other.source && line == other.line && column == other.column;
	}

	ostream& operator<< (ostream& stream, const location_ptr& location) {
		return stream << boost::format("%s:%d:%d") % location->source % location->line % location->column;
	}

	bool operator==(const location_ptr &left, const location_ptr &right) {
		if (left && right) {
			return *left == *right;
		} else {
			return !left == !right;
		}
	}
}}
