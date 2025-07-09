#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <string>
#include <vector>

template<typename K, typename V>
struct HashEntry {
    K key;
    V value;
    bool ocupado;
    bool borrado;

    HashEntry();
    HashEntry(const K& k, const V& v);
};

template<typename K, typename V>
class HashTable {
private:
    std::vector<HashEntry<K, V>> table;
    int tableSize;
    int actualSize;

    int hash(const std::string& key) const;
    void rehash();

public:
    HashTable(int size = 53);

    void insert(const K& key, const V& value);
    V* search(const K& key);
    bool remove(const K& key);

    int size() const;
    bool empty() const;
};

#include "./HashTable.tpp"

#endif