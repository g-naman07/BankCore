#ifndef ACCOUNT_HPP
#define ACCOUNT_HPP

#include <string>
#include <vector>
#include <ctime>
#include <utility>

using namespace std;

class Account {
public:
    int accountId;
    string ownerName;
    string pin;
    double balance;
    vector<string> transactionHistory;

    Account(int id = 0, string name = "", string p = "", double b = 0.0)
        : accountId(id), ownerName(move(name)), pin(move(p)), balance(b) {}

    void deposit(double amount) {
        balance += amount;
    }

    bool withdraw(double amount) {
        if (balance >= amount) {
            balance -= amount;
            return true;
        }
        return false;
    }

    void addTransaction(const string& type, double amount, int counterpartyId = -1) {
        time_t now = time(0);
        char* dt = ctime(&now);
        string transaction_detail = string(dt);
        transaction_detail.pop_back();
        transaction_detail += " | " + type + " $" + to_string(amount);
        if(counterpartyId != -1){
            transaction_detail += " to/from Acct #" + to_string(counterpartyId);
        }
        transactionHistory.push_back(transaction_detail);
    }
};

#endif