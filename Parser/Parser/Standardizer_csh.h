#pragma once
#include "Header.h"

class CSharpStandardizer {
public:
    // -------- настройки ----------
    string typePrefix = "T_";
    string funcPrefix = "f_";
    string varPrefix = "v_";

    //  лючевые слова C# + контекстные ключевые слова + платформенные имена,
    // которые переименовывать нельз€.
    set<string> reserved = {
        // зарезервированные ключевые слова
        "abstract","as","base","bool","break","byte","case","catch","char",
        "checked","class","const","continue","decimal","default","delegate",
        "do","double","else","enum","event","explicit","extern","false",
        "finally","fixed","float","for","foreach","goto","if","implicit",
        "in","int","interface","internal","is","lock","long","namespace",
        "new","null","object","operator","out","override","params","private",
        "protected","public","readonly","ref","return","sbyte","sealed",
        "short","sizeof","stackalloc","static","string","struct","switch",
        "this","throw","true","try","typeof","uint","ulong","unchecked",
        "unsafe","ushort","using","virtual","void","volatile","while",
        // контекстные ключевые слова
        "add","alias","and","ascending","async","await","by","descending",
        "dynamic","equals","from","get","global","group","init","into",
        "join","let","managed","nameof","not","notnull","on","or","orderby",
        "partial","record","remove","required","select","set","unmanaged",
        "value","var","when","where","with","yield",
        // платформа и частые имена
        "Main","System","Console","Math","String","Object","Array",
        "List","Dictionary","HashSet","IEnumerable","IList","IDictionary",
        "Exception","Task","Action","Func","Nullable","Tuple","Environment",
        // частые методы/свойства
        "ToString","Equals","GetHashCode","GetType","Length","Count",
        // частые атрибуты (расшир€йте при необходимости)
        "Serializable","Obsolete","Debug","Conditional","Attribute",
        "TestMethod","Fact","Theory","InlineData"
    };

    // »мена, которые считаем числами и замен€ем на NUM (Math.PI и т.п.).
    set<string> numberNames = { "pi", "PI", "M_PI" };

    // ---------- основной вход ----------
    string standardize(const string& src) {
        mapping_.clear();
        counters_.clear();

        string out;
        out.reserve(src.size());

        size_t i = 0;
        string lastKeyword;   // последнее ключевое слово Ч дл€ выбора префикса

        while (i < src.size()) {
            const char c = src[i];

            if (space(c)) {                            // пробелы Ч как есть
                out += c;
                ++i;
            }
            else if (c == '/' && i + 1 < src.size() && (src[i + 1] == '/' || src[i + 1] == '*')) {
                out += readComment(src, i);            // комментарии Ч целиком
            }
            else if (startsString(src, i)) {           // любые строки -> STR
                skipString(src, i);
                out += "STR";
                lastKeyword.clear();
            }
            else if (c == '#') {                       // #if / #region / #pragma ...
                out += readLine(src, i);
            }
            else if (isWordAt(src, i, "using") && nextNonSpace(src, i + 5) != '(') {
                out += readStatement(src, i);          // using X.Y.Z; Ч целиком (не using-блок)
                lastKeyword.clear();
            }
            else if (c == '@' && i + 1 < src.size() && (letter(src[i + 1]) || src[i + 1] == '_')) {
                ++i;                                   // @identifier: переименовываем даже ключевое слово
                const string id = readWord(src, i);
                out += '@';
                out += rename(id, lastKeyword, nextNonSpace(src, i) == '(');
                lastKeyword.clear();
            }
            else if (digit(c)) {                       // числа -> NUM
                skipNumber(src, i);
                out += "NUM";
                lastKeyword.clear();
            }
            else if (letter(c) || c == '_') {          // идентификаторы и ключевые слова
                const string id = readWord(src, i);
                if (reserved.count(id)) {
                    out += id;
                    lastKeyword = id;
                }
                else if (numberNames.count(id)) {      // PI и подобные -> NUM
                    out += "NUM";
                    lastKeyword.clear();
                }
                else {
                    out += rename(id, lastKeyword, nextNonSpace(src, i) == '(');
                    lastKeyword.clear();
                }
            }
            else {                                     // операторы, скобки, зап€тые...
                out += c;
                ++i;
                if (c == ';' || c == '=' || c == ',' || c == ')' || c == '{' || c == '}')
                    lastKeyword.clear();
            }
        }
        return out;
    }

    const map<string, string>& mapping() const { return mapping_; }

private:
    map<string, string> mapping_;   // old -> new
    map<string, int>         counters_;  // prefix -> счЄтчик

