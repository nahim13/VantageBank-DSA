#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "../bank.h"

// ── Account Creation ────────────────────────────────────────────────────────

TEST_CASE("createAccount returns unique sequential account numbers", "[bank]") {
    Bank bank;
    int id1 = bank.createAccount("Alice", AccountType::SAVINGS);
    int id2 = bank.createAccount("Bob",   AccountType::CURRENT);
    REQUIRE(id1 == 1001);
    REQUIRE(id2 == 1002);
}

TEST_CASE("new account starts with zero balance and is active", "[account]") {
    Bank bank;
    int id = bank.createAccount("Alice", AccountType::SAVINGS);
    auto acc = bank.searchAccount(id);
    REQUIRE(acc != nullptr);
    REQUIRE(acc->getBalance() == Approx(0.0));
    REQUIRE(acc->isActive() == true);
}

TEST_CASE("searchAccount returns nullptr for unknown account", "[bank]") {
    Bank bank;
    REQUIRE(bank.searchAccount(9999) == nullptr);
}

// ── Deposit ──────────────────────────────────────────────────────────────────

TEST_CASE("deposit increases balance", "[account]") {
    Bank bank;
    int id = bank.createAccount("Alice", AccountType::SAVINGS);
    REQUIRE(bank.depositTo(id, 500.0) == true);
    REQUIRE(bank.searchAccount(id)->getBalance() == Approx(500.0));
}

TEST_CASE("deposit rejects zero amount", "[account]") {
    Bank bank;
    int id = bank.createAccount("Alice", AccountType::SAVINGS);
    REQUIRE(bank.depositTo(id, 0.0) == false);
    REQUIRE(bank.searchAccount(id)->getBalance() == Approx(0.0));
}

TEST_CASE("deposit rejects negative amount", "[account]") {
    Bank bank;
    int id = bank.createAccount("Alice", AccountType::SAVINGS);
    REQUIRE(bank.depositTo(id, -100.0) == false);
    REQUIRE(bank.searchAccount(id)->getBalance() == Approx(0.0));
}

TEST_CASE("deposit to unknown account returns false", "[bank]") {
    Bank bank;
    REQUIRE(bank.depositTo(9999, 100.0) == false);
}

// ── Withdraw ─────────────────────────────────────────────────────────────────

TEST_CASE("withdraw reduces balance", "[account]") {
    Bank bank;
    int id = bank.createAccount("Alice", AccountType::SAVINGS);
    bank.depositTo(id, 1000.0);
    REQUIRE(bank.withdrawFrom(id, 400.0) == true);
    REQUIRE(bank.searchAccount(id)->getBalance() == Approx(600.0));
}

TEST_CASE("withdraw fails when insufficient funds", "[account]") {
    Bank bank;
    int id = bank.createAccount("Alice", AccountType::SAVINGS);
    bank.depositTo(id, 100.0);
    REQUIRE(bank.withdrawFrom(id, 200.0) == false);
    REQUIRE(bank.searchAccount(id)->getBalance() == Approx(100.0));
}

TEST_CASE("withdraw rejects zero amount", "[account]") {
    Bank bank;
    int id = bank.createAccount("Alice", AccountType::SAVINGS);
    bank.depositTo(id, 500.0);
    REQUIRE(bank.withdrawFrom(id, 0.0) == false);
}

TEST_CASE("withdraw rejects negative amount", "[account]") {
    Bank bank;
    int id = bank.createAccount("Alice", AccountType::SAVINGS);
    bank.depositTo(id, 500.0);
    REQUIRE(bank.withdrawFrom(id, -50.0) == false);
    REQUIRE(bank.searchAccount(id)->getBalance() == Approx(500.0));
}

TEST_CASE("withdraw fails on inactive account", "[account]") {
    Bank bank;
    int id = bank.createAccount("Alice", AccountType::SAVINGS);
    bank.depositTo(id, 500.0);
    bank.searchAccount(id)->deactivate();
    REQUIRE(bank.withdrawFrom(id, 100.0) == false);
    REQUIRE(bank.searchAccount(id)->getBalance() == Approx(500.0));
}

TEST_CASE("withdraw from unknown account returns false", "[bank]") {
    Bank bank;
    REQUIRE(bank.withdrawFrom(9999, 100.0) == false);
}

// ── Transfer ─────────────────────────────────────────────────────────────────

