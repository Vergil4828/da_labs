#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <type_traits>
#include <vector>

template <typename T>
concept Numeric = std::is_scalar_v<T>;

template <Numeric T>
class TrieNode {
   public:
    std::string k;
    T val;
    size_t bit_idx;
    TrieNode *l_ptr, *r_ptr;

    TrieNode(const std::string& key, T value, size_t idx)
        : k(key), val(value), bit_idx(idx), l_ptr(nullptr), r_ptr(nullptr) {}
};

template <Numeric T>
class TrieMap {
   private:
    TrieNode<T>* head = nullptr;

    inline uint8_t fetch_bit(const std::string& key, size_t bit_pos) {
        if (bit_pos == 0) return 0;
        size_t char_idx = (bit_pos - 1) >> 3;
        if (char_idx >= key.length()) return 0;
        return (static_cast<uint8_t>(key[char_idx]) >> (7 - ((bit_pos - 1) & 7))) & 1;
    }

    size_t diff_at(const std::string& s1, const std::string& s2) {
        size_t max_len = std::max(s1.length(), s2.length());
        for (size_t i = 0; i < max_len; ++i) {
            uint8_t b1 = (i < s1.length()) ? (uint8_t)s1[i] : 0;
            uint8_t b2 = (i < s2.length()) ? (uint8_t)s2[i] : 0;
            if (b1 != b2) {
                uint8_t xor_res = b1 ^ b2;
                int p = 7;
                while (!(xor_res & (1 << p))) p--;
                return i * 8 + (7 - p) + 1;
            }
        }
        return 0;
    }

    void deep_clear(TrieNode<T>* node) {
        if (node->l_ptr && node->l_ptr->bit_idx > node->bit_idx) deep_clear(node->l_ptr);
        if (node->r_ptr && node->r_ptr->bit_idx > node->bit_idx) deep_clear(node->r_ptr);
        delete node;
    }

   public:
    TrieMap() = default;
    ~TrieMap() {
        if (head) {
            if (head->l_ptr != head) deep_clear(head->l_ptr);
            delete head;
        }
    }

    bool add(const std::string& key, T value) {
        if (!head) {
            head = new TrieNode<T>(key, value, 0);
            head->l_ptr = head;
            return true;
        }
        TrieNode<T>*p = head, *c = head->l_ptr;
        while (c->bit_idx > p->bit_idx) {
            p = c;
            c = (fetch_bit(key, c->bit_idx) == 0) ? c->l_ptr : c->r_ptr;
        }
        if (key == c->k) return false;

        size_t n_idx = diff_at(key, c->k);
        p = head;
        c = head->l_ptr;
        while (c->bit_idx > p->bit_idx && c->bit_idx < n_idx) {
            p = c;
            c = (fetch_bit(key, c->bit_idx) == 0) ? c->l_ptr : c->r_ptr;
        }
        TrieNode<T>* leaf = new TrieNode<T>(key, value, n_idx);
        uint8_t bit = fetch_bit(key, n_idx);
        leaf->l_ptr = (bit == 0) ? leaf : c;
        leaf->r_ptr = (bit == 0) ? c : leaf;
        if (p == head)
            p->l_ptr = leaf;
        else {
            if (fetch_bit(key, p->bit_idx) == 0)
                p->l_ptr = leaf;
            else
                p->r_ptr = leaf;
        }
        return true;
    }

    T* find(const std::string& key) {
        if (!head) return nullptr;
        TrieNode<T>*p = head, *c = head->l_ptr;
        while (c->bit_idx > p->bit_idx) {
            p = c;
            c = (fetch_bit(key, c->bit_idx) == 0) ? c->l_ptr : c->r_ptr;
        }
        return (c->k == key) ? &(c->val) : nullptr;
    }

