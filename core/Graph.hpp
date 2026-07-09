#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <unordered_map>
#include <string>
#include <utility>

using namespace std;

class Graph {
public:
    unordered_map<int, vector<pair<int, double>>> adjList;

    void addEdge(int u, int v, double amount);
    string getGraphString();
};

#endif