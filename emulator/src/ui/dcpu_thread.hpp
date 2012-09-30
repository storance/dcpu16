#pragma once

#include <wx/wx.h>
#include <atomic>
#include <thread>
#include <memory>

#include "../dcpu.hpp"

namespace dcpu { namespace emulator {
	DECLARE_EVENT_TYPE(wxEVT_COMMAND_DCPU_STOPPED, wxID_ANY);

	class DcpuThread {
		DcpuThread(DcpuThread const&) = delete;
    	DcpuThread& operator =(DcpuThread const&) = delete;

		Dcpu &cpu;
		wxEvtHandler* eventHandler;
		std::atomic<bool> stopExecution;
		uint64_t cyclesSlept;
		std::thread thread;
        
        void sleepUntilNextCycle();
        void notifyStopped();
        uint64_t getCurrentTime();
        void sleep(uint64_t time);
	public:
		DcpuThread(Dcpu &cpu, wxEvtHandler* eventHandler);

		void run();
		void start();
		void stop();
	};
}}
