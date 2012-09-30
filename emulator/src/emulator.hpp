#include <wx/wx.h>

#include "dcpu.hpp"
#include "ui/dcpu_thread.hpp"

class EmulatorApp : public wxApp {
    virtual bool OnInit();
};

class EmulatorFrame : public wxFrame {
	dcpu::emulator::Dcpu cpu;
    dcpu::emulator::DcpuThread cpuThread;
public:
    EmulatorFrame(const wxString &title, const wxPoint &pos, const wxSize& size);
    
    void OnQuit(wxCommandEvent &event);
    void OnOpen(wxCommandEvent &event);
    void OnStart(wxCommandEvent &event);
    void OnStop(wxCommandEvent &event);
    
    void OnDcpuStopped(wxCommandEvent &event);

    DECLARE_EVENT_TABLE()
};

enum {
    ID_Quit = 1,
    ID_Open = 2,
    ID_Start = 3,
    ID_Stop = 4,
};
