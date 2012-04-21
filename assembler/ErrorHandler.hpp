#pragma once

#include <string>
#include <cstdint>
#include <iostream>

#include <boost/format.hpp>

#include "Token.hpp"

namespace dcpu {
	class ErrorHandler {
	private:
		unsigned int _totalErrors;
		unsigned int _totalWarnings;

		std::ostream &_out;
	public:
		void error(const Location&, const std::string&);
		void error(const Location&, const boost::basic_format<char> &);
		void warning(const Location&, const std::string&);
		void warning(const Location&, const boost::basic_format<char> &);

		bool hasErrors();
		void summary();

		ErrorHandler();
		ErrorHandler(std::ostream &);
	};

}
