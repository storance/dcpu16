#include "ErrorHandler.hpp"

using namespace std;
using namespace dcpu::lexer;

namespace dcpu {
	ErrorHandler::ErrorHandler() : totalErrors(0), totalWarnings(0), out(cerr) {}

	ErrorHandler::ErrorHandler(ostream &out) : totalErrors(0), totalWarnings(0), out(out) {}

	void ErrorHandler::error(const location_t &location, const string &message) {
		++totalErrors;
		out << location << ": error: " << message << endl;
	}

	void ErrorHandler::error(const location_t &location, const boost::basic_format<char> &fmt) {
		error(location, str(fmt));
	}

	void ErrorHandler::errorUnexpectedToken(const Token& token, char c) {
		error(token.location, boost::format("unexpected token '%s'; expected '%c'") % token.content % c);
	}

	void ErrorHandler::errorUnexpectedToken(const Token& token, const std::string &expected) {
		error(token.location, boost::format("unexpected token '%s'; expected %s") % token.content % expected);
	}

	void ErrorHandler::warning(const location_t &location, const string &message) {
		++totalWarnings;
		out << location << ": warning: " << message << endl;
	}

	void ErrorHandler::warning(const location_t &location, const boost::basic_format<char> &fmt) {
		warning(location, str(fmt));
	}

	bool ErrorHandler::hasErrors() {
		return totalErrors > 0;
	}

	bool ErrorHandler::hasWarnings() {
		return totalWarnings > 0;
	}

	void ErrorHandler::summary() {
		if (totalErrors > 0) {
			out << totalErrors << " error(s)" << endl;
		}

		if (totalWarnings > 0) {
			out << totalWarnings << " warning(s)" << endl;
		}
	}
}
