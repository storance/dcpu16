#include "hardware.hpp"

using namespace std;

namespace dcpu { namespace emulator {
	hardware_device::hardware_device(dcpu &cpu, uint32_t manufacturer_id, uint32_t hardware_id, uint16_t version) 
		: cpu(cpu), manufacturer_id(manufacturer_id), hardware_id(hardware_id), version(version) {

	}

	hardware_device::~hardware_device() {

	}

	uint32_t hardware_device::get_hardware_id() {
		return hardware_id;
	}

	uint32_t hardware_device::get_manufacturer_id() {
		return manufacturer_id;
	}

	uint16_t hardware_device::get_version() {
		return version;
	}
}}