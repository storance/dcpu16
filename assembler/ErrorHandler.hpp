#pragma once

#include <string>
#include <cstdint>
#include <iostream>
#include <memory>

#include <boost/format.hpp>

#include "Token.hpp"

namespace dcpu {
	class ErrorHandler {
	private:
		unsigned int totalErrors;
		unsigned int totalWarnings;

		std::ostream &out;
	public:
		void error(const lexer::location_t&, const std::string&);
		void error(const lexer::location_t&, const boost::basic_format<char> &);
		void warning(const lexer::location_t&, const std::string&);
		void warning(const lexer::location_t&, const boost::basic_format<char> &);

		void errorUnexpectedToken(const lexer::Token& token, char c);
		void errorUnexpectedToken(const lexer::Token& token, const std::string &expected);

		bool hasWarnings();
		bool hasErrors();
		void summary();

		ErrorHandler();
		ErrorHandler(std::ostream &);
	};

	typedef std::shared_ptr<ErrorHandler> error_handler_t;
}
