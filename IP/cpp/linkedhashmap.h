#include <unordered_map>
#include <list>
#include <utility>

namespace hhprolog {
    using namespace std;

    template <class K, class V>
    class LinkedHashMap {
    private:
        typedef typename list<K>::iterator list_iterator;
        unordered_map< K, pair<V, list_iterator > > hash;
        list<K> ls;
    public:
        int size() { return hash.size(); }
        bool empty() { return hash.empty(); }
        void insert(pair<K, V> kv) {
            if (hash.count(kv.first) == 1) {
                auto p = hash[kv.first];
                hash[kv.first] = make_pair(kv.second, p.second);
            }
            else {
                ls.push_back(kv.first);
                auto it = ls.end(); --it;
                hash.insert(make_pair(kv.first, make_pair(kv.second, it)));
            }
        }
        void erase(K key) {
            if (hash.count(key) == 1) {
                auto p = hash[key];
                hash.erase(key);
                ls.erase(p.second);
            }
        }
        void eraseEldest() {
            if (!hash.empty()) {
                K key = ls.front();
                ls.pop_front();
                hash.erase(key);
            }
        }
        void eraseNewest() {
            if (!hash.empty()) {
                K key = ls.back();
                ls.pop_back();
                hash.erase(key);
            }
        }
        V at(K key) {
            auto p = hash.at(key);
            return p.first;
        }
        V operator[](K key) {
            auto p = hash[key];
            return p.first;
        }
        list<K>& keyList() {
            return ls;
        }
    };

} // end namespace