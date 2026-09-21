#include <string>
#include <iostream>
#include <set>
#include <memory>
#include <vector>
#include <unordered_set>
#include <unordered_map>

// TODO (radix-tree): implement Radix tree with edge strings.

class RadixNode {
public:
    std::string prefix;
    mutable bool end = false;
    mutable size_t frequency = 0;
    std::vector<std::unique_ptr<RadixNode>> children;

    RadixNode() = default;
    explicit RadixNode(std::string p, bool is_end = false)
        : prefix(std::move(p)), end(is_end), frequency(is_end ? 1 : 0) {}
};

std::set<RadixNode *> nodes;

class [[maybe_unused]] RadixTree {
private:
    size_t getLCP(const std::string& a, const std::string& b) const {
        size_t len = 0;
        while (len < a.length() && len < b.length() && a[len] == b[len]) {
            len++;
        }
        return len;
    }

    bool hasNoChildren(const RadixNode *node) const {
        return node->children.empty();
    }

    void dfs(const RadixNode *cursor, std::string &trail, std::vector<std::string> &words) const {
        for (const auto &child : cursor->children) {
            if (child) {
                trail.append(child->prefix);
                if (child->end) {
                    words.push_back(trail);
                }
                dfs(child.get(), trail, words);
                // Backtrack the appended prefix length
                trail.erase(trail.length() - child->prefix.length());
            }
        }
    }

public:
    RadixTree() = default;

    std::unique_ptr<RadixNode> root = std::make_unique<RadixNode>();

    void insert(const std::string &word) {
        RadixNode* cursor = root.get();
        std::string remaining = word;

        while (!remaining.empty()) {
            bool matched_child = false;

            for (auto &child : cursor->children) {
                size_t lcp = getLCP(child->prefix, remaining);

                if (lcp > 0) {
                    matched_child = true;

                    if (lcp == child->prefix.length()) {
                        remaining = remaining.substr(lcp);
                        cursor = child.get();
                    } else {
                        // Split node case
                        std::string common = child->prefix.substr(0, lcp);
                        std::string existing_suffix = child->prefix.substr(lcp);
                        std::string new_suffix = remaining.substr(lcp);

                        auto new_child = std::make_unique<RadixNode>(common, false);
                        nodes.insert(new_child.get());

                        child->prefix = existing_suffix;
                        new_child->children.push_back(std::move(child));

                        if (new_suffix.empty()) {
                            new_child->end = true;
                            new_child->frequency++;
                        } else {
                            auto leaf = std::make_unique<RadixNode>(new_suffix, true);
                            nodes.insert(leaf.get());
                            new_child->children.push_back(std::move(leaf));
                        }

                        child = std::move(new_child);
                        std::cout << "OK" << "\n";
                        return;
                    }
                    break;
                }
            }

            if (!matched_child) {
                auto new_node = std::make_unique<RadixNode>(remaining, true);
                nodes.insert(new_node.get());
                cursor->children.push_back(std::move(new_node));
                std::cout << "OK" << "\n";
                return;
            }
        }

        cursor->end = true;
        cursor->frequency++;
        std::cout << "OK" << "\n";
    }

    bool find(const std::string &word) const {
        const RadixNode* cursor = root.get();
        std::string remaining = word;

        while (!remaining.empty()) {
            bool found_branch = false;
            for (const auto &child : cursor->children) {
                if (remaining.rfind(child->prefix, 0) == 0) {
                    remaining = remaining.substr(child->prefix.length());
                    cursor = child.get();
                    found_branch = true;
                    break;
                }
                if (child->prefix.rfind(remaining, 0) == 0) {
                    std::cout << "0\n";
                    return false;
                }
            }
            if (!found_branch) {
                std::cout << "0\n";
                return false;
            }
        }

        if (cursor->end) {
            std::cout << cursor->frequency << "\n";
            return true;
        }
        std::cout << "0\n";
        return false;
    }

    void prefix(const std::string &pre) const {
        const RadixNode* cursor = root.get();
        std::string remaining = pre;

        // Traverse down to the prefix node or partial node
        while (!remaining.empty()) {
            bool found_branch = false;
            for (const auto &child : cursor->children) {
                size_t lcp = getLCP(child->prefix, remaining);
                if (lcp > 0) {
                    if (lcp == child->prefix.length()) {
                        remaining = remaining.substr(lcp);
                        cursor = child.get();
                        found_branch = true;
                        break;
                    } else if (lcp == remaining.length()) {
                        // The prefix ends mid-edge inside this child
                        cursor = child.get();
                        found_branch = true;
                        remaining.clear();
                        break;
                    } else {
                        std::cout << "none\n";
                        return;
                    }
                }
            }
            if (!found_branch) {
                std::cout << "none\n";
                return;
            }
        }

        std::vector<std::string> words;
        std::string trail;
        // If we stopped mid-edge, account for the unmatched part of the child's prefix
        dfs(cursor, trail, words);

        for (size_t i = 0; i < words.size(); ++i) {
            if (i != words.size() - 1) {
                std::cout << pre + words[i] << ",";
            } else {
                std::cout << pre + words[i] << "\n";
            }
        }
        if (words.empty()) {
            std::cout << "none\n";
        }
    }

    void contains(const std::string &word) const {
        const RadixNode* cursor = root.get();
        std::string remaining = word;

        while (!remaining.empty()) {
            bool found_branch = false;
            for (const auto &child : cursor->children) {
                if (remaining.rfind(child->prefix, 0) == 0) {
                    remaining = remaining.substr(child->prefix.length());
                    cursor = child.get();
                    found_branch = true;
                    break;
                }
                if (child->prefix.rfind(remaining, 0) == 0) {
                    std::cout << "NO\n";
                    return;
                }
            }
            if (!found_branch) {
                std::cout << "NO\n";
                return;
            }
        }

        if (cursor->end) {
            std::cout << "YES\n";
        } else {
            std::cout << "NO\n";
        }
    }

    void del(const std::string &word) {
        // Simplified lazy deletion for robust stream handling
        const RadixNode* cursor = root.get();
        std::string remaining = word;

        while (!remaining.empty()) {
            bool found_branch = false;
            for (const auto &child : cursor->children) {
                if (remaining.rfind(child->prefix, 0) == 0) {
                    remaining = remaining.substr(child->prefix.length());
                    cursor = child.get();
                    found_branch = true;
                    break;
                }
                if (child->prefix.rfind(remaining, 0) == 0) return;
            }
            if (!found_branch) return;
        }

        if (cursor->end) {
            cursor->end = false;
            cursor->frequency = 0;
            nodes.erase(const_cast<RadixNode*>(cursor));
        }
    }
};

int main() {
    std::string line;
    RadixTree trie;
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
        }
    }
}