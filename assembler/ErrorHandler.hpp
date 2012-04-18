#pragma once

#include <string>
#include <cstdint>

#include "Token.hpp"

namespace dcpu {
	class ErrorHandler {
	private:
		std::uint32_t totalErrors;
		std::uint32_t totalWarnings;
	public:
		void error(Location, std::string);
		void warning(Location, std::string);

		bool isFailed();
		void summary();
	};

}
