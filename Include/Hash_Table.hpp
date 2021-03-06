#ifndef EXERCISE_II_HASH_TABLE_HPP
#define EXERCISE_II_HASH_TABLE_HPP

#include <functional>
#include "Array.hpp"
#include "List.hpp"

namespace Types {
    template<typename K, typename V, typename H = std::hash<K>, typename E = std::equal_to<K>>
    class Hash_Table {
    private:
        using Self = Hash_Table<K, V, H, E>;
    public:
        Hash_Table() = default;

        Hash_Table(const Self &other) = default;

        Hash_Table(Self &&other) noexcept = default;

        Hash_Table &operator=(const Self &other) = default;

        Hash_Table &operator=(Self &&other) noexcept = default;

        Hash_Table(size_t buckets);

        ~Hash_Table() = default;

        size_t Buckets() const;

        const List<std::pair<K, V>> &ListAt(size_t bucket) const;

        List<std::pair<K, V>> &ListAt(size_t bucket);

        bool Contains(const K &key);

        V &Insert(const K &key, const V &value);

        V &Insert(K &&key, V &&value);

        void Remove(const K &key);

        V &operator[](const K &key);

        V &operator[](K &&key);

    private:
        V *Find(const K &key);

        H hasher_;
        E comparer_;
        Array<List<std::pair<K, V>>> table_;
    };

    template<typename K, typename V, typename H, typename E>
    Hash_Table<K, V, H, E>::Hash_Table(size_t buckets)
            : table_{buckets} {}

    template<typename K, typename V, typename H, typename E>
    size_t Hash_Table<K, V, H, E>::Buckets() const {
        return table_.Size();
    }

    template<typename K, typename V, typename H, typename E>
    List<std::pair<K, V>> &Hash_Table<K, V, H, E>::ListAt(size_t bucket) {
        return table_[bucket];
    }

    template<typename K, typename V, typename H, typename E>
    const List<std::pair<K, V>> &Hash_Table<K, V, H, E>::ListAt(size_t bucket) const {
        return table_[bucket];
    }

    template<typename K, typename V, typename H, typename E>
    bool Hash_Table<K, V, H, E>::Contains(const K &key) {
        return Find(key) != nullptr;
    }

    template<typename K, typename V, typename H, typename E>
    V &Hash_Table<K, V, H, E>::Insert(const K &key, const V &value) {
        size_t bucket = hasher_(key) % table_.Size();
        auto &bucket_list = table_[bucket];
        auto pair = std::make_pair(key, value);
        bucket_list.Add_Back(pair);
        return bucket_list.Last().second;
    }

    template<typename K, typename V, typename H, typename E>
    V &Hash_Table<K, V, H, E>::Insert(K &&key, V &&value) {
        size_t bucket = hasher_(key) % table_.Size();
        auto &bucket_list = table_[bucket];
        auto pair = std::make_pair(key, value);
        bucket_list.Add_Back(pair);
        return bucket_list.Last().second;
    }

    template<typename K, typename V, typename H, typename E>
    V &Hash_Table<K, V, H, E>::operator[](const K &key) {
        V *value_ = Find(key);
        return value_ != nullptr ? *value_ : Insert(key, V{});
    }

    template<typename K, typename V, typename H, typename E>
    V &Hash_Table<K, V, H, E>::operator[](K &&key) {
        V *value_ = Find(key);
        return value_ != nullptr ? *value_ : Insert(key, V{});
    }

    template<typename K, typename V, typename H, typename E>
    V *Hash_Table<K, V, H, E>::Find(const K &key) {
        size_t bucket = hasher_(key) % table_.Size();
        auto &bucket_list = table_[bucket];
        for (auto it = bucket_list.Begin(); it != bucket_list.End(); ++it) {
            if (comparer_((*it).first, key)) return &(*it).second;
        }
        return nullptr;
    }

    template<typename K, typename V, typename H, typename E>
    void Hash_Table<K, V, H, E>::Remove(const K &key) {
        size_t bucket = hasher_(key) % table_.Size();
        auto &bucket_list = table_[bucket];
        for (auto it = bucket_list.Begin(); it != bucket_list.End(); ++it) {
            if (comparer_((*it).first, key)) {
                bucket_list.Remove(it);
                break;
            }
        }
    }
}

#endif //EXERCISE_II_HASH_TABLE_HPP
