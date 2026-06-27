#pragma once
#include <wx/wx.h>
#include <wx/listctrl.h>
#include "bank.h"

class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString &title, Bank *bank);

private:
    Bank *bankPtr;           // non-owning; lifetime managed by MainApp
    wxListCtrl *listCtrl;

    void RefreshList();
    void OnCreate(wxCommandEvent &e);
    void OnDeposit(wxCommandEvent &e);      // FIX #2 (GUI): added
    void OnWithdraw(wxCommandEvent &e);     // FIX #2 (GUI): added
    void OnTransfer(wxCommandEvent &e);     // FIX #2 (GUI): added
    void OnApplyInterest(wxCommandEvent &e);
    void OnStatement(wxCommandEvent &e);
};
