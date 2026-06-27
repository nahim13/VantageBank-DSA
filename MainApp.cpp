#include "MainApp.h"
#include "MainFrame.h"

// FIX #1: Bank is owned by MainApp via unique_ptr.
//         MainFrame receives a raw (non-owning) pointer — safe because
//         MainApp outlives all frames in wxWidgets' lifecycle.
bool MainApp::OnInit() {
    bank = std::unique_ptr<Bank>(new Bank());
    MainFrame *frame = new MainFrame("VantageBank Management System", bank.get());
    frame->Show(true);
    return true;
}

int MainApp::OnExit() {
    // unique_ptr destructor releases Bank here, after all frames are gone
    return 0;
}

IMPLEMENT_APP(MainApp)
