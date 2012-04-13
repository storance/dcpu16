#pragma once

#include <iostream>
#include <string>
#include <tuple>

enum ErrorType {
	INVALID_STATEMENT,
	UNTERMINATED_INSTRUCTION,
	MISSING_FIRST_ARGUMENT,
	MISSING_SECOND_ARGUMENT,
	UNMATCHED_BRACKET,
	UNMATCHED_PARENTHESIS,
};

template<typename Iterator>
struct ErrorHandler {
	template <typename, typename, typename>
	struct result { typedef void type; };

	ErrorHandler(const std::string &filename, Iterator first, Iterator last)
		: filename(filename), first(first), last(last) {}

	 template <typename Message, typename What>
	 void operator() (ErrorType errorType, What const& what, Iterator err_pos) const {
		uint32_t line, column;

		std::tie(line, column) = getPosition(err_pos);

		std::cerr << filename << ":" << line << ":" << column << ": " << message << what << std::endl;
	}

	std::tuple<uint32_t, uint32_t> getPosition(Iterator pos) const {
		uint32_t line = 1, column = 1;

		for (Iterator it = first; it != pos;) {
			bool foundEOL = false;
			if (*it == '\r') {
				foundEOL = true;
				++it;
			}

			if (it != pos && *it == '\n') {
				foundEOL = true;
				++it;
			}

			if (foundEOL) {
				++line;
				column = 1;
			} else {
				++column;
				++it;
			}
		}

		return std::tuple<uint32_t, uint32_t>(line, column);
	}

	std::string filename;
	Iterator first, last;

};