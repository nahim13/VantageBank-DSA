#pragma once
#include <wx/wx.h>
#include <memory>
#include "bank.h"

class MainApp : public wxApp {
public:
    virtual bool OnInit();
    virtual int  OnExit();

private:
    // FIX #1: Bank is owned here as unique_ptr — no raw new, no leak
    std::unique_ptr<Bank> bank;
};
