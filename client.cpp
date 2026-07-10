#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <cstring>
#include <iomanip>
#include <sstream>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    using SOCKET = int;
    const int INVALID_SOCKET = -1;
    const int SOCKET_ERROR = -1;
    #define closesocket close
#endif

using namespace std;

SOCKET sock = 0;

string sendRequest(const string& command) {
    send(sock, command.c_str(), command.length(), 0);

    char buffer[2048] = {0};
    string response = "";
    int bytes_received;
    
    do {
        bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            response += buffer;
        }
    } while (bytes_received > 0 && bytes_received == sizeof(buffer) - 1);

    return response;
}

void clearInput() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void pressEnterToContinue() {
    cout << "\nPress Enter to continue...";
    cin.get();
}

void handleCreateAccount() {
    string name, pin;
    cout << "\n--- Create New Account ---" << endl;
    cout << "Enter your full name: ";
    getline(cin, name);
    cout << "Create a 4-digit PIN: ";
    getline(cin, pin);

    string command = "create " + pin + " " + name;
    string response = sendRequest(command);
    cout << "\n[SERVER RESPONSE]\n" << response << endl;
    pressEnterToContinue();
}

void loggedInMenu(int accountId, const string& userName, double balance) {
    int choice = 0;
    while (choice != 6) {
        cout << "\n===================================" << endl;
        cout << "--- Welcome " << userName << " (Account #" << accountId << ") ---" << endl;
        cout << "Balance: $" << fixed << setprecision(2) << balance << endl;
        cout << "===================================" << endl;
        cout << "1. Transfer Money" << endl;
        cout << "2. View Transaction History" << endl;
        cout << "3. Search for a User" << endl;
        cout << "4. View Recent Payees" << endl;
        cout << "5. Connection Suggestions" << endl;
        cout << "6. Logout" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            clearInput();
            choice = 0;
            cout << "Invalid input. Please enter a number." << endl;
            pressEnterToContinue();
            continue;
        }
        clearInput();

        if (choice == 1) {
            int toId;
            double amount;
            cout << "Enter recipient Account ID: ";
            cin >> toId;
            cout << "Enter amount: $";
            cin >> amount;
            clearInput();
            string command = "transfer " + to_string(accountId) + " " + to_string(toId) + " " + to_string(amount);
            string response = sendRequest(command);
            cout << "\n[SERVER RESPONSE]\n" << response << endl;
            
            string balance_response = sendRequest("balance " + to_string(accountId));
            try {
                balance = stod(balance_response);
            } catch(...) { }
            pressEnterToContinue();
        } else if (choice >= 2 && choice <= 5) {
            string command;
            if (choice == 2) command = "history " + to_string(accountId);
            if (choice == 3) {
                string query;
                cout << "Enter name prefix to search: ";
                getline(cin, query);
                command = "search " + query;
            }
            if (choice == 4) command = "recent " + to_string(accountId);
            if (choice == 5) command = "connections " + to_string(accountId);
            
            string response = sendRequest(command);
            cout << "\n[SERVER RESPONSE]\n" << response << endl;
            pressEnterToContinue();
        }
    }
}

void handleLogin() {
    int id;
    string pin;
    cout << "\n--- Login ---" << endl;
    cout << "Enter Account ID: ";
    cin >> id;
    cout << "Enter PIN: ";
    cin >> pin;
    clearInput();

    string command = "login " + to_string(id) + " " + pin;
    string response = sendRequest(command);

    if (response.rfind("OK", 0) == 0) {
        stringstream ss(response);
        string status, userName;
        double balance;
        ss >> status >> userName >> balance;
        loggedInMenu(id, userName, balance);
    } else {
        cout << "\n[SERVER RESPONSE]\n" << response << endl;
        pressEnterToContinue();
    }
}

void handleAdminDashboard() {
    string response = sendRequest("admin");
    cout << "\n[SERVER RESPONSE]\n" << response << endl;
    pressEnterToContinue();
}

int main() {
    #ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            cerr << "WSAStartup failed." << endl;
            return 1;
        }
    #endif

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        cout << "Socket creation error" << endl;
        return -1;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(9090);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        cout << "Connection Failed. Is the server running?" << endl;
        return -1;
    }

    cout << "Connected to AlgoBank Server." << endl;

    int choice = 0;
    while (choice != 4) {
        cout << "\n===================================" << endl;
        cout << "=== Welcome to AlgoBank System ===" << endl;
        cout << "===================================" << endl;
        cout << "1. Login" << endl;
        cout << "2. Create Account" << endl;
        cout << "3. Admin Dashboard" << endl;
        cout << "4. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            clearInput();
            choice = 0;
            cout << "Invalid input. Please enter a number." << endl;
            pressEnterToContinue();
            continue;
        }
        clearInput();

        switch (choice) {
            case 1: handleLogin(); break;
            case 2: handleCreateAccount(); break;
            case 3: handleAdminDashboard(); break;
            case 4: cout << "Exiting. Goodbye!" << endl; break;
            default: cout << "Invalid choice. Please try again." << endl; pressEnterToContinue(); break;
        }
    }

    closesocket(sock);
    #ifdef _WIN32
        WSACleanup();
    #endif
    return 0;
}