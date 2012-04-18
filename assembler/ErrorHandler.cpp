#include "ErrorHandler.hpp"

#include <iostream>

using namespace std;

namespace dcpu {
	void ErrorHandler::error(Location location, string message) {
		++totalErrors;
		cerr << location.sourceName << ":" << location.line << ":" << location.column
			<< ": error: " << message << endl;
	}

	void ErrorHandler::warning(Location location, string message) {
		++totalWarnings;
		cerr << location.sourceName << ":" << location.line << ":" << location.column
			<< ": warning: " << message << endl;
	}

	bool ErrorHandler::isFailed() {
		return totalErrors > 0;
	}

	void ErrorHandler::summary() {
		if (totalErrors > 0) {
			cerr << totalErrors << " error(s)" << endl;
		}

		if (totalWarnings > 0) {
			cerr << totalWarnings << " warning(s)" << endl;
		}
	}
}
