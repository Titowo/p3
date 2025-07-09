#include "BuscadorConCache.h"
#include <algorithm>
#include <iostream>

// CONSTRUCTOR inicialioza el buscador y el tamanio del cache
BuscadorConCache::BuscadorConCache(InvertedIndex* index, ProcesadorDocumentos* docProcessor, int cacheSize)
    : Buscador(index, docProcessor), cache(cacheSize) {}

// crea una clave unica para la cache a partir de los terminos de la consulta
// ordena los terminos y los une con "_"
std::string BuscadorConCache::crearLlaveCache(const std::vector<std::string>& terminos) const {
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

// consulta usando la cache LRU
LinkedList<int>* BuscadorConCache::queryConCache(const std::string& queryString) const {
    // procesa para obtener los terminos limpios
    std::vector<std::string> terminosQuery = procesarQueryString(queryString);

    if (terminosQuery.empty()) {
        std::cout << "No se encontraron terminos validos para la consulta" << std::endl;
        return new LinkedList<int>();
    }

    // crea la clave de cache para consultar
    std::string cacheKey = crearLlaveCache(terminosQuery);

    // busca en la cache 
    LinkedList<int>* cachedResult = cache.get(cacheKey);
    if (cachedResult) {
        std::cout << "Resultado obtenido desde cache (HIT)" << std::endl;
        LinkedList<int>* resultCopy = new LinkedList<int>();
        Node<int>* current = cachedResult->getHead();
        while (current != nullptr) {
            resultCopy->add(current->data);
            current = current->next;
        }
        return resultCopy;
    }

    // si no esta en la cache hace la consulta normal
    LinkedList<int>* result = this->query(queryString);


    // si existe el resultado lo guarda en la cache
    if (result && result->getSize() > 0) {
        LinkedList<int>* resultForCache = new LinkedList<int>();
        Node<int>* current = result->getHead();
        while (current != nullptr) {
            resultForCache->add(current->data);
            current = current->next;
        }
        cache.put(cacheKey, resultForCache);
    }

    return result;
}

void BuscadorConCache::printCacheState() const {
    cache.printCacheState();
}

void BuscadorConCache::printCacheMetrics() const {
    std::cout << "\n=== METRICAS DE CACHE ===" << std::endl;
    std::cout << "Total de consultas procesadas: " << cache.getTotalQueries() << std::endl;
    std::cout << "Total de aciertos (hits): " << cache.getHits() << std::endl;
    std::cout << "Total de fallos (misses): " << cache.getMisses() << std::endl;
    std::cout << "Tasa de aciertos: " << (cache.getHitRate() * 100) << "%" << std::endl;
    std::cout << "Tasa de fallos: " << (cache.getMissRate() * 100) << "%" << std::endl;
    std::cout << "Numero de reemplazos: " << cache.getReplacements() << std::endl;
    std::cout << "Numero de inserciones en cache: " << cache.getInsertions() << std::endl;
    std::cout << "Numero actual de elementos en cache: " << cache.getCurrentSize() << std::endl;
    std::cout << "=========================" << std::endl;
}