#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

using namespace std;

// Representa el token de salida LZ77: (offset, length, nextChar)
struct LZ77Token {
    int offset;
    int length;
    char nextChar;
};

// Construye la ventana de búsqueda (search buffer) hasta 'windowSize' caracteres atrás
string getSearchBuffer(const string& text, int pos, int windowSize) {
    int start = max(0, pos - windowSize);
    return text.substr(start, pos - start);
}

// Construye la ventana de lectura (look-ahead buffer) desde la posición actual
string getLookAheadBuffer(const string& text, int pos) {
    return text.substr(pos);
}

// Busca en 'searchBuffer' la coincidencia más larga con el texto en 'pos'
LZ77Token findLongestMatch(const string& text, int pos, const string& searchBuffer) {
    int bestLength = 0;
    int bestOffset = 0;
    int searchLen  = searchBuffer.size();
    int n          = text.size();

    // Probar longitudes decrecientes para hallar la máxima coincidencia
    for (int len = searchLen; len > 0; --len) {
        if (pos + len > n) continue;
        string pattern = text.substr(pos, len);
        size_t found   = searchBuffer.rfind(pattern);
        if (found != string::npos) {
            bestLength = len;
            bestOffset = searchLen - static_cast<int>(found);
            break;
        }
    }

    // Caracter siguiente tras la coincidencia (o '\0' si ya no hay más)
    char nextChar = '\0';
    if (pos + bestLength < n) {
        nextChar = text[pos + bestLength];
    }

    return { bestOffset, bestLength, nextChar };
}

// Imprime un paso detallado de la compresión
void printStep(int step,
               int searchStart,
               int pos,
               int n,
               const string& searchBuffer,
               const string& lookAheadBuffer,
               const LZ77Token& match) {
    cout << "Paso " << step << ":\n";
    cout << " Search buffer [pos " << searchStart << "-" << (pos - 1) << "]: \""
         << searchBuffer << "\"\n";
    cout << " Lookahead buffer [pos " << pos << "-" << (n - 1) << "]: \""
         << lookAheadBuffer << "\"\n";
    cout << " Match encontrado: (offset=" << match.offset
         << ", length=" << match.length
         << ", next='";
    if (match.nextChar != '\0') cout << match.nextChar;
    else                         cout << "\\0";
    cout << "')\n\n";
}

// Orquesta todo el proceso LZ77 y devuelve el vector de tokens
vector<LZ77Token> runLZ77(const string& text, int windowSize) {
    int n = (int)text.size();
    cout << "Texto: \"" << text << "\"\n";
    cout << "Ventana (search buffer) máxima: " << windowSize << " caracteres\n\n";
    cout << "Pasos de compresión LZ77:\n\n";

    vector<LZ77Token> tokens;
    int i = 0, step = 1;
    while (i < n) {
        int searchStart = max(0, i - windowSize);
        string searchBuffer    = getSearchBuffer(text, i, windowSize);
        string lookAheadBuffer = getLookAheadBuffer(text, i);
        LZ77Token match        = findLongestMatch(text, i, searchBuffer);

        printStep(step++, searchStart, i, n, searchBuffer, lookAheadBuffer, match);

        tokens.push_back(match);
        i += match.length + 1;
    }
    return tokens;
}

// Imprime la lista de tokens comprimidos al final
void printCompressedTokens(const vector<LZ77Token>& tokens) {
    cout << "Tokens de compresion resultantes:\n";
    for (size_t k = 0; k < tokens.size(); ++k) {
        const auto& t = tokens[k];
        cout << "  [" << k << "] (offset=" << t.offset
             << ", length=" << t.length
             << ", next='";
        if (t.nextChar != '\0') cout << t.nextChar;
        else                     cout << "\\0";
        cout << "')\n";
    }
}

int main() {
    const string text       = "pablito clavo un clavito";
    const int    windowSize = 5;

    auto compressed = runLZ77(text, windowSize);
    printCompressedTokens(compressed);

    return 0;
}