    bool del_key(const std::string& key) {
        if (!head) return false;
        TrieNode<T>*gp = nullptr, *p = head, *c = head->l_ptr;
        while (c->bit_idx > p->bit_idx) {
            gp = p;
            p = c;
            c = (fetch_bit(key, c->bit_idx) == 0) ? c->l_ptr : c->r_ptr;
        }
        if (c->k != key) return false;

        TrieNode<T>*target = c, *parent_target = p;

        if (target->bit_idx > 0 || (target == head && target->l_ptr != target)) {
            TrieNode<T>*q_p = target, *q = (fetch_bit(key, target->bit_idx) == 0) ? target->l_ptr : target->r_ptr;
            while (q->bit_idx > q_p->bit_idx) {
                q_p = q;
                q = (fetch_bit(key, q->bit_idx) == 0) ? q->l_ptr : q->r_ptr;
            }

            if (q != target) {
                target->k = q->k;
                target->val = q->val;

                TrieNode<T>*self_p = q, *self = (fetch_bit(q->k, q->bit_idx) == 0) ? q->l_ptr : q->r_ptr;
                while (self->bit_idx > self_p->bit_idx) {
                    self_p = self;
                    self = (fetch_bit(q->k, self->bit_idx) == 0) ? self->l_ptr : self->r_ptr;
                }

                TrieNode<T>* child = (q->l_ptr == q) ? q->r_ptr : q->l_ptr;
                TrieNode<T>* q_parent = q_p;
                if (fetch_bit(q->k, q_parent->bit_idx) == 0)
                    q_parent->l_ptr = child;
                else
                    q_parent->r_ptr = child;

                if (self == q) {
                    if (fetch_bit(q->k, self_p->bit_idx) == 0)
                        self_p->l_ptr = target;
                    else
                        self_p->r_ptr = target;
                }
                delete q;
                return true;
            }
        }

        TrieNode<T>* child = (target->l_ptr == target) ? target->r_ptr : target->l_ptr;
        if (parent_target == head)
            head->l_ptr = (child == head) ? head : child;
        else {
            if (fetch_bit(key, parent_target->bit_idx) == 0)
                parent_target->l_ptr = child;
            else
                parent_target->r_ptr = child;
        }
        if (target == head) head = nullptr;
        delete target;
        return true;
    }
};

void normalize(std::string& s) {
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] >= 'A' && s[i] <= 'Z') s[i] |= 32;
    }
}

struct Command {
    char type;
    std::string key;
    uint64_t val;
};

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    std::vector<Command> history;
    std::string cmd;

    while (std::cin >> cmd) {
        if (cmd == "+") {
            std::string k;
            uint64_t v;
            std::cin >> k >> v;
            normalize(k);
            history.push_back({'+', std::move(k), v});
        } else if (cmd == "-") {
            std::string k;
            std::cin >> k;
            normalize(k);
            history.push_back({'-', std::move(k), 0});
        } else if (cmd == "!") {
            std::string sub, path;
            std::cin >> sub >> path;
        } else {
            normalize(cmd);
            history.push_back({'s', std::move(cmd), 0});
        }
    }

    TrieMap<uint64_t> patricia;
    std::chrono::time_point<std::chrono::high_resolution_clock> startPat = std::chrono::high_resolution_clock::now();
    for (const Command& c : history) {
        if (c.type == '+')
            patricia.add(c.key, c.val);
        else if (c.type == '-')
            patricia.del_key(c.key);
        else
            patricia.find(c.key);
    }
    std::chrono::time_point<std::chrono::high_resolution_clock> endPat = std::chrono::high_resolution_clock::now();

    std::map<std::string, uint64_t> stl_map;
    std::chrono::time_point<std::chrono::high_resolution_clock> startStl = std::chrono::high_resolution_clock::now();
    for (const Command& c : history) {
        if (c.type == '+')
            stl_map[c.key] = c.val;
        else if (c.type == '-')
            stl_map.erase(c.key);
        else
            stl_map.find(c.key);
    }
    std::chrono::time_point<std::chrono::high_resolution_clock> endStl = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> durPat = endPat - startPat;
    std::chrono::duration<double, std::milli> durStl = endStl - startStl;

    std::cerr << std::fixed << std::setprecision(3);
    std::cerr << "Patricia Tree: " << durPat.count() << " ms\n";
    std::cerr << "STL std::map:  " << durStl.count() << " ms\n";

    return 0;
}