#ifndef TRIE_HPP
#define TRIE_HPP

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

using namespace std;

struct TrieNode {
    unordered_map<char, shared_ptr<TrieNode>> children;
    bool isEndOfWord;
    int accountId;

    TrieNode() : isEndOfWord(false), accountId(-1) {}
};

class Trie {
public:
    Trie();
    void insert(const string& key, int accountId);
    vector<pair<string, int>> search(const string& query);

private:
    shared_ptr<TrieNode> root;
    void findMatches(shared_ptr<TrieNode> node, string prefix, vector<pair<string, int>>& matches);
};

#endif