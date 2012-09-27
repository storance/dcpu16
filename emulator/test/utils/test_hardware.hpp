#pragma once

#include "dcpu.hpp"
#include "hardware.hpp"

class TestHardware : public dcpu::emulator::HardwareDevice {
	enum { HARDWARE_ID = 0x01020304, MANUFACTURER_ID = 0x1a2b3c4d, VERSION = 0x03};
public:
	bool interruptCalled;
	TestHardware(dcpu::emulator::Dcpu &cpu);
	
	virtual void tick();
	virtual uint16_t interrupt();
};