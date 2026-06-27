#include "bank.h"
#include <algorithm>
#include <stdexcept>

// ── Account ────────────────────────────────────────────────────────────────

Account::Account(int no, const std::string &n, AccountType t)
    : accNo(no), name(n), balance(0.0), active(true), type(t) {}

// FIX #9 (tier ladder): Added Silver tier between Bronze and Gold
std::string Account::getTier() const {
    if (balance >= 10000) return "Platinum";
    if (balance >= 5000)  return "Gold";
    if (balance >= 1000)  return "Silver";
    return "Bronze";
}

// FIX #4: Validate amount > 0 before crediting
bool Account::deposit(double amt, std::string note) {
    if (amt <= 0) return false;
    balance += amt;
    history.push_back({"CREDIT", amt, std::time(nullptr), note});
    return true;
}

// FIX #4 + #6: Validate amount > 0 AND check active status before allowing withdrawal
bool Account::withdraw(double amt, std::string note) {
    if (amt <= 0)       return false;
    if (!active)        return false;
    if (balance < amt)  return false;
    balance -= amt;
    history.push_back({"DEBIT", amt, std::time(nullptr), note});
    return true;
}

// ── AVL Tree ───────────────────────────────────────────────────────────────
// FIX #7: Full AVL implementation so search stays O(log n) even with
//         sequential account numbers (which previously produced a right-skewed BST).

int Bank::height(AVLNode *n) const {
    return n ? n->height : 0;
}

int Bank::balanceFactor(AVLNode *n) const {
    return n ? height(n->left) - height(n->right) : 0;
}

AVLNode *Bank::rotateRight(AVLNode *y) {
    AVLNode *x  = y->left;
    AVLNode *T2 = x->right;
    x->right = y;
    y->left  = T2;
    y->height = 1 + std::max(height(y->left), height(y->right));
    x->height = 1 + std::max(height(x->left), height(x->right));
    return x;
}

AVLNode *Bank::rotateLeft(AVLNode *x) {
    AVLNode *y  = x->right;
    AVLNode *T2 = y->left;
    y->left  = x;
    x->right = T2;
    x->height = 1 + std::max(height(x->left), height(x->right));
    y->height = 1 + std::max(height(y->left), height(y->right));
    return y;
}

AVLNode *Bank::insertAVL(AVLNode *root, std::shared_ptr<Account> acc) {
    if (!root) return new AVLNode(acc);

    if (acc->getAccNo() < root->acc->getAccNo())
        root->left  = insertAVL(root->left,  acc);
    else
        root->right = insertAVL(root->right, acc);

    root->height = 1 + std::max(height(root->left), height(root->right));

    int bf = balanceFactor(root);

    // Left-Left
    if (bf > 1 && acc->getAccNo() < root->left->acc->getAccNo())
        return rotateRight(root);
    // Right-Right
    if (bf < -1 && acc->getAccNo() > root->right->acc->getAccNo())
        return rotateLeft(root);
    // Left-Right
    if (bf > 1 && acc->getAccNo() > root->left->acc->getAccNo()) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }
    // Right-Left
    if (bf < -1 && acc->getAccNo() < root->right->acc->getAccNo()) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }
    return root;
}

AVLNode *Bank::searchAVL(AVLNode *root, int accNo) const {
    if (!root || root->acc->getAccNo() == accNo) return root;
    return (accNo < root->acc->getAccNo())
        ? searchAVL(root->left,  accNo)
        : searchAVL(root->right, accNo);
}

void Bank::freeAVL(AVLNode *node) {
    if (node) { freeAVL(node->left); freeAVL(node->right); delete node; }
}

// ── Bank ───────────────────────────────────────────────────────────────────

Bank::Bank() : head(nullptr), avlRoot(nullptr), nextAccNo(1001) {}

Bank::~Bank() {
    freeAVL(avlRoot);
    while (head) { auto t = head; head = head->next; delete t; }
}

int Bank::createAccount(const std::string &name, AccountType type) {
    auto newAcc  = std::make_shared<Account>(nextAccNo++, name, type);
    auto newNode = new AccountNode(newAcc);
    newNode->next = head;
    head = newNode;
    avlRoot = insertAVL(avlRoot, newAcc);
    return newAcc->getAccNo();
}

std::shared_ptr<Account> Bank::searchAccount(int accNo) {
    auto node = searchAVL(avlRoot, accNo);
    return node ? node->acc : nullptr;
}

bool Bank::depositTo(int acc, double amt) {
    auto a = searchAccount(acc);
    if (!a) return false;
    return a->deposit(amt);   // FIX #4: propagates validation result
}

bool Bank::withdrawFrom(int acc, double amt) {
    auto a = searchAccount(acc);
    if (!a) return false;
    return a->withdraw(amt);  // FIX #4 + #6: propagates validation + active check
}

bool Bank::transferBetween(int from, int to, double amt) {
    if (amt <= 0) return false;  // FIX #4: validate amount early
    auto a = searchAccount(from);
    auto b = searchAccount(to);
    if (!a || !b) return false;
    if (!a->withdraw(amt, "Transfer to " + std::to_string(to))) return false;
    b->deposit(amt, "Transfer from " + std::to_string(from));
    return true;
}

// FIX #8: Snapshot balances first, then apply interest, so no account
//         benefits from another account's interest in the same pass.
void Bank::applyMonthlyInterest() {
    // Collect (account, interest_amount) pairs before touching any balance
    std::vector<std::pair<std::shared_ptr<Account>, double>> credits;
    auto curr = head;
    while (curr) {
        if (curr->acc->isActive() && curr->acc->getType() == AccountType::SAVINGS)
            credits.push_back({curr->acc, curr->acc->getBalance() * 0.03});
        curr = curr->next;
    }
    for (auto &p : credits)
        p.first->deposit(p.second, "Interest Credit");
}

std::vector<std::shared_ptr<Account>> Bank::getAllAccounts() {
    std::vector<std::shared_ptr<Account>> v;
    auto curr = head;
    while (curr) { v.push_back(curr->acc); curr = curr->next; }
    return v;
}
