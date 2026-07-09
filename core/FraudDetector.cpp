#include "FraudDetector.hpp"

using namespace std;

bool FraudDetector::isCyclicUtil(int v, const Graph& graph, unordered_map<int, bool>& visited, unordered_map<int, bool>& recursionStack) {
    if (!visited[v]) {
        visited[v] = true;
        recursionStack[v] = true;

        if (graph.adjList.count(v)) {
            for (const auto& edge : graph.adjList.at(v)) {
                int neighbor = edge.first;
                if (!visited[neighbor] && isCyclicUtil(neighbor, graph, visited, recursionStack)) {
                    return true;
                } else if (recursionStack[neighbor]) {
                    return true;
                }
            }
        }
    }
    recursionStack[v] = false;
    return false;
}

bool FraudDetector::hasCycle(int startNode, const Graph& graph) {
    unordered_map<int, bool> visited;
    unordered_map<int, bool> recursionStack;

    return isCyclicUtil(startNode, graph, visited, recursionStack);
}