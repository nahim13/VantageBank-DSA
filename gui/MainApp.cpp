#include "MainApp.h"
#include "MainFrame.h"

bool MainApp::OnInit() {
    auto bank = new Bank();
    MainFrame* frame = new MainFrame("VantageBank Management System", bank);
    frame->Show(true);
    return true;
}
IMPLEMENT_APP(MainApp)