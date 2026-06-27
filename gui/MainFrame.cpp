#include "MainFrame.h"
#include <wx/numdlg.h>

enum
{
    ID_INT = 200,
    ID_STMT,
    ID_NEW,
    ID_DEPOSIT,
    ID_WITHDRAW,
    ID_TRANSFER
};

MainFrame::MainFrame(const wxString &title, Bank *bank)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800, 500)), bankPtr(bank)
{
    wxBoxSizer *mainSizer = new wxBoxSizer(wxVERTICAL);
    listCtrl = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
    listCtrl->InsertColumn(0, "ID",      wxLIST_FORMAT_LEFT,  60);
    listCtrl->InsertColumn(1, "Name",    wxLIST_FORMAT_LEFT,  150);
    listCtrl->InsertColumn(2, "Tier",    wxLIST_FORMAT_LEFT,  80);
    listCtrl->InsertColumn(3, "Balance", wxLIST_FORMAT_RIGHT, 100);

    mainSizer->Add(listCtrl, 1, wxEXPAND | wxALL, 10);

    wxBoxSizer *btnSizer = new wxBoxSizer(wxHORIZONTAL);
    btnSizer->Add(new wxButton(this, ID_NEW,      "New Account"),    0, wxALL, 5);
    btnSizer->Add(new wxButton(this, ID_DEPOSIT,  "Deposit"),        0, wxALL, 5);
    btnSizer->Add(new wxButton(this, ID_WITHDRAW, "Withdraw"),       0, wxALL, 5);
    btnSizer->Add(new wxButton(this, ID_TRANSFER, "Transfer"),       0, wxALL, 5);
    btnSizer->Add(new wxButton(this, ID_INT,      "Apply Interest"), 0, wxALL, 5);
    btnSizer->Add(new wxButton(this, ID_STMT,     "Mini-Statement"), 0, wxALL, 5);
    mainSizer->Add(btnSizer, 0, wxCENTER);

    SetSizer(mainSizer);

    Bind(wxEVT_BUTTON, &MainFrame::OnCreate,       this, ID_NEW);
    Bind(wxEVT_BUTTON, &MainFrame::OnDeposit,      this, ID_DEPOSIT);
    Bind(wxEVT_BUTTON, &MainFrame::OnWithdraw,     this, ID_WITHDRAW);
    Bind(wxEVT_BUTTON, &MainFrame::OnTransfer,     this, ID_TRANSFER);
    Bind(wxEVT_BUTTON, &MainFrame::OnApplyInterest,this, ID_INT);
    Bind(wxEVT_BUTTON, &MainFrame::OnStatement,    this, ID_STMT);
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

void MainFrame::OnCreate(wxCommandEvent &)
{
    wxTextEntryDialog dlg(this, "Enter Customer Name:");
    if (dlg.ShowModal() != wxID_OK) return;
    wxString name = dlg.GetValue().Trim();
    if (name.IsEmpty()) { wxMessageBox("Name cannot be empty.", "Error"); return; }

    wxString choices[] = { "Savings", "Current" };
    wxSingleChoiceDialog typeDlg(this, "Account Type:", "Select Type", 2, choices);
    if (typeDlg.ShowModal() != wxID_OK) return;
    AccountType t = (typeDlg.GetSelection() == 1) ? AccountType::CURRENT : AccountType::SAVINGS;

    int id = bankPtr->createAccount(std::string(name.mb_str()), t);
    wxMessageBox(wxString::Format("Account created: %d", id), "Success");
    RefreshList();
}

// FIX #2 (GUI): Deposit button now wired up and functional
void MainFrame::OnDeposit(wxCommandEvent &)
{
    long sel = listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (sel == -1) { wxMessageBox("Select an account first.", "Error"); return; }
    int id = std::stoi(std::string(listCtrl->GetItemText(sel, 0).mb_str()));

    double amt = wxGetNumberFromUser("Amount to deposit:", "$", "Deposit", 0, 1, 1000000, this);
    if (amt <= 0) return;
    if (bankPtr->depositTo(id, amt))
        RefreshList();
    else
        wxMessageBox("Deposit failed.", "Error");
}

// FIX #2 (GUI): Withdraw button now wired up and functional
void MainFrame::OnWithdraw(wxCommandEvent &)
{
    long sel = listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (sel == -1) { wxMessageBox("Select an account first.", "Error"); return; }
    int id = std::stoi(std::string(listCtrl->GetItemText(sel, 0).mb_str()));

    double amt = wxGetNumberFromUser("Amount to withdraw:", "$", "Withdraw", 0, 1, 1000000, this);
    if (amt <= 0) return;
    if (bankPtr->withdrawFrom(id, amt))
        RefreshList();
    else
        wxMessageBox("Withdrawal failed (insufficient funds or inactive account).", "Error");
}

// FIX #2 (GUI): Transfer button now wired up and functional
void MainFrame::OnTransfer(wxCommandEvent &)
{
    long sel = listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (sel == -1) { wxMessageBox("Select a source account first.", "Error"); return; }
    int fromId = std::stoi(std::string(listCtrl->GetItemText(sel, 0).mb_str()));

    long toId = wxGetNumberFromUser("Destination account number:", "Account:", "Transfer", 0, 1001, 999999, this);
    if (toId <= 0) return;
    double amt = wxGetNumberFromUser("Amount to transfer:", "$", "Transfer", 0, 1, 1000000, this);
    if (amt <= 0) return;

    if (bankPtr->transferBetween(fromId, (int)toId, amt))
        RefreshList();
    else
        wxMessageBox("Transfer failed (insufficient funds, inactive account, or invalid destination).", "Error");
}

void MainFrame::OnApplyInterest(wxCommandEvent &)
{
    bankPtr->applyMonthlyInterest();
    RefreshList();
}

void MainFrame::OnStatement(wxCommandEvent &)
{
    long sel = listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (sel == -1) { wxMessageBox("Select an account first.", "Error"); return; }
    int id = std::stoi(std::string(listCtrl->GetItemText(sel, 0).mb_str()));

    // FIX #5: Guard against null before dereferencing
    auto acc = bankPtr->searchAccount(id);
    if (!acc) { wxMessageBox("Account not found.", "Error"); return; }

    wxDialog dlg(this, wxID_ANY, "Statement", wxDefaultPosition, wxSize(350, 400));
    wxListBox *lb = new wxListBox(&dlg, wxID_ANY);

    const auto &history = acc->getHistory();
    if (history.empty())
        lb->Append("No transactions yet.");
    else
        for (auto &h : history)
            lb->Append(wxString::Format("%s  $%.2f  — %s", h.type, h.amount, h.note));

    wxBoxSizer *s = new wxBoxSizer(wxVERTICAL);
    s->Add(lb, 1, wxEXPAND | wxALL, 10);
    dlg.SetSizer(s);
    dlg.ShowModal();
}
