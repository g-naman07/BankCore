#ifndef BANK_SYSTEM_HPP
#define BANK_SYSTEM_HPP

#include "Account.hpp"
#include "Graph.hpp"
#include "FraudDetector.hpp"
#include "Trie.hpp"
#include "CreditSystem.hpp"
#include "LRUCache.hpp"
#include "DisjointSet.hpp"
#include <string>
#include <unordered_map>
#include <sstream>

using namespace std;

class BankSystem {
public:
    BankSystem();
    
    string createAccount(const string& name, const string& pin);
    string transferMoney(int fromId, int toId, double amount);
    string suggestConnections(int accountId);
    string searchUser(const string& query);
    string getRecentTransactions(int accountId);
    string getAdminDashboard();
    string getTransactionHistory(int accountId);
    string getBalance(int accountId);

    Account* login(int accountId, const string& pin);
    
private:
    string checkFraud(int startNodeId);
    void seedInitialAccounts();

    unordered_map<int, Account> accounts;
    int nextAccountId;
    Graph transactionGraph;
    FraudDetector fraudDetector;
    Trie userTrie;
    CreditSystem creditSystem;
    unordered_map<int, LRUCache<int, string>> recentTransactionsCache;
};

#endif