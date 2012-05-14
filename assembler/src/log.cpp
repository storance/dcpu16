#include <boost/format.hpp>

#include "log.hpp"

using namespace std;
using namespace dcpu::lexer;

namespace dcpu { namespace logging {
	log::log() : total_errors(0), total_warnings(0), out(cerr) {}

	log::log(ostream &out) : total_errors(0), total_warnings(0), out(out) {}

	void log::error(const location_ptr &location, const string &message) {
		++total_errors;
		out << location << ": error: " << message << endl;
	}

	void log::error(const location_ptr &location, const boost::basic_format<char> &fmt) {
		error(location, str(fmt));
	}

	void log::unexpected_token(const token& token, char c) {
		error(token.location, boost::format("unexpected token '%s'; expected '%c'") % token.content % c);
	}

	void log::unexpected_token(const token& token, const std::string &expected) {
		error(token.location, boost::format("unexpected token '%s'; expected %s") % token.content % expected);
	}

	void log::warning(const location_ptr &location, const string &message) {
		++total_warnings;
		out << location << ": warning: " << message << endl;
	}

	void log::warning(const location_ptr &location, const boost::basic_format<char> &fmt) {
		warning(location, str(fmt));
	}

	void log::divide_by_zero(const location_ptr &location) {
		if (warned_divide_by_zero.count(location)) {
			return;
		}

		warned_divide_by_zero.insert(location);
		warning(location, "division or modulo by zero");
	}

	bool log::has_errors() {
		return total_errors > 0;
	}

	bool log::has_warnings() {
		return total_warnings > 0;
	}

	void log::summary() {
		if (total_errors > 0) {
			out << total_errors << " error(s)" << endl;
		}

		if (total_warnings > 0) {
			out << total_warnings << " warning(s)" << endl;
		}
	}
}}
