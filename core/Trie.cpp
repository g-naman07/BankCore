#include "Trie.hpp"

using namespace std;

Trie::Trie() {
    root = make_shared<TrieNode>();
}

void Trie::insert(const string& key, int accountId) {
    shared_ptr<TrieNode> current = root;
    for (char ch : key) {
        if (current->children.find(ch) == current->children.end()) {
            current->children[ch] = make_shared<TrieNode>();
        }
        current = current->children[ch];
    }
    current->isEndOfWord = true;
    current->accountId = accountId;
}

vector<pair<string, int>> Trie::search(const string& query) {
    shared_ptr<TrieNode> current = root;
    for (char ch : query) {
        if (current->children.find(ch) == current->children.end()) {
            return {};
        }
        current = current->children[ch];
    }
    
    vector<pair<string, int>> matches;
    findMatches(current, query, matches);
    return matches;
}

void Trie::findMatches(shared_ptr<TrieNode> node, string prefix, vector<pair<string, int>>& matches) {
    if (node->isEndOfWord) {
        matches.push_back({prefix, node->accountId});
    }
    for (const auto& pair : node->children) {
        findMatches(pair.second, prefix + pair.first, matches);
    }
}