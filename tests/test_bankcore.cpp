#include <iostream>
#include <string>
#include <cassert>
#include <vector>

#include "../core/Account.hpp"
#include "../core/BankSystem.hpp"
#include "../core/Trie.hpp"
#include "../core/LRUCache.hpp"
#include "../core/FraudDetector.hpp"
#include "../core/Graph.hpp"

using namespace std;

// Lightweight Test Harness Macros
int g_passed = 0;
int g_failed = 0;

#define TEST_ASSERT(condition, msg) \
    do { \
        if (condition) { \
            cout << "  [PASS] " << msg << endl; \
            g_passed++; \
        } else { \
            cout << "  [FAIL] " << msg << " (" << __FILE__ << ":" << __LINE__ << ")" << endl; \
            g_failed++; \
        } \
    } while(0)

void test_account_operations() {
    cout << "\n--- Testing Account Operations ---" << endl;
    Account acc(1, "Alice", "1234", 500.0);
    
    TEST_ASSERT(acc.balance == 500.0, "Initial balance is 500.0");
    
    acc.deposit(200.0);
    TEST_ASSERT(acc.balance == 700.0, "Deposit 200.0 increases balance to 700.0");
    
    bool withdrawSuccess = acc.withdraw(150.0);
    TEST_ASSERT(withdrawSuccess == true, "Withdraw 150.0 succeeds");
    TEST_ASSERT(acc.balance == 550.0, "Balance is 550.0 after withdrawal");
    
    bool overdrawSuccess = acc.withdraw(1000.0);
    TEST_ASSERT(overdrawSuccess == false, "Overdraw 1000.0 is rejected");
    TEST_ASSERT(acc.balance == 550.0, "Balance remains unchanged after rejected withdrawal");
}

void test_bank_system_transfers() {
    cout << "\n--- Testing BankSystem Operations ---" << endl;
    BankSystem bank;
    
    string res1 = bank.createAccount("Bob", "1111");
    string res2 = bank.createAccount("Charlie", "2222");
    
    TEST_ASSERT(res1.find("Account created successfully") != string::npos, "Account creation for Bob");
    TEST_ASSERT(res2.find("Account created successfully") != string::npos, "Account creation for Charlie");
    
    Account* bob = bank.login(1001, "1111");
    Account* charlie = bank.login(1002, "2222");
    
    TEST_ASSERT(bob != nullptr, "Bob login with valid PIN succeeds");
    TEST_ASSERT(bank.login(1001, "9999") == nullptr, "Bob login with invalid PIN fails");
    
    if (bob && charlie) {
        bob->deposit(300.0);
        
        string xferRes = bank.transferMoney(1001, 1002, 100.0);
        TEST_ASSERT(xferRes.find("Transfer successful") != string::npos, "Transfer $100 from Bob to Charlie succeeds");
        TEST_ASSERT(bob->balance == 200.0, "Bob balance reduced to $200");
        TEST_ASSERT(charlie->balance == 100.0, "Charlie balance increased to $100");
        
        string failXfer = bank.transferMoney(1001, 1002, 5000.0);
        TEST_ASSERT(failXfer.find("Insufficient balance") != string::npos, "Overdraw transfer rejected");
        
        string negXfer = bank.transferMoney(1001, 1002, -50.0);
        TEST_ASSERT(negXfer.find("Invalid transfer amount") != string::npos, "Negative amount transfer rejected");
    }
}

void test_trie_and_lru_cache() {
    cout << "\n--- Testing Data Structures (Trie & LRUCache) ---" << endl;
    
    Trie trie;
    trie.insert("Naman", 1001);
    trie.insert("Nikhil", 1002);
    trie.insert("Navin", 1003);
    
    vector<pair<string, int>> results = trie.searchPrefix("Na");
    TEST_ASSERT(results.size() == 3, "Trie prefix search for 'Na' returns 3 matching users");
    
    LRUCache<int, string> cache(2);
    cache.put(1001, "Bob");
    cache.put(1002, "Charlie");
    cache.put(1003, "David"); // Should evict 1001 (Bob)
    
    string display = cache.getDisplayString("Alice");
    TEST_ASSERT(display.find("David") != string::npos, "LRUCache contains latest put item 'David'");
    TEST_ASSERT(display.find("Bob") == string::npos, "LRUCache evicted oldest item 'Bob'");
}

void test_fraud_detection() {
    cout << "\n--- Testing Fraud Detector (Graph Cycle Detection) ---" << endl;
    Graph g;
    g.addEdge(1, 2);
    g.addEdge(2, 3);
    g.addEdge(3, 1); // Cycle 1 -> 2 -> 3 -> 1
    
    FraudDetector detector;
    TEST_ASSERT(detector.hasCycle(1, g) == true, "FraudDetector correctly flags circular transfer ring (Cycle)");
    
    Graph linearG;
    linearG.addEdge(1, 2);
    linearG.addEdge(2, 3);
    TEST_ASSERT(detector.hasCycle(1, linearG) == false, "FraudDetector confirms no cycle in linear transfers");
}

int main() {
    cout << "=======================================" << endl;
    cout << "  BankCore C++ Unit Test Suite (SDET)  " << endl;
    cout << "=======================================" << endl;
    
    test_account_operations();
    test_bank_system_transfers();
    test_trie_and_lru_cache();
    test_fraud_detection();
    
    cout << "\n=======================================" << endl;
    cout << "  Test Summary: " << g_passed << " Passed, " << g_failed << " Failed" << endl;
    cout << "=======================================" << endl;
    
    return (g_failed == 0) ? 0 : 1;
}
