#include <algorithm>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <type_traits>

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

    ~TrieNode() = default;
};

template <Numeric T>
class TrieMap {
   private:
    TrieNode<T>* head = nullptr;

    uint8_t fetch_bit(const std::string& key, size_t bit_pos) {
        if (bit_pos == 0) {
            return 0;
        }
        size_t char_idx = (bit_pos - 1) / 8;
        if (char_idx >= key.length()) {
            return 0;
        }
        return (static_cast<uint8_t>(key[char_idx]) >> (7 - (bit_pos - 1) % 8)) & 1;
    }

    size_t diff_at(const std::string& s1, const std::string& s2) {
        size_t max_len = std::max(s1.length(), s2.length());
        for (size_t i = 0; i < max_len; ++i) {
            uint8_t b1 = (i < s1.length()) ? (uint8_t)s1[i] : 0;
            uint8_t b2 = (i < s2.length()) ? (uint8_t)s2[i] : 0;
            if (b1 != b2) {
                uint8_t xor_res = b1 ^ b2;
                int p = 7;
                while (!(xor_res & (1 << p))) {
                    p--;
                }
                return i * 8 + (7 - p) + 1;
            }
        }
        return 0;
    }

    void deep_clear(TrieNode<T>* node) {
        if (node->l_ptr && node->l_ptr->bit_idx > node->bit_idx) {
            deep_clear(node->l_ptr);
        }
        if (node->r_ptr && node->r_ptr->bit_idx > node->bit_idx) {
            deep_clear(node->r_ptr);
        }
        delete node;
    }

    void write_recursive(std::ofstream& os, TrieNode<T>* node) {
        if (!node) {
            return;
        }
        size_t sz = node->k.size();
        os.write(reinterpret_cast<const char*>(&sz), sizeof(sz));
        os.write(node->k.c_str(), sz);
        os.write(reinterpret_cast<const char*>(&node->val), sizeof(T));

        if (node->l_ptr && node->l_ptr->bit_idx > node->bit_idx) {
            write_recursive(os, node->l_ptr);
        }
        if (node->r_ptr && node->r_ptr->bit_idx > node->bit_idx) {
            write_recursive(os, node->r_ptr);
        }
    }

    void locate(const std::string& key, TrieNode<T>*& target, TrieNode<T>*& parent, TrieNode<T>*& grandparent) {
        target = head->l_ptr;
        parent = head;
        grandparent = nullptr;
        while (target->bit_idx > parent->bit_idx) {
            grandparent = parent;
            parent = target;
            if (fetch_bit(key, target->bit_idx) == 0) {
                target = target->l_ptr;
            } else {
                target = target->r_ptr;
            }
        }
    }

   public:
    TrieMap() = default;

    ~TrieMap() {
        if (!head) {
            return;
        }
        if (head->l_ptr != head) {
            deep_clear(head->l_ptr);
        }
        delete head;
    }

    bool add(const std::string& key, T value) {
        if (!head) {
            head = new TrieNode<T>(key, value, 0);
            head->l_ptr = head;
            return true;
        }

        TrieNode<T>* prev = head;
        TrieNode<T>* curr = head->l_ptr;
        while (curr->bit_idx > prev->bit_idx) {
            prev = curr;
            if (fetch_bit(key, curr->bit_idx) == 0) {
                curr = curr->l_ptr;
            } else {
                curr = curr->r_ptr;
            }
        }

        if (key == curr->k) {
            return false;
        }

        size_t new_idx = diff_at(key, curr->k);
        prev = head;
        curr = head->l_ptr;
        while (curr->bit_idx > prev->bit_idx && curr->bit_idx < new_idx) {
            prev = curr;
            if (fetch_bit(key, curr->bit_idx) == 0) {
                curr = curr->l_ptr;
            } else {
                curr = curr->r_ptr;
            }
        }

        TrieNode<T>* leaf = new TrieNode<T>(key, value, new_idx);
        uint8_t bit = fetch_bit(key, new_idx);
        if (bit == 0) {
            leaf->l_ptr = leaf;
            leaf->r_ptr = curr;
        } else {
            leaf->l_ptr = curr;
            leaf->r_ptr = leaf;
        }

        if (prev == head) {
            prev->l_ptr = leaf;
        } else {
            if (fetch_bit(key, prev->bit_idx) == 0) {
                prev->l_ptr = leaf;
            } else {
                prev->r_ptr = leaf;
            }
        }
        return true;
    }

