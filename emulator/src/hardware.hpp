#pragma once

#include "dcpu.hpp"

namespace dcpu { namespace emulator {

	class HardwareDevice {
	protected:
		Dcpu &cpu;
		uint32_t manufacturerId;
		uint32_t hardwareId;
		uint16_t version;
	public:
		HardwareDevice(Dcpu &cpu, uint32_t manufacturerId, uint32_t hardwareId, uint16_t version);
		virtual ~HardwareDevice();

		virtual void tick()=0;
		virtual uint16_t interrupt()=0;

		uint32_t getHardwareId();
		uint32_t getManufacturerId();
		uint16_t getVersion();
	};
}}
