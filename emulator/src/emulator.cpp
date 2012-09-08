#include "emulator.h"

BEGIN_EVENT_TABLE(EmulatorFrame, wxFrame)
    EVT_MENU(ID_Quit, EmulatorFrame::OnQuit)
END_EVENT_TABLE()

IMPLEMENT_APP(EmulatorApp)

bool EmulatorApp::OnInit() {
    EmulatorFrame *frame = new EmulatorFrame( _("DCPU-16 Emulator"), wxPoint(50, 50), wxSize(450, 340));
    frame->Show(true);
    SetTopWindow(frame);

    return true;
}

EmulatorFrame::EmulatorFrame(const wxString &title, const wxPoint &pos, const wxSize &size) : wxFrame(NULL, -1, title, pos, size) {
    wxMenu *menuFile = new wxMenu;

    menuFile->Append(ID_Quit, _("E&xit"));

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _("&File"));

    SetMenuBar(menuBar);
}

void EmulatorFrame::OnQuit(wxCommandEvent & WXUNUSED(event)) {
    Close(TRUE);
}