TEST_CASE("transfer moves funds between accounts", "[bank]") {
    Bank bank;
    int a = bank.createAccount("Alice", AccountType::SAVINGS);
    int b = bank.createAccount("Bob",   AccountType::SAVINGS);
    bank.depositTo(a, 1000.0);
    REQUIRE(bank.transferBetween(a, b, 300.0) == true);
    REQUIRE(bank.searchAccount(a)->getBalance() == Approx(700.0));
    REQUIRE(bank.searchAccount(b)->getBalance() == Approx(300.0));
}

TEST_CASE("transfer fails when source has insufficient funds", "[bank]") {
    Bank bank;
    int a = bank.createAccount("Alice", AccountType::SAVINGS);
    int b = bank.createAccount("Bob",   AccountType::SAVINGS);
    bank.depositTo(a, 100.0);
    REQUIRE(bank.transferBetween(a, b, 500.0) == false);
    REQUIRE(bank.searchAccount(a)->getBalance() == Approx(100.0));
    REQUIRE(bank.searchAccount(b)->getBalance() == Approx(0.0));
}

TEST_CASE("transfer rejects zero and negative amounts", "[bank]") {
    Bank bank;
    int a = bank.createAccount("Alice", AccountType::SAVINGS);
    int b = bank.createAccount("Bob",   AccountType::SAVINGS);
    bank.depositTo(a, 500.0);
    REQUIRE(bank.transferBetween(a, b,   0.0) == false);
    REQUIRE(bank.transferBetween(a, b, -50.0) == false);
}

TEST_CASE("transfer fails when source account is inactive", "[bank]") {
    Bank bank;
    int a = bank.createAccount("Alice", AccountType::SAVINGS);
    int b = bank.createAccount("Bob",   AccountType::SAVINGS);
    bank.depositTo(a, 500.0);
    bank.searchAccount(a)->deactivate();
    REQUIRE(bank.transferBetween(a, b, 100.0) == false);
    REQUIRE(bank.searchAccount(b)->getBalance() == Approx(0.0));
}

TEST_CASE("transfer fails with unknown source or destination", "[bank]") {
    Bank bank;
    int a = bank.createAccount("Alice", AccountType::SAVINGS);
    bank.depositTo(a, 500.0);
    REQUIRE(bank.transferBetween(a,    9999, 100.0) == false);
    REQUIRE(bank.transferBetween(9999, a,    100.0) == false);
}

// ── Tier ─────────────────────────────────────────────────────────────────────

TEST_CASE("tier reflects correct balance thresholds", "[account]") {
    Bank bank;
    int id = bank.createAccount("Alice", AccountType::SAVINGS);
    auto acc = bank.searchAccount(id);

    bank.depositTo(id, 500.0);
    REQUIRE(acc->getTier() == "Bronze");

    bank.depositTo(id, 500.0);   // 1000
    REQUIRE(acc->getTier() == "Silver");

    bank.depositTo(id, 4000.0);  // 5000
    REQUIRE(acc->getTier() == "Gold");

    bank.depositTo(id, 5000.0);  // 10000
    REQUIRE(acc->getTier() == "Platinum");
}

TEST_CASE("tier boundary: exactly 1000 is Silver not Bronze", "[account]") {
    Bank bank;
    int id = bank.createAccount("Alice", AccountType::SAVINGS);
    bank.depositTo(id, 1000.0);
    REQUIRE(bank.searchAccount(id)->getTier() == "Silver");
}

TEST_CASE("tier boundary: exactly 5000 is Gold not Silver", "[account]") {
    Bank bank;
    int id = bank.createAccount("Alice", AccountType::SAVINGS);
    bank.depositTo(id, 5000.0);
    REQUIRE(bank.searchAccount(id)->getTier() == "Gold");
}

TEST_CASE("tier boundary: exactly 10000 is Platinum not Gold", "[account]") {
    Bank bank;
    int id = bank.createAccount("Alice", AccountType::SAVINGS);
    bank.depositTo(id, 10000.0);
    REQUIRE(bank.searchAccount(id)->getTier() == "Platinum");
}

// ── Monthly Interest ──────────────────────────────────────────────────────────

TEST_CASE("applyMonthlyInterest credits 3% to savings accounts", "[bank]") {
    Bank bank;
    int id = bank.createAccount("Alice", AccountType::SAVINGS);
    bank.depositTo(id, 1000.0);
    bank.applyMonthlyInterest();
    REQUIRE(bank.searchAccount(id)->getBalance() == Approx(1030.0));
}

