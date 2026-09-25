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

//one node - holds a vector of children. Holds an end bool flag + freq count

struct Compare {
    bool operator()(const std::pair<size_t, std::string> &a, const std::pair<size_t, std::string> &b) {
        if (a.first == b.first) {
            return a.second < b.second; // alphabetical tie-breaker (lexicographically larger at top to pop it first)
        }
        return a.first > b.first; // min-heap based on frequency
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

//holds 1 root + methods for operations. That root is a 'Node' object and it will hold everything inside it.
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
        // optional safety check: check if it exists first
        deleteHelper(root.get(), word, 0);
    }

    // returns true if the current node can be safely deleted by its parent
    bool deleteHelper(Node *cursor, const std::string &word, size_t depth) {
        // base case: reached the end of the word
        if (depth == word.length()) {
            if (!cursor->end) return false; // Word doesn't exist

            cursor->end = false;
            cursor->frequency = 0;

            // remove from global nodes tracking set if you use it
            nodes.erase(cursor);

            // return true if this node has no children, meaning it can be deleted
            return hasNoChildren(cursor);
        }

        size_t index = word[depth] - 'a';
        if (!cursor->children[index]) {
            return false; // word not found
        }

        // recurse down
        bool shouldDeleteChild = deleteHelper(cursor->children[index].get(), word, depth + 1);

        if (shouldDeleteChild) {
            // reset/destroy the unique_ptr, freeing the node
            nodes.erase(cursor->children[index].get());
            cursor->children[index].reset();

            // return true if current node is also safe to delete (not an end of another word and has no other children)
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

        //we are at the end
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
                return; // prefix doesn't exist in trie
            }
            cursor = cursor->children[index].get();
        }

        MinHeap pq;
        std::string trail;

        // start DFS from the end of the prefix node
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
            // move into it
            cursor = cursor->children[index].get();
        }
        // set the terminal state and frequency explicitly
        cursor->end = true;
        cursor->frequency = freq; // set directly instead of just ++
    }

    void fuzzyDfs(const Node *cursor, const std::string &query, size_t maxDistance,
                  const std::vector<size_t> &row, std::string &trail,
                  std::vector<std::string> &matches) const {
        for (size_t i = 0; i < cursor->children.size(); ++i) {
            if (!cursor->children[i]) continue;

            std::vector<size_t> nextRow(query.size() + 1);
            nextRow[0] = row[0] + 1;
            size_t minDistance = nextRow[0];
            for (size_t j = 1; j <= query.size(); ++j) {
                const size_t cost = query[j - 1] == static_cast<char>('a' + i) ? 0 : 1;
                nextRow[j] = std::min({nextRow[j - 1] + 1, row[j] + 1, row[j - 1] + cost});
                minDistance = std::min(minDistance, nextRow[j]);
            }

            if (minDistance > maxDistance) continue;

            trail.push_back(static_cast<char>('a' + i));
            if (cursor->children[i]->end && nextRow.back() <= maxDistance) {
                matches.push_back(trail + "(" + std::to_string(nextRow.back()) + ")");
            }
            fuzzyDfs(cursor->children[i].get(), query, maxDistance, nextRow, trail, matches);
            trail.pop_back();
        }
    }

    void fuzzy(const std::string &query, int maxDistance) const {
        if (maxDistance < 0) {
            std::cout << "none\n";
            return;
        }

        std::vector<size_t> row(query.size() + 1);
        for (size_t i = 0; i <= query.size(); ++i) row[i] = i;

        std::vector<std::string> matches;
        std::string trail;
        fuzzyDfs(root.get(), query, static_cast<size_t>(maxDistance), row, trail, matches);

        if (matches.empty()) {
            std::cout << "none\n";
            return;
        }
        for (size_t i = 0; i < matches.size(); ++i) {
            if (i > 0) std::cout << ",";
            std::cout << matches[i];
        }
        std::cout << "\n";
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
        } else if (tokens[0] == "FUZZY") {
            trie.fuzzy(tokens[1], std::stoi(tokens[2]));
        }
    }
}
