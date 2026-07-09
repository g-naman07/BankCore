#ifndef FRAUD_DETECTOR_HPP
#define FRAUD_DETECTOR_HPP

#include "Graph.hpp"
#include <unordered_map>

using namespace std;

class FraudDetector {
public:
    bool hasCycle(int startNode, const Graph& graph);

private:
    bool isCyclicUtil(int v, const Graph& graph, unordered_map<int, bool>& visited, unordered_map<int, bool>& recursionStack);
};

#endif