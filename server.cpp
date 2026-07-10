#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <pthread.h>
    using SOCKET = int;
    const int INVALID_SOCKET = -1;
    const int SOCKET_ERROR = -1;
    #define closesocket close
#endif

#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <sstream>
#include <iomanip>

#include "core/BankSystem.hpp"

using namespace std;

BankSystem bank;
CRITICAL_SECTION bankMutex;

void sendResponse(SOCKET client_socket, const string& response) {
    send(client_socket, response.c_str(), response.length(), 0);
}

DWORD WINAPI handleClient(LPVOID client_socket_ptr) {
    SOCKET client_socket = (SOCKET)(ULONG_PTR)client_socket_ptr;
    cout << "[SERVER] New client connected on socket " << client_socket << endl;
    char buffer[1024];

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

        if (bytes_received <= 0) {
            cout << "[SERVER] Client on socket " << client_socket << " disconnected." << endl;
            break;
        }

        string command_str(buffer, bytes_received);
        cout << "[SERVER] Received command: \"" << command_str << "\" from socket " << client_socket << endl;

        string response;
        istringstream iss(command_str);
        string cmd;
        iss >> cmd;

        EnterCriticalSection(&bankMutex);

        try {
            if (cmd == "create") {
                string pin, name;
                iss >> pin;
                getline(iss, name);
                if (!name.empty() && name[0] == ' ') name = name.substr(1);
                response = bank.createAccount(name, pin);
            } else if (cmd == "login") {
                int id;
                string pin;
                iss >> id >> pin;
                Account* acc = bank.login(id, pin);
                if (acc) {
                    stringstream ss;
                    ss << "OK " << acc->ownerName << " " << fixed << setprecision(2) << acc->balance;
                    response = ss.str();
                } else {
                    response = "Login failed: Invalid credentials.";
                }
            } else if (cmd == "transfer") {
                int from, to;
                double amt;
                iss >> from >> to >> amt;
                response = bank.transferMoney(from, to, amt);
            } else if (cmd == "balance") {
                int id;
                iss >> id;
                response = bank.getBalance(id);
            } else if (cmd == "history") {
                int id;
                iss >> id;
                response = bank.getTransactionHistory(id);
            } else if (cmd == "search") {
                string query;
                getline(iss, query);
                if (!query.empty() && query[0] == ' ') query = query.substr(1);
                response = bank.searchUser(query);
            } else if (cmd == "recent") {
                int id;
                iss >> id;
                response = bank.getRecentTransactions(id);
            } else if (cmd == "connections") {
                int id;
                iss >> id;
                response = bank.suggestConnections(id);
            } else if (cmd == "admin") {
                response = bank.getAdminDashboard();
            } else {
                response = "Invalid command.";
            }
        } catch (const exception& e) {
            response = "Server error: " + string(e.what());
        } catch (...) {
            response = "An unknown server error occurred.";
        }

        LeaveCriticalSection(&bankMutex);
        sendResponse(client_socket, response);
    }

    closesocket(client_socket);
    return 0;
}

int main() {
    #ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            cerr << "WSAStartup failed." << endl;
            return 1;
        }
    #endif

    InitializeCriticalSection(&bankMutex);

    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        perror("socket failed");
        return 1;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(9090);

   if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
    cerr << "bind failed: " << WSAGetLastError() << endl;
    closesocket(server_fd);
    return 1;
}

    if (listen(server_fd, 10) == SOCKET_ERROR) {
        perror("listen");
        closesocket(server_fd);
        return 1;
    }

    cout << "[SERVER] AlgoBank server is listening on port 9090..." << endl;

    while (true) {
        SOCKET new_socket = accept(server_fd, nullptr, nullptr);
        if (new_socket == INVALID_SOCKET) {
            perror("accept");
            continue;
        }

        HANDLE threadHandle = CreateThread(NULL, 0, handleClient, (LPVOID)(ULONG_PTR)new_socket, 0, NULL);
        if (threadHandle) {
            CloseHandle(threadHandle);
        }
    }

    closesocket(server_fd);
    DeleteCriticalSection(&bankMutex);
    #ifdef _WIN32
        WSACleanup();
    #endif
    return 0;
}
