#include "bank.h"
#include <iostream>
#include <limits>
#include <string>

// FIX #3: Helper that reads a full line after flushing leftover whitespace
static std::string readLine(const std::string &prompt) {
    std::cout << prompt;
    std::string s;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(std::cin, s);
    return s;
}

// FIX #4: Helper that reads and validates a positive amount
static double readAmount(const std::string &prompt) {
    double amt = 0;
    while (true) {
        std::cout << prompt;
        if (std::cin >> amt && amt > 0) return amt;
        std::cout << "Amount must be a positive number. Try again.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

static int readAccountNo(const std::string &prompt) {
    int id = 0;
    std::cout << prompt;
    std::cin >> id;
    return id;
}

void menu()
{
    Bank bank;
    int choice;

    while (true)
    {
        std::cout << "\n--- VantageBank Console ---\n"
                     "1. Create Account\n"
                     "2. Deposit\n"
                     "3. Withdraw\n"
                     "4. Transfer\n"
                     "5. View All\n"
                     "0. Exit\n"
                     "Choice: ";
        std::cin >> choice;

        if (choice == 0) break;

        // FIX #2: Implemented all missing menu options (2, 3, 4)
        if (choice == 1)
        {
            std::string name = readLine("Name: ");
            if (name.empty()) { std::cout << "Name cannot be empty.\n"; continue; }
            int typeChoice;
            std::cout << "Account type — 1. Savings  2. Current: ";
            std::cin >> typeChoice;
            AccountType t = (typeChoice == 2) ? AccountType::CURRENT : AccountType::SAVINGS;
            int id = bank.createAccount(name, t);
            std::cout << "Account created: " << id << "\n";
        }
        else if (choice == 2)
        {
            int id  = readAccountNo("Account number: ");
            double amt = readAmount("Amount: $");
            if (bank.depositTo(id, amt))
                std::cout << "Deposited $" << amt << " to account " << id << "\n";
            else
                std::cout << "Deposit failed (invalid account or amount).\n";
        }
        else if (choice == 3)
        {
            int id  = readAccountNo("Account number: ");
            double amt = readAmount("Amount: $");
            if (bank.withdrawFrom(id, amt))
                std::cout << "Withdrew $" << amt << " from account " << id << "\n";
            else
                std::cout << "Withdrawal failed (insufficient funds, inactive account, or invalid input).\n";
        }
        else if (choice == 4)
        {
            int from = readAccountNo("From account: ");
            int to   = readAccountNo("To account:   ");
            double amt = readAmount("Amount: $");
            if (bank.transferBetween(from, to, amt))
                std::cout << "Transferred $" << amt << " from " << from << " to " << to << "\n";
            else
                std::cout << "Transfer failed (insufficient funds, inactive account, or invalid account).\n";
        }
        else if (choice == 5)
        {
            auto accounts = bank.getAllAccounts();
            if (accounts.empty()) { std::cout << "No accounts found.\n"; continue; }
            std::cout << "\n"
                      << std::string(55, '-') << "\n"
                      << "  ID    Name                 Balance      Tier\n"
                      << std::string(55, '-') << "\n";
            for (auto &a : accounts)
                std::cout << "  " << a->getAccNo()
                          << "  " << a->getName()
                          << "  $" << a->getBalance()
                          << "  [" << a->getTier() << "]\n";
        }
        else
        {
            std::cout << "Invalid choice.\n";
        }
    }
}

int main()
{
    menu();
    return 0;
}
