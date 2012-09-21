#pragma once

#include "dcpu.hpp"

namespace dcpu { namespace emulator {

	class hardware_device {
		dcpu &cpu;
		uint32_t manufacturer_id;
		uint32_t hardware_id;
		uint16_t version;
	public:
		hardware_device(dcpu &cpu, uint32_t manufacturer_id, uint32_t hardware_id, uint16_t version);
		virtual ~hardware_device();

		virtual void tick()=0;
		virtual uint16_t interrupt()=0;

		uint32_t get_hardware_id();
		uint32_t get_manufacturer_id();
		uint16_t get_version();
	};
}}
