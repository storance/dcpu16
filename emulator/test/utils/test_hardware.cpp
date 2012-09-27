#include "test_hardware.hpp"

using dcpu::emulator::HardwareDevice;

TestHardware::TestHardware(dcpu::emulator::Dcpu &cpu) : HardwareDevice(cpu, MANUFACTURER_ID, HARDWARE_ID, VERSION),
		interruptCalled(false) {

}

void TestHardware::tick() {

}

uint16_t TestHardware::interrupt() {
	interruptCalled = true;

	return 0;
}