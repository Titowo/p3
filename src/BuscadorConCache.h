#ifndef BUSCADOR_CON_CACHE_H
#define BUSCADOR_CON_CACHE_H

#include "Buscador.h"
#include "LRUCache.h"
#include <vector>
#include <string>

class BuscadorConCache : public Buscador {
private:
    mutable LRUCache cache;
    std::string crearLlaveCache(const std::vector<std::string>& terminos) const;

public:
    BuscadorConCache(InvertedIndex* index, ProcesadorDocumentos* docProcessor, int cacheSize = 20);

    LinkedList<int>* queryConCache(const std::string& queryString) const;
    void printCacheState() const;
    void printCacheMetrics() const;
};

#endif