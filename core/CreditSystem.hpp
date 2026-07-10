#ifndef CREDIT_SYSTEM_HPP
#define CREDIT_SYSTEM_HPP

#include "Account.hpp"
#include <queue>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

struct CreditScore {
    int accountId;
    double score;

    bool operator>(const CreditScore& other) const {
        return score > other.score;
    }
};

class CreditSystem {
public:
    void updateScore(Account& account, double transactionAmount, bool isSender);
    double getScore(int accountId);
    string getTopLowScoresString(int count);

private:
    unordered_map<int, double> scores;
};

#endif