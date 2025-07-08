#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <string>
#include <vector>

template<typename K, typename V>
struct HashEntry {
    K key;
    V value;
    bool occupied;
    bool deleted;
    
    HashEntry() : occupied(false), deleted(false) {}
    HashEntry(const K& k, const V& v) : key(k), value(v), occupied(true), deleted(false) {}
};

template<typename K, typename V>
class HashTable {
private:
    std::vector<HashEntry<K, V>> table;
    int tableSize;
    int currentSize;
    
    int hash(const std::string& key) const {
        unsigned long hash = 5381;
        for (char c : key) {
            hash = ((hash << 5) + hash) + c;
        }
        return hash % tableSize;
    }
    
    void rehash() {
        std::vector<HashEntry<K, V>> oldTable = table;
        tableSize *= 2;
        table.clear();
        table.resize(tableSize);
        currentSize = 0;
        
        for (const auto& entry : oldTable) {
            if (entry.occupied && !entry.deleted) {
                insert(entry.key, entry.value);
            }
        }
    }
    
public:
    HashTable(int size = 53) : tableSize(size), currentSize(0) {
        table.resize(tableSize);
    }
    
    void insert(const K& key, const V& value) {
        if (currentSize >= tableSize * 0.7) {
            rehash();
        }
        
        int index = hash(key);
        int originalIndex = index;
        
        while (table[index].occupied && !table[index].deleted && table[index].key != key) {
            index = (index + 1) % tableSize;
            if (index == originalIndex) break;
        }
        
        if (!table[index].occupied || table[index].deleted) {
            currentSize++;
        }
        
        table[index] = HashEntry<K, V>(key, value);
    }
    
    V* search(const K& key) {
        int index = hash(key);
        int originalIndex = index;
        
        while (table[index].occupied) {
            if (!table[index].deleted && table[index].key == key) {
                return &table[index].value;
            }
            index = (index + 1) % tableSize;
            if (index == originalIndex) break;
        }
        return nullptr;
    }
    
    bool remove(const K& key) {
        int index = hash(key);
        int originalIndex = index;
        
        while (table[index].occupied) {
            if (!table[index].deleted && table[index].key == key) {
                table[index].deleted = true;
                currentSize--;
                return true;
            }
            index = (index + 1) % tableSize;
            if (index == originalIndex) break;
        }
        return false;
    }
    
    int size() const { return currentSize; }
    bool empty() const { return currentSize == 0; }
};

#endif