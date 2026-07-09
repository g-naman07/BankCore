#include "Graph.hpp"
#include <iostream>
#include <sstream>

using namespace std;

void Graph::addEdge(int u, int v, double amount) {
    adjList[u].push_back({v, amount});
}

string Graph::getGraphString() {
    stringstream ss;
    ss << "--- Transaction Graph ---\n";
    if (adjList.empty()){
        ss << "No transactions have been made yet.\n";
        return ss.str();
    }
    for (const auto& pair : adjList) {
        ss << "User " << pair.first << " has transacted with: \n";
        for (const auto& trans : pair.second) {
            ss << "  -> User " << trans.first << " ($" << trans.second << ")\n";
        }
    }
    return ss.str();
}