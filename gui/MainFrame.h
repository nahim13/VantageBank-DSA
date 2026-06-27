#pragma once
#include <wx/wx.h>
#include <wx/listctrl.h>
#include "bank.h"

class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString &title, Bank *bank);

private:
    Bank *bankPtr;
    wxListCtrl *listCtrl;
    void RefreshList();
    void OnApplyInterest(wxCommandEvent &e);
    void OnStatement(wxCommandEvent &e);
    void OnCreate(wxCommandEvent &e);
};