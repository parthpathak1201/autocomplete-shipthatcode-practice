#include <string>
#include <iostream>
#include <set>
#include <memory>
#include <vector>
#include <unordered_set>
#include <unordered_map>

//#include <bits/stdc++.h>


// TODO (trie-basics): implement per the lesson description.

//One node - holds a vector of children. Holds an end bool flag + freq count


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
    }

    void find(const std::string &word) const {
        auto cursor = root.get();
        for (const auto &c: word) {
            const size_t index = c - 'a';
            if (!cursor->children[index]) {
                std::cout << "0\n";
                return;
            }
            cursor = cursor->children[index].get();
        }

        //we are at the end.
        (cursor->end) ? std::cout << cursor->frequency << "\n" : std::cout << "0\n";
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
        }
    }
}
