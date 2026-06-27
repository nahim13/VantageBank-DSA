#include "MainFrame.h"

enum
{
    ID_INT = 200,
    ID_STMT,
    ID_NEW
};

MainFrame::MainFrame(const wxString &title, Bank *bank)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800, 500)), bankPtr(bank)
{

    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
    listCtrl = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
    listCtrl->InsertColumn(0, "ID", 0, 50);
    listCtrl->InsertColumn(1, "Name", 0, 150);
    listCtrl->InsertColumn(2, "Tier", 0, 100);
    listCtrl->InsertColumn(3, "Balance", wxLIST_FORMAT_RIGHT, 100);

    mainSizer->Add(listCtrl, 1, wxEXPAND | wxALL, 10);

    wxBoxSizer *btnSizer = new wxBoxSizer(wxHORIZONTAL);
    btnSizer->Add(new wxButton(this, ID_NEW, "New Account"), 0, wxALL, 5);
    btnSizer->Add(new wxButton(this, ID_INT, "Apply Interest"), 0, wxALL, 5);
    btnSizer->Add(new wxButton(this, ID_STMT, "Mini-Statement"), 0, wxALL, 5);
    mainSizer->Add(btnSizer, 0, wxCENTER);

    SetSizer(mainSizer);

    Bind(wxEVT_BUTTON, &MainFrame::OnCreate, this, ID_NEW);
    Bind(wxEVT_BUTTON, &MainFrame::OnApplyInterest, this, ID_INT);
    Bind(wxEVT_BUTTON, &MainFrame::OnStatement, this, ID_STMT);
}

void MainFrame::RefreshList()
{
    listCtrl->DeleteAllItems();
    auto accs = bankPtr->getAllAccounts();
    for (size_t i = 0; i < accs.size(); ++i)
    {
        listCtrl->InsertItem(i, std::to_string(accs[i]->getAccNo()));
        listCtrl->SetItem(i, 1, accs[i]->getName());
        listCtrl->SetItem(i, 2, accs[i]->getTier());
        listCtrl->SetItem(i, 3, wxString::Format("%.2f", accs[i]->getBalance()));
    }
}

void MainFrame::OnCreate(wxCommandEvent &e)
{
    wxTextEntryDialog dlg(this, "Enter Customer Name:");
    if (dlg.ShowModal() == wxID_OK)
    {
        bankPtr->createAccount(std::string(dlg.GetValue().mb_str()), AccountType::SAVINGS);
        RefreshList();
    }
}

void MainFrame::OnApplyInterest(wxCommandEvent &e)
{
    bankPtr->applyMonthlyInterest();
    RefreshList();
}

void MainFrame::OnStatement(wxCommandEvent &e)
{
    long sel = listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (sel == -1)
        return;
    int id = std::stoi(std::string(listCtrl->GetItemText(sel, 0).mb_str()));
    auto acc = bankPtr->searchAccount(id);

    wxDialog dlg(this, wxID_ANY, "Statement", wxDefaultPosition, wxSize(300, 400));
    wxListBox *lb = new wxListBox(&dlg, wxID_ANY);
    for (auto &h : acc->getHistory())
        lb->Append(wxString::Format("%s: $%.2f", h.type, h.amount));

    wxBoxSizer *s = new wxBoxSizer(wxVERTICAL);
    s->Add(lb, 1, wxEXPAND | wxALL, 10);
    dlg.SetSizer(s);
    dlg.ShowModal();
}