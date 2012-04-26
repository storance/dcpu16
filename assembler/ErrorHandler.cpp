#include "ErrorHandler.hpp"

using namespace std;
using namespace dcpu::lexer;

namespace dcpu {
	ErrorHandler::ErrorHandler() : _totalErrors(0), _totalWarnings(0), _out(cerr) {}

	ErrorHandler::ErrorHandler(ostream &out) : _totalErrors(0), _totalWarnings(0), _out(out) {}

	void ErrorHandler::error(const Location &location, const string &message) {
		++_totalErrors;
		_out << location << ": error: " << message << endl;
	}

	void ErrorHandler::error(const Location& location, const boost::basic_format<char> &fmt) {
		error(location, str(fmt));
	}

	void ErrorHandler::errorUnexpectedToken(TokenPtr& token, char c) {
		error(token->location, boost::format("Unexpected token '%s'; expected '%s'") % token->content % c);
	}

	void ErrorHandler::errorUnexpectedToken(TokenPtr& token, const std::string &expected) {
		error(token->location, boost::format("Unexpected token '%s'; expected %s") % token->content % expected);
	}

	void ErrorHandler::warning(const Location &location, const string &message) {
		++_totalWarnings;
		_out << location << ": warning: " << message << endl;
	}

	void ErrorHandler::warning(const Location& location, const boost::basic_format<char> &fmt) {
		warning(location, str(fmt));
	}

	bool ErrorHandler::hasErrors() {
		return _totalErrors > 0;
	}

	void ErrorHandler::summary() {
		if (_totalErrors > 0) {
			_out << _totalErrors << " error(s)" << endl;
		}

		if (_totalWarnings > 0) {
			_out << _totalWarnings << " warning(s)" << endl;
		}
	}
}
