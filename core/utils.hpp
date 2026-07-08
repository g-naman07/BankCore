#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <limits>

using namespace std;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pressEnterToContinue() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if(cin.gcount() == 0) {
        cin.get();
    }
}

#endif