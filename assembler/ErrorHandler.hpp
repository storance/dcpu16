#pragma once

#include <string>
#include <cstdint>
#include <iostream>
#include <memory>

#include <boost/format.hpp>

#include "Token.hpp"
#include "Types.hpp"

namespace dcpu {
	class ErrorHandler {
	private:
		unsigned int _totalErrors;
		unsigned int _totalWarnings;

		std::ostream &_out;
	public:
		void error(const lexer::Location&, const std::string&);
		void error(const lexer::Location&, const boost::basic_format<char> &);
		void warning(const lexer::Location&, const std::string&);
		void warning(const lexer::Location&, const boost::basic_format<char> &);

		void errorUnexpectedToken(TokenPtr& token, char c);
		void errorUnexpectedToken(TokenPtr& token, const std::string &expected);

		bool hasErrors();
		void summary();

		ErrorHandler();
		ErrorHandler(std::ostream &);
	};
}
