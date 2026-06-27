#include "bank.h"
#include <iostream>

void menu()
{
    Bank bank;
    int choice;
    while (true)
    {
        std::cout << "\n--- VantageBank Console ---\n1. Create Account\n2. Deposit\n3. Withdraw\n4. Transfer\n5. View All\n0. Exit\nChoice: ";
        std::cin >> choice;
        if (choice == 0)
            break;
        if (choice == 1)
        {
            std::string name;
            std::cout << "Name: ";
            std::cin >> name;
            int id = bank.createAccount(name, AccountType::SAVINGS);
            std::cout << "Account Created: " << id << std::endl;
        }
        else if (choice == 5)
        {
            for (auto &a : bank.getAllAccounts())
                std::cout << a->getAccNo() << " | " << a->getName() << " | $" << a->getBalance() << " [" << a->getTier() << "]\n";
        }
    }
}

int main()
{
    menu();
    return 0;
}