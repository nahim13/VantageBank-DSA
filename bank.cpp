#include "bank.h"
#include <algorithm>

Account::Account(int no, const std::string& n, AccountType t) 
    : accNo(no), name(n), balance(0.0), active(true), type(t) {}

std::string Account::getTier() const {
    if (balance >= 10000) return "Platinum";
    if (balance >= 5000) return "Gold";
    return "Bronze";
}

void Account::deposit(double amt, std::string note) {
    balance += amt;
    history.push_back({"CREDIT", amt, std::time(nullptr), note});
}

bool Account::withdraw(double amt, std::string note) {
    if (balance >= amt) {
        balance -= amt;
        history.push_back({"DEBIT", amt, std::time(nullptr), note});
        return true;
    }
    return false;
}

Bank::Bank() : head(nullptr), bstRoot(nullptr), nextAccNo(1001) {}
Bank::~Bank() { freeBST(bstRoot); while(head) { auto t = head; head = head->next; delete t; } }

int Bank::createAccount(const std::string& name, AccountType type) {
    auto newAcc = std::make_shared<Account>(nextAccNo++, name, type);
    auto newNode = new AccountNode(newAcc);
    newNode->next = head; head = newNode;
    bstRoot = insertBST(bstRoot, newAcc);
    return newAcc->getAccNo();
}

bool Bank::depositTo(int acc, double amt) {
    auto a = searchAccount(acc);
    if (a) { a->deposit(amt); return true; }
    return false;
}

bool Bank::withdrawFrom(int acc, double amt) {
    auto a = searchAccount(acc);
    if (a) return a->withdraw(amt);
    return false;
}

bool Bank::transferBetween(int from, int to, double amt) {
    auto a = searchAccount(from);
    auto b = searchAccount(to);
    if (a && b && a->withdraw(amt, "Transfer to " + std::to_string(to))) {
        b->deposit(amt, "Transfer from " + std::to_string(from));
        return true;
    }
    return false;
}

void Bank::applyMonthlyInterest() {
    auto curr = head;
    while(curr) {
        if(curr->acc->isActive() && curr->acc->getType() == AccountType::SAVINGS) {
            curr->acc->deposit(curr->acc->getBalance() * 0.03, "Interest Credit");
        }
        curr = curr->next;
    }
}

std::shared_ptr<Account> Bank::searchAccount(int accNo) {
    auto node = searchBST(bstRoot, accNo);
    return node ? node->acc : nullptr;
}

BSTNode* Bank::insertBST(BSTNode* root, std::shared_ptr<Account> acc) {
    if (!root) return new BSTNode(acc);
    if (acc->getAccNo() < root->acc->getAccNo()) root->left = insertBST(root->left, acc);
    else root->right = insertBST(root->right, acc);
    return root;
}

BSTNode* Bank::searchBST(BSTNode* root, int accNo) const {
    if (!root || root->acc->getAccNo() == accNo) return root;
    return (accNo < root->acc->getAccNo()) ? searchBST(root->left, accNo) : searchBST(root->right, accNo);
}

void Bank::freeBST(BSTNode* node) { if(node) { freeBST(node->left); freeBST(node->right); delete node; } }

std::vector<std::shared_ptr<Account>> Bank::getAllAccounts() {
    std::vector<std::shared_ptr<Account>> v;
    auto curr = head;
    while(curr) { v.push_back(curr->acc); curr = curr->next; }
    return v;
}