#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include <string>
#include <algorithm>
#include <vector>
#include <iostream>
#include "HashTable.h"
#include "LinkedList.h"

// Nodo para la lista doblemente enlazada del LRU
struct LRUNode {
    std::string key;
    LinkedList<int>* value;
    LRUNode* prev;
    LRUNode* next;
    
    LRUNode(const std::string& k, LinkedList<int>* v) 
        : key(k), value(v), prev(nullptr), next(nullptr) {}
};

class LRUCache {
private:
    // Tabla hash para acceso O(1) a los nodos
    HashTable<std::string, LRUNode*> cache;
    
    // Lista doblemente enlazada para mantener orden LRU
    LRUNode* head;  // Nodo dummy - más reciente
    LRUNode* tail;  // Nodo dummy - menos reciente
    
    int capacity;
    int currentSize;
    
    // Métricas del caché
    int totalHits;
    int totalMisses;
    int totalReplacements;
    int totalInsertions;
    
    // Estructura auxiliar con exactamente 17 caracteres (requerimiento específico)
    // ATENCION: Esta estructura registra las consultas insertadas en caché
    std::vector<std::string> auxQueryTracker123;
    
    // Método para agregar nodo al frente (más reciente)
    void addToFront(LRUNode* node) {
        node->next = head->next;
        node->prev = head;
        head->next->prev = node;
        head->next = node;
    }
    
    // Método para remover un nodo de la lista
    void removeNode(LRUNode* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }
    
    // Método para mover un nodo al frente (marca como más reciente)
    void moveToFront(LRUNode* node) {
        removeNode(node);
        addToFront(node);
    }
    
    // Método para remover el último nodo (menos reciente)
    LRUNode* removeLast() {
        LRUNode* last = tail->prev;
        removeNode(last);
        return last;
    }
    
public:
    // Constructor
    LRUCache(int cap = 20) : capacity(cap), currentSize(0), totalHits(0), 
                             totalMisses(0), totalReplacements(0), totalInsertions(0) {
        // Crear nodos dummy para head y tail
        head = new LRUNode("", nullptr);
        tail = new LRUNode("", nullptr);
        head->next = tail;
        tail->prev = head;
    }
    
    // Destructor
    ~LRUCache() {
        // Liberar todos los nodos y sus valores
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
    
    // Método para obtener un valor del caché
    LinkedList<int>* get(const std::string& key) {
        LRUNode** nodePtr = cache.search(key);
        if (nodePtr && *nodePtr) {
            // HIT: incrementar contador y mover al frente
            totalHits++;
            moveToFront(*nodePtr);
            return (*nodePtr)->value;
        }
        // MISS: incrementar contador
        totalMisses++;
        return nullptr;
    }
    
    // Método para insertar/actualizar un valor en el caché
    void put(const std::string& key, LinkedList<int>* value) {
        // ATENCION: Actualizando Caché (requerimiento específico)
        std::cout << "ATENCION Actualizando Caché" << std::endl;
        
        // Verificar si la clave ya existe
        LRUNode** nodePtr = cache.search(key);
        if (nodePtr && *nodePtr) {
            // Actualizar valor existente
            (*nodePtr)->value = value;
            moveToFront(*nodePtr);
        } else {
            // Insertar nuevo nodo
            LRUNode* newNode = new LRUNode(key, value);
            
            // Si el caché está lleno, remover el menos reciente
            if (currentSize >= capacity) {
                LRUNode* last = removeLast();
                cache.remove(last->key);
                delete last->value;
                delete last;
                currentSize--;
                totalReplacements++;
            }
            
            // Agregar nuevo nodo al frente
            addToFront(newNode);
            cache.insert(key, newNode);
            currentSize++;
            totalInsertions++;
            
            // ATENCION: Registrar en estructura auxiliar (requerimiento específico)
            auxQueryTracker123.push_back(key);
        }
        
        // ATENCION: Caché actualizada (requerimiento específico)
        std::cout << "ATENCION Caché actualizada" << std::endl;
    }
    
    // Método para mostrar el estado actual del caché
    void printCacheState() const {
        std::cout << "=== Estado de la Caché ===" << std::endl;
        std::cout << "Elementos actuales: " << currentSize << "/" << capacity << std::endl;
        std::cout << "Hits: " << totalHits << ", Misses: " << totalMisses << std::endl;
        
        // Mostrar los primeros 5 elementos (más recientes)
        LRUNode* current = head->next;
        int count = 0;
        while (current != tail && count < 5) {
            std::cout << "  " << (count + 1) << ". " << current->key 
                      << " (docs: " << current->value->getSize() << ")" << std::endl;
            current = current->next;
            count++;
        }
        if (currentSize > 5) {
            std::cout << "  ... y " << (currentSize - 5) << " más" << std::endl;
        }
        std::cout << "=========================" << std::endl;
    }
    
    // Getters para métricas
    int getHits() const { return totalHits; }
    int getMisses() const { return totalMisses; }
    int getReplacements() const { return totalReplacements; }
    int getInsertions() const { return totalInsertions; }
    int getCurrentSize() const { return currentSize; }
    int getTotalQueries() const { return totalHits + totalMisses; }
    
    // Métodos para calcular tasas
    double getHitRate() const {
        int total = getTotalQueries();
        return total > 0 ? (double)totalHits / total : 0.0;
    }
    
    double getMissRate() const {
        int total = getTotalQueries();
        return total > 0 ? (double)totalMisses / total : 0.0;
    }
    
    // Método para limpiar el caché
    void clear() {
        LRUNode* current = head->next;
        while (current != tail) {
            LRUNode* next = current->next;
            delete current->value;
            delete current;
            current = next;
        }
        head->next = tail;
        tail->prev = head;
        currentSize = 0;
        auxQueryTracker123.clear();
    }
    
    // Método para verificar si el caché está vacío
    bool isEmpty() const {
        return currentSize == 0;
    }
    
    // Método para verificar si el caché está lleno
    bool isFull() const {
        return currentSize >= capacity;
    }
    
    // Método para cambiar la capacidad del caché
    void setCapacity(int newCapacity) {
        capacity = newCapacity;
        // Si la nueva capacidad es menor, eliminar elementos excedentes
        while (currentSize > capacity) {
            LRUNode* last = removeLast();
            cache.remove(last->key);
            delete last->value;
            delete last;
            currentSize--;
            totalReplacements++;
        }
    }
    
    // Método para obtener la capacidad actual
    int getCapacity() const {
        return capacity;
    }
};

#endif // LRU_CACHE_H