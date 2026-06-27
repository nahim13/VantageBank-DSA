#pragma once
#include <wx/wx.h>
#include "bank.h"

class MainApp : public wxApp {
public:
    virtual bool OnInit();
private:
    std::unique_ptr<Bank> bank;
};