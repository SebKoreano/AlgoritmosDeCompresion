#include <iostream>
#include <string>
#include <queue>
#include <unordered_map>
#include <vector>
using namespace std;

// —————— Estructuras y tipos auxiliares ——————

struct Node {
    string symbols;
    int    freq;
    Node*  left;
    Node*  right;
    Node(string s, int f, Node* l = nullptr, Node* r = nullptr)
      : symbols(s), freq(f), left(l), right(r) {}
};

struct Compare {
    bool operator()(Node* a, Node* b) {
        // Nodo de menor frecuencia tiene mayor prioridad
        return a->freq > b->freq;
    }
};

// —————— Paso 1: Conteo de frecuencias ——————

unordered_map<char,int> countFrequencies(const string& text) {
    unordered_map<char,int> freq;
    for (char c : text) freq[c]++;
    return freq;
}

void printFrequencies(const unordered_map<char,int>& freq) {
    cout << "1) Frecuencias:\n";
    for (auto& kv : freq) {
        if (kv.first == ' ')
            cout << "' ' : " << kv.second << "\n";
        else
            cout << kv.first << " : " << kv.second << "\n";
    }
    cout << "\n";
}

// —————— Paso 2: Inicializar cola de prioridad ——————

priority_queue<Node*, vector<Node*>, Compare>
buildPriorityQueue(const unordered_map<char,int>& freq) {
    priority_queue<Node*, vector<Node*>, Compare> pq;
    for (auto& kv : freq) {
        pq.push(new Node(string(1, kv.first), kv.second));
    }
    return pq;  // mueve (move-semantics)
}

// —————— Paso 3: Construir árbol de Huffman ——————

Node* buildHuffmanTree(priority_queue<Node*, vector<Node*>, Compare>& pq) {
    cout << "2) Construccion del arbol:\n";
    int step = 1;
    while (pq.size() > 1) {
        Node* a = pq.top(); pq.pop();
        Node* b = pq.top(); pq.pop();
        cout << " Paso " << step++
             << ": unir [" << a->symbols << ":" << a->freq << "] + "
             << "[" << b->symbols << ":" << b->freq << "] = "
             << "[" << (a->symbols + b->symbols)
             << ":" << (a->freq + b->freq) << "]\n";
        pq.push(new Node(a->symbols + b->symbols,
                         a->freq + b->freq,
                         a, b));
    }
    cout << "\n";
    return pq.top();
}

// —————— Paso 4: Generar los códigos ——————

void buildCodes(Node* node, const string& prefix, unordered_map<char,string>& outCodes) {
    if (!node) return;
    if (!node->left && !node->right) {
        outCodes[node->symbols[0]] = prefix;
        return;
    }
    buildCodes(node->left,  prefix + "0", outCodes);
    buildCodes(node->right, prefix + "1", outCodes);
}

unordered_map<char,string> generateCodes(Node* root) {
    cout << "3) Asignacion de codigos:\n";
    unordered_map<char,string> codes;
    buildCodes(root, "", codes);
    for (auto& kv : codes) {  //kv es un par clave-valor
        if (kv.first == ' ')
            cout << "' ' : " << kv.second << "\n";
        else
            cout << kv.first << " : " << kv.second << "\n";
    }
    cout << "\n";
    return codes;
}

// —————— Paso 5: Codificar el texto ——————

string encodeText(const string& text, const unordered_map<char,string>& codes) {
    cout << "4) Codificacion del texto:\n";
    string result;
    for (char c : text) {
        result += codes.at(c) + " ";
    }
    cout << result << "\n\n";
    return result;
}

// —————— Paso 6: Estadísticas ——————

void printStatistics(const string& text,
                     const unordered_map<char,string>& codes) {
    int originalBits   = text.size() * 8;
    int compressedBits = 0;
    for (char c : text) compressedBits += codes.at(c).size();

    cout << "Resumen:\n";
    cout << " - Bits original   : " << originalBits   << "\n";
    cout << " - Bits comprimido : " << compressedBits << "\n";
    cout << " - Ratio compresión: "
         << (100.0 * compressedBits / originalBits) << " %\n";
}

// —————— Función auxiliar que orquesta todo ——————

void runHuffman(const string& text) {
    auto freq = countFrequencies(text);
    printFrequencies(freq);

    auto pq   = buildPriorityQueue(freq);
    Node* root = buildHuffmanTree(pq);

    auto codes = generateCodes(root);
    encodeText(text, codes);
    printStatistics(text, codes);
}

int main() {
    const string texto = "pablito clavo un clavito";
    runHuffman(texto);
    return 0;
}
