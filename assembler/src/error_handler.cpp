#include <boost/format.hpp>

#include "error_handler.hpp"

using namespace std;
using namespace dcpu::lexer;

namespace dcpu {
	error_handler::error_handler() : total_errors(0), total_warnings(0), out(cerr) {}

	error_handler::error_handler(ostream &out) : total_errors(0), total_warnings(0), out(out) {}

	void error_handler::error(const location_ptr &location, const string &message) {
		++total_errors;
		out << location << ": error: " << message << endl;
	}

	void error_handler::error(const location_ptr &location, const boost::basic_format<char> &fmt) {
		error(location, str(fmt));
	}

	void error_handler::unexpected_token(const token& token, char c) {
		error(token.location, boost::format("unexpected token '%s'; expected '%c'") % token.content % c);
	}

	void error_handler::unexpected_token(const token& token, const std::string &expected) {
		error(token.location, boost::format("unexpected token '%s'; expected %s") % token.content % expected);
	}

	void error_handler::warning(const location_ptr &location, const string &message) {
		++total_warnings;
		out << location << ": warning: " << message << endl;
	}

	void error_handler::warning(const location_ptr &location, const boost::basic_format<char> &fmt) {
		warning(location, str(fmt));
	}

	bool error_handler::has_errors() {
		return total_errors > 0;
	}

	bool error_handler::has_warnings() {
		return total_warnings > 0;
	}

	void error_handler::summary() {
		if (total_errors > 0) {
			out << total_errors << " error(s)" << endl;
		}

		if (total_warnings > 0) {
			out << total_warnings << " warning(s)" << endl;
		}
	}
}
