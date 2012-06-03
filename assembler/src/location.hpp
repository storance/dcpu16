#pragma once

#include <string>
#include <ostream>
#include <memory>
#include <cstdint>

namespace dcpu { namespace assembler {
	struct location {
		std::string source;
		uint32_t line;
		uint32_t column;

		location();
		location(const std::string&, uint32_t, uint32_t);

		bool operator==(const location& other) const;
	};

	
	typedef std::shared_ptr<location> location_ptr;
	std::ostream& operator<< (std::ostream& stream, const location_ptr& location);
	bool operator==(const location_ptr &, const location_ptr &);
}}
