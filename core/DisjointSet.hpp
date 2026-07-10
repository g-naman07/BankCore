#ifndef DISJOINT_SET_HPP
#define DISJOINT_SET_HPP

#include <vector>
#include <numeric>
#include <unordered_map>
#include <iostream>

using namespace std;

class DisjointSet {
public:
    DisjointSet(int size) {
        parent.resize(size + 1);
        iota(parent.begin(), parent.end(), 0);
        rank.resize(size + 1, 0);
    }

    int find(int i) {
        if (parent.at(i) == i)
            return i;
        return parent.at(i) = find(parent.at(i));
    }

    void unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);
        if (root_i != root_j) {
            if (rank.at(root_i) < rank.at(root_j))
                parent.at(root_i) = root_j;
            else if (rank.at(root_i) > rank.at(root_j))
                parent.at(root_j) = root_i;
            else {
                parent.at(root_j) = root_i;
                rank.at(root_i)++;
            }
        }
    }

private:
    vector<int> parent;
    vector<int> rank;
};

#endif