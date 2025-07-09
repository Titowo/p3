// CONSTRUCTOR POR DEFECTO
template<typename K, typename V>
HashEntry<K, V>::HashEntry() : ocupado(false), borrado(false) {}

// CONSTRUCTOR CON CLAVE VALOR
template<typename K, typename V>
HashEntry<K, V>::HashEntry(const K& k, const V& v) : key(k), value(v), ocupado(true), borrado(false) {}

// CONSTRUCOTR que inicializa la tabla con el tamanio dado y poen el contador de elementos en 0
template<typename K, typename V>
HashTable<K, V>::HashTable(int size) : tableSize(size), actualSize(0) {
    table.resize(tableSize);
}

// Funcion hash para clabes string usando algoritmo djb2
template<typename K, typename V>
int HashTable<K, V>::hash(const std::string& key) const {
    unsigned long hash = 5381;
    for (char c : key) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % tableSize;
}

// rehash duplica el tamanio de la tabla y re-inserta las entradas validas
template<typename K, typename V>
void HashTable<K, V>::rehash() {
    std::vector<HashEntry<K, V>> oldTable = table;
    tableSize *= 2;
    table.clear();
    table.resize(tableSize);
    actualSize = 0;

    // auto toma cualquier valor que tenga oldtable en entry
    // reincerta todas las entradas que estan ocupadas y no eliminadas
    for (const auto& entry : oldTable) {
        if (entry.ocupado && !entry.borrado) {
            insert(entry.key, entry.value);
        }
    }
}

// interta una clave-valor a la table hash
template<typename K, typename V>
void HashTable<K, V>::insert(const K& key, const V& value) {
    // si la tabla se llena al 70 por cieot, realiza un rehash para evitar que colisione
    if (actualSize >= tableSize * 0.7) {
        rehash();
    }

    int index = hash(key);
    int originalIndex = index;

    // buasca una posicion libre o la posicion clave para que se actualize
    while (table[index].ocupado && !table[index].borrado && table[index].key != key) {
        index = (index + 1) % tableSize;
        if (index == originalIndex) break;
    }

    // si la posicion esta libre o fue elimninada incrementa el contador
    if (!table[index].ocupado || table[index].borrado) {
        actualSize++;
    }
    // inserta o actualiza la entrada
    table[index] = HashEntry<K, V>(key, value);
}

// busca la clave que se le pase y retorna un puntero al valor o nullptr si no existe
template<typename K, typename V>
V* HashTable<K, V>::search(const K& key) {
    int index = hash(key);
    int originalIndex = index;

    // busca la clave
    while (table[index].ocupado) {
        if (!table[index].borrado && table[index].key == key) {
            return &table[index].value;
        }
        index = (index + 1) % tableSize;
        if (index == originalIndex) break; // evita el bucle infinito
    }
    return nullptr;
}

// elimina la clave de la tabla marcando como eliminadad
template<typename K, typename V>
bool HashTable<K, V>::remove(const K& key) {
    int index = hash(key);
    int originalIndex = index;

    // busca la clave con el mismo algoritmo anterioor
    while (table[index].ocupado) {
        if (!table[index].borrado && table[index].key == key) {
            table[index].borrado = true; // marca como eliminada
            actualSize--;
            return true;
        }
        index = (index + 1) % tableSize;
        if (index == originalIndex) break; // evita el bucle
    }
    return false;
}

// retorna el numero de elementos activos en la tabla
template<typename K, typename V>
int HashTable<K, V>::size() const { return actualSize; }

// retorna true si la tabla esta vacia
template<typename K, typename V>
bool HashTable<K, V>::empty() const { return actualSize == 0; }