    T* find(const std::string& key) {
        if (!head) {
            return nullptr;
        }
        TrieNode<T>* p = head;
        TrieNode<T>* c = head->l_ptr;
        while (c->bit_idx > p->bit_idx) {
            p = c;
            if (fetch_bit(key, c->bit_idx) == 0) {
                c = c->l_ptr;
            } else {
                c = c->r_ptr;
            }
        }
        if (c->k == key) {
            return &(c->val);
        } else {
            return nullptr;
        }
    }

    bool del_key(const std::string& key) {
        if (!head) {
            return false;
        }
        if (key == head->k && head->l_ptr == head) {
            delete head;
            head = nullptr;
            return true;
        }

        TrieNode<T>*target, *q, *parent_q;
        locate(key, target, q, parent_q);

        if (!target || target->k != key) {
            return false;
        }

        if (target == q && target != head) {
            TrieNode<T>* child;
            if (target->l_ptr == target) {
                child = target->r_ptr;
            } else {
                child = target->l_ptr;
            }

            if (fetch_bit(key, parent_q->bit_idx) == 0) {
                parent_q->l_ptr = child;
            } else {
                parent_q->r_ptr = child;
            }
            delete target;
            return true;
        }

        TrieNode<T>*t_dummy, *q_back, *q_back_p;
        locate(q->k, t_dummy, q_back, q_back_p);

        TrieNode<T>* q_child;
        if (q->l_ptr == target) {
            q_child = q->r_ptr;
        } else {
            q_child = q->l_ptr;
        }

        if (q_child == q) {
            q_child = target;
        }

        if (parent_q) {
            if (fetch_bit(key, parent_q->bit_idx) == 0) {
                parent_q->l_ptr = q_child;
            } else {
                parent_q->r_ptr = q_child;
            }
        } else {
            head->l_ptr = q_child;
        }

        if (q_back && q_back != q) {
            if (fetch_bit(q->k, q_back->bit_idx) == 0) {
                q_back->l_ptr = target;
            } else {
                q_back->r_ptr = target;
            }
        }

        target->val = q->val;
        target->k = q->k;
        delete q;
        return true;
    }

    int store(const std::string& path) {
        std::ofstream os(path, std::ios::binary);
        if (!os) {
            std::cerr << "ERROR: " << std::strerror(errno) << "\n";
            return -1;
        }
        if (head) {
            write_recursive(os, head->l_ptr);
        }
        return 0;
    }

    int restore(const std::string& path) {
        std::ifstream is(path, std::ios::binary);
        if (!is) {
            std::cerr << "ERROR: " << std::strerror(errno) << "\n";
            return -1;
        }
        TrieMap<T> next;
        size_t s_len;
        while (is.read(reinterpret_cast<char*>(&s_len), sizeof(s_len))) {
            std::string s_key(s_len, ' ');
            T s_val;
            if (!is.read(s_key.data(), s_len) || !is.read(reinterpret_cast<char*>(&s_val), sizeof(T))) {
                return -1;
            }
            next.add(s_key, s_val);
        }
        std::swap(head, next.head);
        return 0;
    }
};

void normalize(std::string& s) {
    size_t len = s.size();
    for (size_t i = 0; i < len; ++i) {
        if (s[i] >= 'A' && s[i] <= 'Z') {
            s[i] |= 32;
        }
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    TrieMap<uint64_t> dictionary;
    std::string cmd;

    while (std::cin >> cmd) {
        try {
            if (cmd == "+") {
                std::string k;
                uint64_t v;
                std::cin >> k >> v;
                normalize(k);
                if (dictionary.add(k, v)) {
                    std::cout << "OK\n";
                } else {
                    std::cout << "Exist\n";
                }
            } else if (cmd == "-") {
                std::string k;
                std::cin >> k;
                normalize(k);
                if (dictionary.del_key(k)) {
                    std::cout << "OK\n";
                } else {
                    std::cout << "NoSuchWord\n";
                }
            } else if (cmd == "!") {
                std::string sub, path;
                std::cin >> sub >> path;
                if (sub == "Save") {
                    if (dictionary.store(path) == 0) {
                        std::cout << "OK\n";
                    }
                } else {
                    if (dictionary.restore(path) == 0) {
                        std::cout << "OK\n";
                    }
                }
            } else {
                normalize(cmd);
                uint64_t* res = dictionary.find(cmd);
                if (res) {
                    std::cout << "OK: " << *res << "\n";
                } else {
                    std::cout << "NoSuchWord\n";
                }
            }
        } catch (const std::bad_alloc&) {
            std::cout << "ERROR: Memory failure\n";
        } catch (const std::exception& e) {
            std::cout << "ERROR: " << e.what() << "\n";
        }
    }
    return 0;
}