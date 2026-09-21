#include <string>
#include <iostream>
#include <set>
#include <memory>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <stack>
#include <functional>
#include <algorithm>


//#include <bits/stdc++.h>


// TODO (trie-basics): implement per the lesson description.

//One node - holds a vector of children. Holds an end bool flag + freq count

struct Compare {
    bool operator()(const std::pair<size_t, std::string> &a, const std::pair<size_t, std::string> &b) {
        if (a.first == b.first) {
            return a.second < b.second; // Alphabetical tie-breaker (lexicographically larger at top to pop it first)
        }
        return a.first > b.first; // Min-heap based on frequency
    }
};

using MinHeap = std::priority_queue<std::pair<size_t, std::string>,
    std::vector<std::pair<size_t, std::string> >,
    Compare>;

class Node {
public:
    bool end = false;
    size_t frequency = 0;
    std::vector<std::unique_ptr<Node> > children{26};
    //by default the unique_ptr is a nullptr - RAII practice - no need for manual cleanup.
    //used unique_ptr because we might need to track this node from multiple places
};

std::set<Node *> nodes;

//Holds 1 root + methods for operations. That root is a 'Node' object and it will hold everything inside it.
class [[maybe_unused]] Trie {
public:
    Trie() = default;

    std::unique_ptr<Node> root = std::make_unique<Node>();


    void insert(const std::string &word) const {
        auto cursor = root.get();
        for (const auto &c: word) {
            const size_t index = c - 'a';
            if (!cursor->children[index]) {
                //if this branch node doesnt exist - create it
                cursor->children[index] = std::make_unique<Node>();
                nodes.insert(cursor->children[index].get());
            }
            //exists - move into it
            cursor = cursor->children[index].get();
        }
        //we are at the end.
        cursor->end = true;
        cursor->frequency++;

        //std::cout << "OK" << "\n";
    }

    bool find(const std::string &word) const {
        auto cursor = root.get();
        for (const auto &c: word) {
            const size_t index = c - 'a';
            if (!cursor->children[index]) {
                std::cout << "0\n";
                return false;
            }
            cursor = cursor->children[index].get();
        }

        //we are at the end.
        (cursor->end) ? std::cout << cursor->frequency << "\n" : std::cout << "0\n";
        return true;
    }

    void dfs(const Node *cursor, std::string &trail, std::vector<std::string> &words) {
        for (int i = 0; i < 26; ++i) {
            if (cursor->children[i]) {
                trail.push_back(static_cast<char>('a' + i));
                if (cursor->children[i]->end) {
                    words.push_back(trail);
                }
                dfs(cursor->children[i].get(), trail, words);
                if (!trail.empty()) {
                    trail.pop_back();
                }
            }
        }
    }


    void prefix(const std::string &pre) {
        auto cursor = root.get();
        for (const auto &c: pre) {
            const size_t index = c - 'a';
            if (!cursor->children[index]) {
                std::cout << "none\n";
                return;
            }
            cursor = cursor->children[index].get();
        }
        //cursor points at the last letter of the given prefix
        std::vector<std::string> words;
        std::string trail;
        dfs(cursor, trail, words);
        for (auto &word: words) {
            if (word != words.back()) {
                std::cout << pre + word << ",";
            } else {
                std::cout << pre + word << "\n";
            }
        }
    }

    void del(const std::string &word) {
        // Optional safety check: check if it exists first
        deleteHelper(root.get(), word, 0);
    }

    // Returns true if the current node can be safely deleted by its parent
    bool deleteHelper(Node *cursor, const std::string &word, size_t depth) {
        // Base case: reached the end of the word
        if (depth == word.length()) {
            if (!cursor->end) return false; // Word doesn't exist

            cursor->end = false;
            cursor->frequency = 0;

            // Remove from global nodes tracking set if you use it
            nodes.erase(cursor);

            // Return true if this node has no children, meaning it can be deleted
            return hasNoChildren(cursor);
        }

        size_t index = word[depth] - 'a';
        if (!cursor->children[index]) {
            return false; // Word not found
        }

        // Recurse down
        bool shouldDeleteChild = deleteHelper(cursor->children[index].get(), word, depth + 1);

        if (shouldDeleteChild) {
            // Reset/destroy the unique_ptr, freeing the node
            nodes.erase(cursor->children[index].get());
            cursor->children[index].reset();

            // Return true if current node is also safe to delete (not an end of another word and has no other children)
            return !cursor->end && hasNoChildren(cursor);
        }

        return false;
    }

