#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;

// Token de salida LZ78: (indice_en_diccionario, caracter_siguiente)
struct LZ78Token {
    int  index;
    char nextChar;
};

// Busca la mayor cadena del diccionario que coincida con el prefijo de text en pos.
// Devuelve (indice_en_diccionario, longitud_de_la_coincidencia).
pair<int,int> findLongestMatch(const string& text, int pos, const unordered_map<string,int>& dictMap) {
    int bestIndex = 0;
    int bestLen   = 0;
    int n         = text.size();
    int maxLen    = n - pos;
    for (int len = maxLen; len > 0; --len) {
        string pattern = text.substr(pos, len);
        auto it = dictMap.find(pattern);
        if (it != dictMap.end()) {
            bestIndex = it->second;
            bestLen   = len;
            break;
        }
    }
    return { bestIndex, bestLen };
}

// Imprime un paso del algoritmo LZ78, mostrando el estado del diccionario, el buffer de look-ahead,
void printStep(int step, int pos, int n, const string& lookAheadBuffer, const vector<string>& dict, int matchIndex, int matchLen, char nextChar, int nextDictIndex) {
    cout << "Paso " << step << ":\n";
    cout << " Diccionario actual:\n";
    for (int i = 1; i < dict.size(); ++i) {
        cout << "   [" << i << "] \"" << dict[i] << "\"\n";
    }
    cout << " Look-ahead buffer [pos " << pos << "-" << (n - 1)
         << "]: \"" << lookAheadBuffer << "\"\n";
    string w = (matchLen > 0 ? dict[matchIndex] : string(""));
    cout << " w encontrada: \"" << w << "\" (indice " << matchIndex << ")\n";
    cout << " Caracter siguiente: '";
    if (nextChar != '\0') cout << nextChar;
    else                   cout << "\\0";
    cout << "'\n";
    cout << " Emitir token (" << matchIndex << ", '";
    if (nextChar != '\0') cout << nextChar;
    else                   cout << "\\0";
    cout << "')\n";
    cout << " Anadir al diccionario [" << nextDictIndex
         << "] = \"" << w << nextChar << "\"\n\n";
}

// Ejecuta LZ78 sobre text, devuelve el vector de tokens y construye el diccionario
vector<LZ78Token> runLZ78(const string& text, vector<string>& outDict) {
    int n = text.size();
    outDict.clear();
    outDict.push_back("");                    // dict[0] = ""
    unordered_map<string,int> dictMap;
    vector<LZ78Token> tokens;
    
    int nextDictIndex = 1;
    int pos           = 0;
    int step          = 1;

    cout << "Texto: \"" << text << "\"\n\n";
    while (pos < n) {
        auto [matchIndex, matchLen] = findLongestMatch(text, pos, dictMap);
        char nextChar = (pos + matchLen < n) ? text[pos + matchLen] : '\0'; 
        tokens.push_back({ matchIndex, nextChar });

        // Crear nueva entrada en el diccionario
        string newEntry = text.substr(pos, matchLen) + nextChar;
        outDict.push_back(newEntry);
        dictMap[newEntry] = nextDictIndex;

        string lookAheadBuffer = text.substr(pos);
        printStep(step++, pos, n, lookAheadBuffer, outDict, matchIndex, matchLen, nextChar, nextDictIndex);

        nextDictIndex++;
        pos += matchLen + 1;
    }
    return tokens;
}

// Imprime la lista de tokens resultantes de la compresión LZ78
void printTokens(const vector<LZ78Token>& tokens) {
    cout << "Tokens resultantes:\n";
    for (int i = 0; i < tokens.size(); ++i) {
        const auto& t = tokens[i];
        cout << " [" << i << "] (" << t.index << ", '";
        if (t.nextChar != '\0') cout << t.nextChar;
        else                     cout << "\\0";
        cout << "')\n";
    }
    cout << "\n";
}

// Descomprime la lista de tokens LZ78 usando el diccionario construido
string decompressLZ78(const vector<LZ78Token>& tokens) {
    vector<string> dict;
    dict.push_back("");  // dict[0] = ""
    string result;
    for (const auto& t : tokens) {
        // Reconstruir la entrada: cadena existente + nextChar si no es nulo
        string entry = dict[t.index];
        if (t.nextChar != '\0') {
            entry.push_back(t.nextChar);
        }
        // Añadir al resultado
        result += entry;
        // Añadir la nueva entrada al diccionario
        dict.push_back(entry);
    }
    return result;
}

int main() {
    const string text = "pablito clavo un clavito";

    // 1) Compresion
    vector<string> dict;
    auto tokens = runLZ78(text, dict);
    printTokens(tokens);

    // 2) Descompresion
    string recovered = decompressLZ78(tokens);
    cout << "Texto descomprimido: \"" << recovered << "\"\n";

    return 0;
}
