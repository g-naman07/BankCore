#include "BankSystem.hpp"
#include <iostream>
#include <queue>
#include <set>
#include <iomanip>

using namespace std;

BankSystem::BankSystem() : nextAccountId(1001) {
    seedInitialAccounts();
}

void BankSystem::seedInitialAccounts() {
    int id1 = nextAccountId++;
    accounts[id1] = Account(id1, "Alice", "1234", 1000.0);
    userTrie.insert("Alice", id1);

    int id2 = nextAccountId++;
    accounts[id2] = Account(id2, "Bob", "5678", 1000.0);
    userTrie.insert("Bob", id2);

    int id3 = nextAccountId++;
    accounts[id3] = Account(id3, "Charlie", "1111", 1000.0);
    userTrie.insert("Charlie", id3);

    int id4 = nextAccountId++;
    accounts[id4] = Account(id4, "David", "2222", 1000.0);
    userTrie.insert("David", id4);

    int id5 = nextAccountId++;
    accounts[id5] = Account(id5, "Eve", "3333", 1000.0);
    userTrie.insert("Eve", id5);
}

string BankSystem::createAccount(const string& name, const string& pin) {
    int id = nextAccountId++;
    accounts[id] = Account(id, name, pin, 1000.0);
    userTrie.insert(name, id);
    return "Account created successfully for " + name + ". Your Account ID is: " + to_string(id);
}

Account* BankSystem::login(int accountId, const string& pin) {
    if (accounts.count(accountId) && accounts[accountId].pin == pin) {
        return &accounts[accountId];
    }
    return nullptr;
}

string BankSystem::transferMoney(int fromId, int toId, double amount) {
    if (accounts.find(fromId) == accounts.end() || accounts.find(toId) == accounts.end()) {
        return "Error: Invalid sender or receiver account ID.";
    }
    if (amount <= 0) {
        return "Error: Transfer amount must be positive.";
    }
    if (accounts.at(fromId).balance < amount) {
        return "Error: Insufficient funds.";
    }

    accounts.at(fromId).withdraw(amount);
    accounts.at(fromId).addTransaction("Sent", amount, toId);
    accounts.at(toId).deposit(amount);
    accounts.at(toId).addTransaction("Received", amount, fromId);

    transactionGraph.addEdge(fromId, toId, amount);
    creditSystem.updateScore(accounts.at(fromId), amount, true);
    creditSystem.updateScore(accounts.at(toId), amount, false);

    if (recentTransactionsCache.find(fromId) == recentTransactionsCache.end()) {
        recentTransactionsCache.emplace(fromId, LRUCache<int, string>(5));
    }
    recentTransactionsCache.at(fromId).put(toId, accounts.at(toId).ownerName);

    string response = "Transfer successful from " + to_string(fromId) + " to " + to_string(toId) + ".";
    response += checkFraud(fromId);
    return response;
}

string BankSystem::checkFraud(int startNodeId) {
    if (fraudDetector.hasCycle(startNodeId, transactionGraph)) {
        return "\n\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
               "!!! FRAUD ALERT: A potential cycle of transactions has been detected starting from account " + to_string(startNodeId) + ".\n"
               "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
    }
    return "";
}

string BankSystem::suggestConnections(int accountId) {
    stringstream ss;
    ss << "--- Transaction Connection Suggestions (BFS) ---\n";
    queue<pair<int, int>> q;
    set<int> visited;

    q.push({accountId, 0});
    visited.insert(accountId);

    bool found = false;
    while (!q.empty()) {
        pair<int, int> current = q.front();
        int currentId = current.first;
        int dist = current.second;
        q.pop();

        if (dist > 0 && dist <= 3) {
            found = true;
            ss << "  - Account #" << currentId << " (" << accounts.at(currentId).ownerName << "), "
                 << dist << " hop(s) away.\n";
        }

        if (dist < 3 && transactionGraph.adjList.count(currentId)) {
            for (const auto& edge : transactionGraph.adjList.at(currentId)) {
                if (visited.find(edge.first) == visited.end()) {
                    visited.insert(edge.first);
                    q.push({edge.first, dist + 1});
                }
            }
        }
    }
    if (!found) {
        ss << "No connections found within 3 hops.\n";
    }
    return ss.str();
}

string BankSystem::searchUser(const string& query) {
    stringstream ss;
    vector<pair<string, int>> results = userTrie.search(query);
    ss << "--- Search Results (Trie) ---\n";
    if (results.empty()) {
        ss << "No users found matching '" << query << "'.\n";
    } else {
        for (const auto& result : results) {
            ss << "  - Name: " << result.first << ", Account ID: " << result.second << "\n";
        }
    }
    return ss.str();
}

string BankSystem::getRecentTransactions(int accountId) {
    if (recentTransactionsCache.count(accountId)) {
        return recentTransactionsCache.at(accountId).getDisplayString(accounts.at(accountId).ownerName);
    }
    return "No recent payees to display from cache.";
}

string BankSystem::getTransactionHistory(int accountId) {
    if (accounts.find(accountId) == accounts.end()) {
        return "Error: Account not found.";
    }
    stringstream ss;
    ss << "--- Transaction History for " << accounts.at(accountId).ownerName << " ---\n";
    const auto& history = accounts.at(accountId).transactionHistory;
    if (history.empty()) {
        ss << "No transactions found.\n";
    } else {
        for (const string& trx : history) {
            ss << trx << "\n";
        }
    }
    return ss.str();
}

string BankSystem::getBalance(int accountId) {
    if (accounts.find(accountId) == accounts.end()) {
        return "Error: Account not found.";
    }
    stringstream ss;
    ss << fixed << setprecision(2) << accounts.at(accountId).balance;
    return ss.str();
}

string BankSystem::getAdminDashboard() {
    stringstream ss;
    ss << "========================\n";
    ss << "=== ADMIN DASHBOARD ===\n";
    ss << "========================\n\n";

    ss << creditSystem.getTopLowScoresString(5) << "\n";
    ss << transactionGraph.getGraphString() << "\n";

    ss << "--- User Transaction Clusters (Disjoint Set) ---\n";
    if (accounts.empty()) {
        ss << "No accounts to analyze.\n";
        return ss.str();
    }

    DisjointSet ds(nextAccountId);
    for (const auto& user_pair : transactionGraph.adjList) {
        int u = user_pair.first;
        for (const auto& edge : user_pair.second) {
            ds.unite(u, edge.first);
        }
    }
    
    unordered_map<int, vector<int>> clusters;
    for (const auto& acc_pair : accounts) {
        clusters[ds.find(acc_pair.first)].push_back(acc_pair.first);
    }

    int clusterCount = 1;
    bool foundClusters = false;
    for (const auto& cluster_pair : clusters) {
        const auto& members = cluster_pair.second;
        if (members.size() > 1) {
            foundClusters = true;
            ss << "Cluster " << clusterCount++ << ": ";
            for (size_t i = 0; i < members.size(); ++i) {
                ss << accounts.at(members[i]).ownerName << " (#" << members[i] << ")" << (i == members.size() - 1 ? "" : ", ");
            }
            ss << "\n";
        }
    }
    if (!foundClusters) {
        ss << "No significant transaction clusters found.\n";
    }
    return ss.str();
}