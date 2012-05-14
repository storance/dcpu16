#pragma once

#include <string>
#include <cstdint>
#include <iostream>
#include <memory>
#include <set>

#include <boost/format/format_fwd.hpp>

#include "token.hpp"

namespace dcpu { namespace logging {
	class log {
	private:
		unsigned int total_errors;
		unsigned int total_warnings;

		std::ostream &out;
	public:
		void error(const lexer::location_ptr&, const std::string&);
		void error(const lexer::location_ptr&, const boost::basic_format<char> &);
		void warning(const lexer::location_ptr&, const std::string&);
		void warning(const lexer::location_ptr&, const boost::basic_format<char> &);

		void unexpected_token(const lexer::token& token, char c);
		void unexpected_token(const lexer::token& token, const std::string &expected);

		bool has_warnings();
		bool has_errors();
		void summary();

		log();
		log(std::ostream &);
	};
}}
