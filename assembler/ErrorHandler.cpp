#include "ErrorHandler.hpp"

using namespace std;

namespace dcpu {
	ErrorHandler::ErrorHandler() : _out(cerr) {}

	ErrorHandler::ErrorHandler(ostream &out) : _out(out) {}

	void ErrorHandler::error(const Location &location, const string &message) {
		++_totalErrors;
		_out << location << ": error: " << message << endl;
	}

	void ErrorHandler::error(const Location& location, const boost::basic_format<char> &fmt) {
		error(location, str(fmt));
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
