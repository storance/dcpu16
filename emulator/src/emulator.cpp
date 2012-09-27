#include <wx/filename.h>
#include <wx/strconv.h>

#include <iostream>

#include "emulator.hpp"
#include "dcpu.hpp"

using namespace std;

BEGIN_EVENT_TABLE(EmulatorFrame, wxFrame)
    EVT_MENU(ID_Quit, EmulatorFrame::OnQuit)
    EVT_MENU(ID_Open, EmulatorFrame::OnOpen)
END_EVENT_TABLE()

IMPLEMENT_APP(EmulatorApp)

bool EmulatorApp::OnInit() {
    EmulatorFrame *frame = new EmulatorFrame( _("DCPU-16 Emulator"), wxPoint(50, 50), wxSize(450, 340));
    frame->Show(true);
    SetTopWindow(frame);

    return true;
}

EmulatorFrame::EmulatorFrame(const wxString &title, const wxPoint &pos, const wxSize &size) 
        : wxFrame(NULL, -1, title, pos, size), cpu() {
    wxMenu *menuFile = new wxMenu;

    menuFile->Append(ID_Open, _("&Open"));
    menuFile->AppendSeparator();
    menuFile->Append(ID_Quit, _("E&xit"));

    wxMenu *menuEmulator = new wxMenu;
    menuEmulator->Append(ID_Run, _("&Run"));
    menuEmulator->Append(ID_Stop, _("&Stop"));
    menuEmulator->AppendSeparator();
    menuEmulator->Append(ID_Pause, _("&Pause"));
    menuEmulator->Append(ID_Resume, _("&Resume"));
    
    menuEmulator->Enable(ID_Stop, false);
    menuEmulator->Enable(ID_Pause, false);
    menuEmulator->Enable(ID_Resume, false);


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
    }
}

/*size_t load(dcpu::emulator::dcpu &cpu, const char *filename) {
    cpu.registers.pc = 0;

    ifstream file;
    
    file.open(filename);
    if (!file) {
        throw runtime_error(str(boost::format("Failed to open the file %s: %s") 
                % filename % strerror(errno)));
    }

    size_t index = 0;
    while (file && index <= 65536) {
        uint8_t b1 = file.get();
        uint8_t b2 = file.get();
        if (file.bad()) {
            throw runtime_error(str(boost::format("Failed to read the next "
                "word from the file %s: %s") % filename 
                % strerror(errno)));
        }

        cpu.memory[index++] = (b1 << 8) | b2;
    }
    file.close();

    return index;
}


int main(int argc, char **argv) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " </path/to/dcpu/program>" << endl;
        return 1;
    }

    dcpu::emulator::dcpu cpu;

    try {
        load(cpu, argv[1]);
        cpu.run();
        cpu.dump(cout);
    } catch (exception &e) {
        cerr << "Error: " << e.what() << endl;

        return 1;
    }
}*/