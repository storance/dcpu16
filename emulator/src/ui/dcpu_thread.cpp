#include <time.h>
#include <iostream>
#include <cstring>
#include <boost/format.hpp>

#include "dcpu_thread.hpp"

using namespace std;
using boost::format;
using boost::str;

static const int64_t SECOND_IN_NS = 1000000000L;
static const int64_t CYCLE_IN_NANOSECONDS = SECOND_IN_NS / dcpu::emulator::Dcpu::FREQUENCY;

namespace dcpu { namespace emulator {
	DEFINE_EVENT_TYPE(wxEVT_COMMAND_DCPU_STOPPED);
	
	DcpuThread::DcpuThread(Dcpu &cpu, wxEvtHandler* eventHandler) : cpu(cpu), eventHandler(eventHandler),
		stopExecution(false), cyclesSlept(-1) {
	}

	void DcpuThread::run() {
		try {
			while (!stopExecution && !cpu.isOnFire()) {
				sleepUntilNextCycle();

				if (cyclesSlept >= cpu.getCycles()) {
					cpu.tick();
				}

				cpu.hardwareManager.tickAll();
			}
		} catch (exception &e) {
			cerr << "Error: " << e.what() << endl;
		}

		notifyStopped();
	}

	void DcpuThread::start() {
		stopExecution = false;
		thread = std::thread(&DcpuThread::run, this);
	}

	void DcpuThread::stop() {
        stopExecution = true;
        if (thread.joinable()) {
        	thread.join();
        }
	}

	void DcpuThread::sleepUntilNextCycle() {
		uint64_t currentTime = getCurrentTime();

		if (currentTime % CYCLE_IN_NANOSECONDS != 0) {
			uint64_t sleepUntil = ((currentTime / CYCLE_IN_NANOSECONDS) + 1) * CYCLE_IN_NANOSECONDS;
			this->sleep(sleepUntil);
		}

		++cyclesSlept;
	}

	uint64_t DcpuThread::getCurrentTime() {
		timespec currentTime;
		if (clock_gettime(CLOCK_REALTIME, &currentTime) != 0) {
			throw logic_error(str(format("clock_gettime: %s") % strerror(errno)));
		}

		return (currentTime.tv_sec * SECOND_IN_NS) + currentTime.tv_nsec;
	}


    void DcpuThread::sleep(uint64_t sleepTime) {
    	timespec sleepUntil;
		sleepUntil.tv_sec = sleepTime / SECOND_IN_NS;
		sleepUntil.tv_nsec = sleepTime % SECOND_IN_NS;

		// keep sleeping until we don't get an interrupted error code
		int errorCode;
		while ((errorCode = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &sleepUntil, NULL)) == EINTR);
		if (errorCode != 0) {
			throw logic_error(str(format("clock_nanosleep: %s") % strerror(errorCode)));
		}
    }

	void DcpuThread::notifyStopped() {
		wxCommandEvent stoppedEvent(wxEVT_COMMAND_DCPU_STOPPED);
		eventHandler->AddPendingEvent(stoppedEvent);
	}
}}
