#ifndef BANK_H
#define BANK_H

#include <string>
#include <vector>
#include <memory>
#include <ctime>

enum class AccountType
{
    SAVINGS,
    CURRENT
};

struct Transaction
{
    std::string type;
    double amount;
    std::time_t timestamp;
    std::string note;
};

class Account
{
private:
    int accNo;
    std::string name;
    double balance;
    bool active;
    AccountType type;
    std::vector<Transaction> history;

public:
    Account(int no, const std::string &n, AccountType t);
    int getAccNo() const { return accNo; }
    std::string getName() const { return name; }
    double getBalance() const { return balance; }
    bool isActive() const { return active; }
    AccountType getType() const { return type; }
    std::string getTier() const;

    bool deposit(double amt, std::string note = "Deposit");   // FIX #4: returns bool, validates amount
    bool withdraw(double amt, std::string note = "Withdrawal"); // FIX #6: checks active status
    void deactivate() { active = false; }
    const std::vector<Transaction> &getHistory() const { return history; }
};

// FIX #7: Replace raw BST with AVL tree to prevent degenerate case on sorted inserts
struct AVLNode
{
    std::shared_ptr<Account> acc;
    AVLNode *left, *right;
    int height;
    AVLNode(std::shared_ptr<Account> a) : acc(a), left(nullptr), right(nullptr), height(1) {}
};

struct AccountNode
{
    std::shared_ptr<Account> acc;
    AccountNode *next;
    AccountNode(std::shared_ptr<Account> a) : acc(a), next(nullptr) {}
};

class Bank
{
private:
    AccountNode *head;
    AVLNode *avlRoot;
    int nextAccNo;

    // AVL helpers
    int height(AVLNode *n) const;
    int balanceFactor(AVLNode *n) const;
    AVLNode *rotateRight(AVLNode *y);
    AVLNode *rotateLeft(AVLNode *x);
    AVLNode *insertAVL(AVLNode *root, std::shared_ptr<Account> acc);
    AVLNode *searchAVL(AVLNode *root, int accNo) const;
    void freeAVL(AVLNode *node);

public:
    Bank();
    ~Bank();
    int createAccount(const std::string &name, AccountType type);
    std::shared_ptr<Account> searchAccount(int accNo);
    bool depositTo(int acc, double amt);
    bool withdrawFrom(int acc, double amt);
    bool transferBetween(int from, int to, double amt);
    void applyMonthlyInterest();
    std::vector<std::shared_ptr<Account>> getAllAccounts();
};

#endif
