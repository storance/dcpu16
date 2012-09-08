#include <wx/wx.h>

class EmulatorApp : public wxApp {
    virtual bool OnInit();
};

class EmulatorFrame : public wxFrame {
public:
    EmulatorFrame(const wxString &title, const wxPoint &pos, const wxSize& size);
    
    void OnQuit(wxCommandEvent &event);

    DECLARE_EVENT_TABLE()
};

enum {
    ID_Quit = 1
};