    // Ќовое им€: префикс по контексту + пор€дковый номер.
    string rename(const string& id, const string& lastKeyword, bool looksLikeFunc) {
        auto it = mapping_.find(id);
        if (it != mapping_.end())
            return it->second;                     // уже переименовывали

        string prefix = varPrefix;
        if (lastKeyword == "class" || lastKeyword == "struct" || lastKeyword == "interface" ||
            lastKeyword == "enum" || lastKeyword == "record" || lastKeyword == "namespace" ||
            lastKeyword == "delegate" || lastKeyword == "event")
            prefix = typePrefix;
        else if (looksLikeFunc)                    // дальше идЄт '(' Ч метод
            prefix = funcPrefix;

        const string newName = prefix + to_string(++counters_[prefix]);
        mapping_[id] = newName;
        return newName;
    }

    // -------- читатели токенов (сдвигают i за токен) ----------

    static string readWord(const string& s, size_t& i) {
        const size_t start = i;
        while (i < s.size() && (alnum(s[i]) || s[i] == '_')) ++i;
        return s.substr(start, i - start);
    }

    static string readLine(const string& s, size_t& i) {
        const size_t start = i;
        while (i < s.size() && s[i] != '\n') ++i;
        return s.substr(start, i - start);
    }

    // строка до ';' или конца строки (';' тоже забираем)
    static string readStatement(const string& s, size_t& i) {
        const size_t start = i;
        while (i < s.size() && s[i] != ';' && s[i] != '\n') ++i;
        if (i < s.size() && s[i] == ';') ++i;
        return s.substr(start, i - start);
    }

    static string readComment(const string& s, size_t& i) {
        const size_t start = i;
        if (s[i + 1] == '/') {                     // // ... до конца строки
            while (i < s.size() && s[i] != '\n') ++i;
        }
        else {                                     // /* ... */
            i += 2;
            while (i < s.size() && !(s[i] == '*' && i + 1 < s.size() && s[i + 1] == '/')) ++i;
            if (i < s.size()) i += 2;
        }
        return s.substr(start, i - start);
    }

    // начинаетс€ ли строковый литерал: "...", '...', @"...", $"...", $@"...", @$"..."
    static bool startsString(const string& s, size_t i) {
        const char c = s[i];
        if (c == '"' || c == '\'') return true;
        if (c != '@' && c != '$') return false;
        size_t j = i + 1;
        if (j < s.size() && (s[j] == '@' || s[j] == '$')) ++j;   // $@ или @$
        return j < s.size() && s[j] == '"';
    }

    // пропускает строку любого вида (выражени€ внутри $"..." упрощЄнно не разбираем)
    static void skipString(const string& s, size_t& i) {
        bool verbatim = false;
        while (i < s.size() && (s[i] == '$' || s[i] == '@')) {   // префиксы
            if (s[i] == '@') verbatim = true;
            ++i;
        }
        if (i >= s.size()) return;
        if (s[i] == '\'') { skipQuoted(s, i, '\'', false); return; }   // символьный литерал

        if (!verbatim && i + 2 < s.size() && s[i + 1] == '"' && s[i + 2] == '"') {   // raw-строка
            size_t q = 0;
            while (i + q < s.size() && s[i + q] == '"') ++q;     // сколько кавычек открыло
            const string close(q, '"');
            i += q;
            while (i < s.size()) {
                if (s.compare(i, q, close) == 0) { i += q; return; }
                ++i;
            }
            return;
        }
        skipQuoted(s, i, '"', verbatim);
    }

    // обычна€ строка с escape или verbatim с удвоенными кавычками
    static void skipQuoted(const string& s, size_t& i, char q, bool verbatim) {
        ++i;                                       // открывающа€ кавычка
        while (i < s.size()) {
            if (s[i] == q) {
                if (verbatim && i + 1 < s.size() && s[i + 1] == q) { i += 2; continue; }  // ""
                ++i;                               // закрывающа€
                return;
            }
            if (!verbatim && s[i] == '\\') ++i;    // escape
            ++i;
        }
    }

    // 123, 3.14, 0xFF, 1_000, 1.5e-3, 2.5m, 10UL
    static void skipNumber(const string& s, size_t& i) {
        while (i < s.size()) {
            const char c = s[i];
            if (alnum(c) || c == '.' || c == '_') { ++i; continue; }
            if ((c == '+' || c == '-') && (s[i - 1] == 'e' || s[i - 1] == 'E')) { ++i; continue; }
            break;
        }
    }

    // слово w ровно в позиции i (не часть более длинного идентификатора)
    static bool isWordAt(const string& s, size_t i, const string& w) {
        if (s.compare(i, w.size(), w) != 0) return false;
        const size_t j = i + w.size();
        return j >= s.size() || (!alnum(s[j]) && s[j] != '_');
    }

    static char nextNonSpace(const string& s, size_t i) {
        while (i < s.size() && space(s[i])) ++i;
        return i < s.size() ? s[i] : '\0';
    }

    static bool space(char c) { return isspace(static_cast<unsigned char>(c)) != 0; }
    static bool digit(char c) { return isdigit(static_cast<unsigned char>(c)) != 0; }
    static bool letter(char c) { return isalpha(static_cast<unsigned char>(c)) != 0; }
    static bool alnum(char c) { return isalnum(static_cast<unsigned char>(c)) != 0; }
};