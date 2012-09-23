#include "test_hardware.hpp"

using dcpu::emulator::hardware_device;

test_hardware::test_hardware(dcpu::emulator::dcpu &cpu) : hardware_device(cpu, MANUFACTURER_ID, HARDWARE_ID, VERSION),
		interrupt_called(false) {

}

void test_hardware::tick() {

}

uint16_t test_hardware::interrupt() {
	interrupt_called = true;

	return 0;
}