#include "hardware.hpp"

using namespace std;

namespace dcpu { namespace emulator {
	HardwareDevice::HardwareDevice(Dcpu &cpu, uint32_t manufacturerId, uint32_t hardwareId, uint16_t version) 
		: cpu(cpu), manufacturerId(manufacturerId), hardwareId(hardwareId), version(version) {

	}

	HardwareDevice::~HardwareDevice() {

	}

	uint32_t HardwareDevice::getHardwareId() {
		return hardwareId;
	}

	uint32_t HardwareDevice::getManufacturerId() {
		return manufacturerId;
	}

	uint16_t HardwareDevice::getVersion() {
		return version;
	}
}}