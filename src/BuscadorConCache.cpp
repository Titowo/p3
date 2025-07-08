#ifndef BUSCADOR_CON_CACHE_H
#define BUSCADOR_CON_CACHE_H

#include "Buscador.h"
#include "LRUCache.h"
#include <algorithm>

class BuscadorConCache : public Buscador {
private:
    mutable LRUCache cache;
    
    std::string createCacheKey(const std::vector<std::string>& terminos) const {
        std::vector<std::string> sortedTerms = terminos;
        std::sort(sortedTerms.begin(), sortedTerms.end());
        
        std::string key = "";
        for (size_t i = 0; i < sortedTerms.size(); ++i) {
            key += sortedTerms[i];
            if (i < sortedTerms.size() - 1) {
                key += "_";
            }
        }
        return key;
    }
    
public:
    BuscadorConCache(InvertedIndex* index, ProcesadorDocumentos* docProcessor, int cacheSize = 20)
        : Buscador(index, docProcessor), cache(cacheSize) {}
    
    LinkedList<int>* queryWithCache(const std::string& queryString) const {
        std::vector<std::string> terminosQuery = procesarQueryString(queryString);
        
        if (terminosQuery.empty()) {
            std::cout << "No se encontraron terminos validos para la consulta" << std::endl;
            return new LinkedList<int>();
        }
        
        std::string cacheKey = createCacheKey(terminosQuery);
        
        // Buscar en caché
        LinkedList<int>* cachedResult = cache.get(cacheKey);
        if (cachedResult) {
            std::cout << "Resultado obtenido desde caché (HIT)" << std::endl;
            
            // Crear copia del resultado
            LinkedList<int>* resultCopy = new LinkedList<int>();
            Node<int>* current = cachedResult->getHead();
            while (current != nullptr) {
                resultCopy->add(current->data);
                current = current->next;
            }
            return resultCopy;
        }
        
        // Cache miss - buscar en índice principal
        std::cout << "Reevaluando indice principal" << std::endl;
        std::cout << "Resultado no encontrado en caché (MISS)" << std::endl;
        
        // Usar método original del buscador
        LinkedList<int>* result = this->query(queryString);
        
        if (result && result->getSize() > 0) {
            // Crear copia para almacenar en caché
            LinkedList<int>* resultForCache = new LinkedList<int>();
            Node<int>* current = result->getHead();
            while (current != nullptr) {
                resultForCache->add(current->data);
                current = current->next;
            }
            
            // Almacenar en caché
            cache.put(cacheKey, resultForCache);
        }
        
        return result;
    }
    
    void printCacheState() const {
        cache.printCacheState();
    }
    
    void printCacheMetrics() const {
        std::cout << "\n=== MÉTRICAS DE CACHÉ ===" << std::endl;
        std::cout << "Total de consultas procesadas: " << cache.getTotalQueries() << std::endl;
        std::cout << "Total de aciertos (hits): " << cache.getHits() << std::endl;
        std::cout << "Total de fallos (misses): " << cache.getMisses() << std::endl;
        std::cout << "Tasa de aciertos: " << (cache.getHitRate() * 100) << "%" << std::endl;
        std::cout << "Tasa de fallos: " << (cache.getMissRate() * 100) << "%" << std::endl;
        std::cout << "Número de reemplazos: " << cache.getReplacements() << std::endl;
        std::cout << "Número de inserciones en caché: " << cache.getInsertions() << std::endl;
        std::cout << "Número actual de elementos en caché: " << cache.getCurrentSize() << std::endl;
        std::cout << "=========================" << std::endl;
    }
};

#endif