    bool hasNoChildren(const Node *node) const {
        for (const auto &child: node->children) {
            if (child) return false;
        }
        return true;
    }

    void contains(const std::string &word) const {
        auto cursor = root.get();
        for (const auto &c: word) {
            const size_t index = c - 'a';
            if (!cursor->children[index]) {
                std::cout << "NO\n";
                return;
            }
            cursor = cursor->children[index].get();
        }

        //we are at the end.
        (cursor->end) ? std::cout << "YES" << "\n" : std::cout << "NO\n";
    }

    void dfsK(const Node *cursor, std::string &trail, MinHeap &pq, int k, std::string &pre) {
        for (int i = 0; i < 26; ++i) {
            if (cursor->children[i]) {
                trail.push_back(static_cast<char>('a' + i));
                if (cursor->children[i]->end) {
                    pq.emplace(cursor->children[i]->frequency, pre + trail);
                    if (pq.size() > k) {
                        pq.pop();
                    }
                }
                dfsK(cursor->children[i].get(), trail, pq, k, pre);
                if (!trail.empty()) {
                    trail.pop_back();
                }
            }
        }
    }

    void topK(std::string &pre, int k) {
        auto cursor = root.get();
        for (const auto &c: pre) {
            const size_t index = c - 'a';
            if (!cursor->children[index]) {
                std::cout << "none\n";
                return; // Prefix doesn't exist in trie
            }
            cursor = cursor->children[index].get();
        }

        MinHeap pq;
        std::string trail;

        // Start DFS from the end of the prefix node
        dfsK(cursor, trail, pq, k, pre);

        std::vector<std::pair<size_t, std::string> > words;
        while (!pq.empty()) {
            words.emplace_back(pq.top());
            pq.pop();
        }
        std::reverse(words.begin(), words.end());

        for (const auto &pair: words) {
            auto word = pair.second;
            auto freq = pair.first;
            std::cout << word << "(" << freq << ")";
            if (pair != words.back()) {
                std::cout << ",";
            }
        }
        std::cout << "\n";
    }

    void insertN(const std::string &word, size_t freq) const {
        auto cursor = root.get();
        for (const auto &c: word) {
            const size_t index = c - 'a';
            if (!cursor->children[index]) {
                // If this branch node doesn't exist - create it
                cursor->children[index] = std::make_unique<Node>();
                nodes.insert(cursor->children[index].get());
            }
            // Move into it
            cursor = cursor->children[index].get();
        }
        // Set the terminal state and frequency explicitly
        cursor->end = true;
        cursor->frequency = freq; // Set directly instead of just ++
    }
};


int main() {
    std::string line;
    Trie trie;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        std::vector<std::string> tokens;
        size_t start = 0;
        for (size_t i = 0; i <= line.length(); ++i) {
            if (i == line.length() || line[i] == ' ') {
                tokens.push_back(line.substr(start, i - start));
                start = i + 1;
            }
        }

        if (tokens.size() != 2) {
            if (tokens[0] == "NODES") {
                std::cout << nodes.size() + 1 << "\n";
            }
        }

        if (tokens[0] == "INSERT") {
            trie.insert(tokens[1]);
        } else if (tokens[0] == "FREQ") {
            trie.find(tokens[1]);
        } else if (tokens[0] == "PREFIX") {
            trie.prefix(tokens[1]);
        } else if (tokens[0] == "DELETE") {
            trie.del(tokens[1]);
        } else if (tokens[0] == "CONTAINS") {
            trie.contains(tokens[1]);
        } else if (tokens[0] == "SUGGEST") {
            trie.topK(tokens[1], std::stoi(tokens[2]));
        } else if (tokens[0] == "INSERT_N") {
            if (tokens.size() >= 3) {
                trie.insertN(tokens[1], std::stoul(tokens[2]));
            }
        }
    }
}