TEST_CASE("applyMonthlyInterest does not credit current accounts", "[bank]") {
    Bank bank;
    int id = bank.createAccount("Alice", AccountType::CURRENT);
    bank.depositTo(id, 1000.0);
    bank.applyMonthlyInterest();
    REQUIRE(bank.searchAccount(id)->getBalance() == Approx(1000.0));
}

TEST_CASE("applyMonthlyInterest skips inactive accounts", "[bank]") {
    Bank bank;
    int id = bank.createAccount("Alice", AccountType::SAVINGS);
    bank.depositTo(id, 1000.0);
    bank.searchAccount(id)->deactivate();
    bank.applyMonthlyInterest();
    REQUIRE(bank.searchAccount(id)->getBalance() == Approx(1000.0));
}

TEST_CASE("applyMonthlyInterest does not compound within the same pass", "[bank]") {
    Bank bank;
    int a = bank.createAccount("Alice", AccountType::SAVINGS);
    int b = bank.createAccount("Bob",   AccountType::SAVINGS);
    bank.depositTo(a, 1000.0);
    bank.depositTo(b, 2000.0);
    bank.applyMonthlyInterest();
    REQUIRE(bank.searchAccount(a)->getBalance() == Approx(1030.0));
    REQUIRE(bank.searchAccount(b)->getBalance() == Approx(2060.0));
}

// ── Transaction History ───────────────────────────────────────────────────────

TEST_CASE("deposit records a CREDIT transaction", "[account]") {
    Bank bank;
    int id = bank.createAccount("Alice", AccountType::SAVINGS);
    bank.depositTo(id, 250.0);
    const auto &h = bank.searchAccount(id)->getHistory();
    REQUIRE(h.size() == 1);
    REQUIRE(h[0].type   == "CREDIT");
    REQUIRE(h[0].amount == Approx(250.0));
}

TEST_CASE("withdraw records a DEBIT transaction", "[account]") {
    Bank bank;
    int id = bank.createAccount("Alice", AccountType::SAVINGS);
    bank.depositTo(id, 500.0);
    bank.withdrawFrom(id, 200.0);
    const auto &h = bank.searchAccount(id)->getHistory();
    REQUIRE(h.size() == 2);
    REQUIRE(h[1].type   == "DEBIT");
    REQUIRE(h[1].amount == Approx(200.0));
}

TEST_CASE("failed operations do not add history entries", "[account]") {
    Bank bank;
    int id = bank.createAccount("Alice", AccountType::SAVINGS);
    bank.depositTo(id,   -100.0);  // invalid amount
    bank.withdrawFrom(id, 999.0);  // insufficient funds
    REQUIRE(bank.searchAccount(id)->getHistory().empty());
}

TEST_CASE("transfer records correct notes on both accounts", "[bank]") {
    Bank bank;
    int a = bank.createAccount("Alice", AccountType::SAVINGS);
    int b = bank.createAccount("Bob",   AccountType::SAVINGS);
    bank.depositTo(a, 1000.0);
    bank.transferBetween(a, b, 300.0);

    const auto &ha = bank.searchAccount(a)->getHistory();
    const auto &hb = bank.searchAccount(b)->getHistory();

    REQUIRE(ha.back().type == "DEBIT");
    REQUIRE(hb.back().type == "CREDIT");
}

// ── getAllAccounts ────────────────────────────────────────────────────────────

TEST_CASE("getAllAccounts returns all created accounts", "[bank]") {
    Bank bank;
    bank.createAccount("Alice", AccountType::SAVINGS);
    bank.createAccount("Bob",   AccountType::CURRENT);
    bank.createAccount("Carol", AccountType::SAVINGS);
    REQUIRE(bank.getAllAccounts().size() == 3);
}

TEST_CASE("getAllAccounts returns empty vector for a fresh bank", "[bank]") {
    Bank bank;
    REQUIRE(bank.getAllAccounts().empty());
}

// ── AVL search correctness (many accounts) ───────────────────────────────────

TEST_CASE("searchAccount works correctly after many sequential inserts", "[bank][avl]") {
    Bank bank;
    // Insert enough accounts to cause a degenerate BST but a balanced AVL
    std::vector<int> ids;
    for (int i = 0; i < 50; ++i)
        ids.push_back(bank.createAccount("User" + std::to_string(i), AccountType::SAVINGS));

    for (int id : ids)
        REQUIRE(bank.searchAccount(id) != nullptr);

    REQUIRE(bank.searchAccount(9999) == nullptr);
}
