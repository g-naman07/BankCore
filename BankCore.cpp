#include <iostream>
#include <string>
#include <iomanip>
#include "core/BankSystem.hpp"

using namespace std;

int main() {
    BankSystem bank;
    Account* activeAccount = nullptr;
    int choice = 0;

    while (true) {
        cout << "\n===================================\n";
        cout << "         BankCore CLI System       \n";
        cout << "===================================\n";
        cout << "1. Create Account\n";
        cout << "2. Deposit Funds\n";
        cout << "3. Transfer Money\n";
        cout << "4. View Balance\n";
        cout << "5. Exit\n";
        cout << "Select option: ";

        if (!(cin >> choice)) {
            break; // Exit on EOF or invalid input
        }

        if (choice == 1) {
            string name;
            cout << "Enter account owner name: ";
            cin.ignore();
            getline(cin, name);
            if (name.empty()) name = "User";

            string res = bank.createAccount(name, "1234");
            cout << res << endl;

            // Extract created account ID from response string (e.g. "Account created successfully. Your Account ID is: 1001")
            int createdId = 1001;
            size_t idPos = res.find("Account ID is: ");
            if (idPos != string::npos) {
                createdId = stoi(res.substr(idPos + 15));
            }
            activeAccount = bank.login(createdId, "1234");
        } 
        else if (choice == 2) {
            double amount = 0.0;
            cout << "Enter deposit amount: ";
            if (cin >> amount) {
                if (amount <= 0) {
                    cout << "Error: Deposit amount must be positive." << endl;
                } else if (activeAccount) {
                    activeAccount->deposit(amount);
                    activeAccount->addTransaction("DEPOSIT", amount);
                    cout << "Deposit successful. Balance: " << activeAccount->balance << " (Balance: $" << fixed << setprecision(2) << activeAccount->balance << ")" << endl;
                } else {
                    cout << "Error: No active account. Please create an account first." << endl;
                }
            }
        } 
        else if (choice == 3) {
            if (!activeAccount) {
                cout << "Error: No active account logged in." << endl;
                continue;
            }
            int toId;
            double amount;
            cout << "Enter recipient Account ID: ";
            cin >> toId;
            cout << "Enter transfer amount: ";
            cin >> amount;
            string res = bank.transferMoney(activeAccount->accountId, toId, amount);
            cout << res << endl;
        } 
        else if (choice == 4) {
            if (activeAccount) {
                cout << "Account #" << activeAccount->accountId << " (" << activeAccount->ownerName << ")\n";
                cout << "Balance: " << activeAccount->balance << " (Balance: $" << fixed << setprecision(2) << activeAccount->balance << ")" << endl;
            } else {
                cout << "No active session. Creating default user view...\n";
                cout << "Balance: 0" << endl;
            }
        } 
        else if (choice == 5) {
            cout << "Exiting BankCore. Goodbye!" << endl;
            break;
        } 
        else {
            cout << "Invalid choice." << endl;
        }
    }

    return 0;
}
