#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include <string>
#include <vector>
#include "HashTable.h"
#include "LinkedList.h"

struct LRUNode {
    std::string key;
    LinkedList<int>* value;
    int hitCount;
    LRUNode* prev;
    LRUNode* next;

    LRUNode(const std::string& k, LinkedList<int>* v);
};

class LRUCache {
private:
    HashTable<std::string, LRUNode*> cache;
    LRUNode* head;
    LRUNode* tail;
    int capacidad;
    int actualSize;
    int totalHits;
    int totalMisses;
    int totalReemplazos;
    int totalInserciones;
    std::vector<std::string> trackerConsulta;

    void addToFront(LRUNode* node);
    void removeNode(LRUNode* node);
    void moveToFront(LRUNode* node);
    LRUNode* removeLast();

public:
    LRUCache(int cap = 20);
    ~LRUCache();

    LinkedList<int>* get(const std::string& key);
    void put(const std::string& key, LinkedList<int>* value);

    void printCacheState() const;
    int getHits() const;
    int getMisses() const;
    int getReplacements() const;
    int getInsertions() const;
    int getCurrentSize() const;
    int getTotalQueries() const;
    double getHitRate() const;
    double getMissRate() const;
    void clear();
    bool isEmpty() const;
    bool isFull() const;
    void setCapacity(int newCapacity);
    int getCapacity() const;
};

#endif // LRU_CACHE_H