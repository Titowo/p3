#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <vector>

#include "BuscadorConCache.h"
#include "Grafo.h"
#include "InvertedIndex.h"
#include "ProcesadorDocumentos.h"
#include "LinkedList.h"

#define STOPWORDS_FILE "data/stopwords_english.dat.txt"
#define DOCUMENT_FILE "data/gov2_pages.dat"
#define QUERY_LOGS "data/Log-Queries.dat"

#define QUERY_LOG_LIMIT 10'000
#define TOP_K_DOCUMENTOS 10
#define CACHE_SIZE 20

int main() {
    std::cout << "[MAIN] Iniciando motor de busqueda con caché LRU..." << std::endl;

    // 1) INICIAR OBJETOS
    ProcesadorDocumentos pd;
    InvertedIndex ii;
    BuscadorConCache bs(&ii, &pd, CACHE_SIZE);
    Grafo g;

    // 2) CARGAR STOPWORDS
    std::cout << "[MAIN] Cargando STOPWORDS..." << std::endl;
    pd.loadStopWords(STOPWORDS_FILE);

    // 3) CARGAR DOCUMENTOS
    std::cout << "[MAIN] Cargando y procesando documentos (" << DOCUMENT_FILE << ")..." << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();
    pd.loadAndProcessDocuments(DOCUMENT_FILE, ii);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "[MAIN] Tiempo de carga y procesamiento: " << duration.count() << " ms" << std::endl;

    // 3.5) CONSTRUCCIÓN GRAFO OFFLINE
    std::cout << "[MAIN] Construyendo Grafo de co-relevancia desde logs de consulta..." << std::endl;
    start_time = std::chrono::high_resolution_clock::now();

    std::ifstream QUERY_LOGS_PROCESADOR(QUERY_LOGS);
    if (!QUERY_LOGS_PROCESADOR.is_open()) {
        std::cerr << "[ERROR] No se pudo abrir el archivo de consultas: " << QUERY_LOGS << std::endl;
        return 1;
    }

    std::string lineaQuery;
    int queryCount = 0;
    while (std::getline(QUERY_LOGS_PROCESADOR, lineaQuery) && queryCount < QUERY_LOG_LIMIT) {
        if (lineaQuery.empty()) {
            continue;
        }

        LinkedList<int>* resultadoQuery = bs.querySinPR(lineaQuery);

        if (resultadoQuery && resultadoQuery->getSize() > 0) {
            std::vector<int> topKDocs;
            Node<int>* actual = resultadoQuery->getHead();
            int count = 0;
            while (actual != nullptr && count < TOP_K_DOCUMENTOS) {
                topKDocs.push_back(actual->data);
                actual = actual->next;
                count++;
            }

            for (size_t i = 0; i < topKDocs.size(); ++i) {
                for (size_t j = i + 1; j < topKDocs.size(); ++j) {
                    g.addVertice(topKDocs[i], topKDocs[j]);
                }
            }
        }

        if (resultadoQuery) {
            delete resultadoQuery;
            resultadoQuery = nullptr;
        }
        queryCount++;
        if (queryCount % 1000 == 0) {
            std::cout << "[MAIN] Procesadas " << queryCount << " consultas del log." << std::endl;
        }
    }
    QUERY_LOGS_PROCESADOR.close();
    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "[MAIN] Grafo construido con " << g.getNumNodes() << " nodos y " 
              << g.getNumAristas() << " aristas en " << duration.count() << " ms." << std::endl;

    // 3.9) CALCULAR PAGERANK
    std::cout << "[MAIN] Calculando PageRank..." << std::endl;
    start_time = std::chrono::high_resolution_clock::now();
    std::map<int, double> pageRankScores = g.calcularPageRank();
    end_time = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "[MAIN] PageRank calculado en " << duration.count() << " ms." << std::endl;

    bs.setPageRankScores(&pageRankScores);

    // 4) INTERFAZ DE CONSULTAS CON CACHÉ
    std::cout << "\n==== Motor de Búsqueda con Caché LRU ====" << std::endl;
    std::cout << "Tamaño de caché: " << CACHE_SIZE << " elementos" << std::endl;
    std::cout << "Política de reemplazo: LRU (Least Recently Used)" << std::endl;
    std::cout << "Ingrese consulta (o 'exit' para terminar):" << std::endl;

    while (std::getline(std::cin, lineaQuery) && lineaQuery != "exit") {
        if (lineaQuery.empty()) {
            std::cout << "Por favor, ingrese una consulta..." << std::endl;
            continue;
        }

        std::cout << "\nProcesando consulta: '" << lineaQuery << "'..." << std::endl;
        
        start_time = std::chrono::high_resolution_clock::now();
        LinkedList<int>* resultado = bs.queryWithCache(lineaQuery);
        end_time = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        if (resultado && resultado->getSize() > 0) {
            std::cout << "Documentos encontrados: " << resultado->getSize() << std::endl;
            std::cout << "Top 10 documentos: [";
            Node<int>* actual = resultado->getHead();
            int count = 0;
            while (actual != nullptr && count < 10) {
                std::cout << actual->data;
                if (actual->next != nullptr && count < 9) {
                    std::cout << ", ";
                }
                actual = actual->next;
                count++;
            }
            std::cout << "]" << std::endl;
        } else {
            std::cout << "No se encontraron documentos para la consulta." << std::endl;
        }
        
        std::cout << "Tiempo de búsqueda: " << duration.count() << " ms" << std::endl;
        
        if (resultado) {
            delete resultado;
            resultado = nullptr;
        }

        bs.printCacheState();
        
        std::cout << "\nIngrese una consulta (o 'exit' para terminar):" << std::endl;
    }

    bs.printCacheMetrics();
    
    return 0;
}