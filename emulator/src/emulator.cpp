#include <wx/filename.h>
#include <wx/strconv.h>

#include <iostream>

#include "emulator.hpp"
#include "dcpu.hpp"

using namespace std;
using namespace dcpu::emulator;

BEGIN_EVENT_TABLE(EmulatorFrame, wxFrame)
    EVT_MENU(ID_Quit, EmulatorFrame::OnQuit)
    EVT_MENU(ID_Open, EmulatorFrame::OnOpen)
    EVT_MENU(ID_Start, EmulatorFrame::OnStart)
    EVT_MENU(ID_Stop, EmulatorFrame::OnStop)
    EVT_COMMAND(wxID_ANY, wxEVT_COMMAND_DCPU_STOPPED, EmulatorFrame::OnDcpuStopped)
END_EVENT_TABLE()

IMPLEMENT_APP(EmulatorApp)

bool EmulatorApp::OnInit() {
    EmulatorFrame *frame = new EmulatorFrame( _("DCPU-16 Emulator"), wxPoint(50, 50), wxSize(450, 340));
    frame->Show(true);
    SetTopWindow(frame);

    return true;
}

EmulatorFrame::EmulatorFrame(const wxString &title, const wxPoint &pos, const wxSize &size) 
        : wxFrame(NULL, -1, title, pos, size), cpu(), cpuThread(cpu, this) {
    wxMenu *menuFile = new wxMenu;

    menuFile->Append(ID_Open, _("&Open"));
    menuFile->AppendSeparator();
    menuFile->Append(ID_Quit, _("E&xit"));

    wxMenu *menuEmulator = new wxMenu;
    menuEmulator->Append(ID_Start, _("&Start"));
    menuEmulator->Append(ID_Stop, _("S&top"));
    
    menuEmulator->Enable(ID_Start, false);
    menuEmulator->Enable(ID_Stop, false);


    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _("&File"));
    menuBar->Append(menuEmulator, _("&Emulator"));

    SetMenuBar(menuBar);
}

void EmulatorFrame::OnQuit(wxCommandEvent & WXUNUSED(event)) {
    Close(TRUE);
}

void EmulatorFrame::OnOpen(wxCommandEvent & WXUNUSED(event)) {
    auto openFileDialog = make_shared<wxFileDialog>(this);
 
    if (openFileDialog->ShowModal() == wxID_OK ) {
        cpu.load(openFileDialog->GetPath().mb_str(wxConvUTF8));
        GetMenuBar()->Enable(ID_Start, true);
    }
}

void EmulatorFrame::OnStart(wxCommandEvent & WXUNUSED(event)) {
    cpuThread.start();

    GetMenuBar()->Enable(ID_Start, false);
    GetMenuBar()->Enable(ID_Stop, true);
}

void EmulatorFrame::OnStop(wxCommandEvent & WXUNUSED(event)) {
    cpuThread.stop();
}

void EmulatorFrame::OnDcpuStopped(wxCommandEvent & WXUNUSED(event)) {
    GetMenuBar()->Enable(ID_Start, true);
    GetMenuBar()->Enable(ID_Stop, false);

    cpuThread.stop();
    cpu.dump(cout);
}