#include "LRUCache.h"
#include <iostream>

// CONSTRUCTOR setea los campos y punteros
LRUNode::LRUNode(const std::string& k, LinkedList<int>* v)
    : key(k), value(v), hitCount(0), prev(nullptr), next(nullptr) {}

// CONSTRUCTOR inicializa el cache,  crea los nodos dumy
LRUCache::LRUCache(int cap)
    : cache(cap), capacidad(cap), actualSize(0), totalHits(0),
      totalMisses(0), totalReemplazos(0), totalInserciones(0) {
    head = new LRUNode("", nullptr);
    tail = new LRUNode("", nullptr);
    head->next = tail;
    tail->prev = head;
}

// DESTRUCTOR libera todos los nodos y vlaores
LRUCache::~LRUCache() {
    LRUNode* current = head;
    while (current != nullptr) {
        LRUNode* next = current->next;
        if (current->value != nullptr) {
            delete current->value;
        }
        delete current;
        current = next;
    }
}

void LRUCache::addToFront(LRUNode* node) {
    node->next = head->next;
    node->prev = head;
    head->next->prev = node;
    head->next = node;
}

void LRUCache::removeNode(LRUNode* node) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

void LRUCache::moveToFront(LRUNode* node) {
    removeNode(node);
    addToFront(node);
}

LRUNode* LRUCache::removeLast() {
    LRUNode* last = tail->prev;
    removeNode(last);
    return last;
}

// busca un valor en la cache por clave
LinkedList<int>* LRUCache::get(const std::string& key) {
    LRUNode** nodePtr = cache.search(key);
    if (nodePtr && *nodePtr) {
        // hace hit
        totalHits++;
        (*nodePtr)->hitCount++;
        moveToFront(*nodePtr);
        return (*nodePtr)->value;
    }
    // hace miss
    totalMisses++;
    return nullptr;
}

// intersta o actualza el valor del cache
void LRUCache::put(const std::string& key, LinkedList<int>* value) {
    LRUNode** nodePtr = cache.search(key);
    if (nodePtr && *nodePtr) {
        // si ya existe lo borra
        delete (*nodePtr)->value;
        (*nodePtr)->value = value;
        moveToFront(*nodePtr);
    } else {
        LRUNode* newNode = new LRUNode(key, value);
        if (actualSize >= capacidad) {
            // si esta llena borra el menos reciente
            LRUNode* last = removeLast();
            cache.remove(last->key);
            delete last->value;
            delete last;
            actualSize--;
            totalReemplazos++;
        }
        // inserta el nuevo nodo al frente y en la tabla hash
        addToFront(newNode);
        cache.insert(key, newNode);
        actualSize++;
        totalInserciones++;
        trackerConsulta.push_back(key);
    }
}

void LRUCache::printCacheState() const {
    std::cout << "=== Estado de la Cache ===" << std::endl;
    std::cout << "Elementos actuales: " << actualSize << "/" << capacidad << std::endl;
    std::cout << "Hits: " << totalHits << ", Misses: " << totalMisses << std::endl;

    LRUNode* current = head->next;
    int count = 0;
    while (current != tail && count < 5) {
        std::cout << "  " << (count + 1) << ". " << current->key
                  << " (docs: " << (current->value ? current->value->getSize() : 0)
                  << ", hits: " << current->hitCount << ")" << std::endl;
        current = current->next;
        count++;
    }
    if (actualSize > 5) {
        std::cout << "  ... y " << (actualSize - 5) << " mas" << std::endl;
    }
    std::cout << "=========================" << std::endl;
}

int LRUCache::getHits() const { return totalHits; }
int LRUCache::getMisses() const { return totalMisses; }
int LRUCache::getReplacements() const { return totalReemplazos; }
int LRUCache::getInsertions() const { return totalInserciones; }
int LRUCache::getCurrentSize() const { return actualSize; }
int LRUCache::getTotalQueries() const { return totalHits + totalMisses; }

double LRUCache::getHitRate() const {
    int total = getTotalQueries();
    return total > 0 ? (double)totalHits / total : 0.0;
}

double LRUCache::getMissRate() const {
    int total = getTotalQueries();
    return total > 0 ? (double)totalMisses / total : 0.0;
}

void LRUCache::clear() {
    LRUNode* current = head->next;
    while (current != tail) {
        LRUNode* next = current->next;
        delete current->value;
        delete current;
        current = next;
    }
    head->next = tail;
    tail->prev = head;
    actualSize = 0;
    trackerConsulta.clear();
}

bool LRUCache::isEmpty() const {
    return actualSize == 0;
}

bool LRUCache::isFull() const {
    return actualSize >= capacidad;
}

void LRUCache::setCapacity(int nuevaCapacidad) {
    capacidad = nuevaCapacidad;
    while (actualSize> capacidad) {
        LRUNode* last = removeLast();
        cache.remove(last->key);
        delete last->value;
        delete last;
        actualSize--;
        totalReemplazos++;
    }
}

int LRUCache::getCapacity() const {
    return capacidad;
}