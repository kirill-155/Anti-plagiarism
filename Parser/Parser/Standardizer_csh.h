#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <cctype>

class CSharpStandardizer {
public:
    std::string typePrefix = "T_";
    std::string funcPrefix = "f_";
    std::string varPrefix = "v_";

    //  лючевые слова C# + контекстные ключевые слова + платформенные имена,
    // которые переименовывать нельз€.
    std::unordered_set<std::string> reserved = {
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

    // ------------------------------------------------------------------
    std::string standardize(const std::string& src) {
        mapping_.clear();
        counters_.clear();

        std::string out;
        out.reserve(src.size());

        size_t i = 0, n = src.size();
        std::string lastKeyword;

        while (i < n) {
            const char c = src[i];

            // пробелы как есть
            if (std::isspace(static_cast<unsigned char>(c))) { out += src[i++]; continue; }

            // // комментарий
            if (c == '/' && i + 1 < n && src[i + 1] == '/') {
                while (i < n && src[i] != '\n') out += src[i++];
                continue;
            }
            // /* ... */
            if (c == '/' && i + 1 < n && src[i + 1] == '*') {
                out += src[i++]; out += src[i++];
                while (i < n) {
                    if (src[i] == '*' && i + 1 < n && src[i + 1] == '/') {
                        out += src[i++]; out += src[i++]; break;
                    }
                    out += src[i++];
                }
                continue;
            }

            // ---------- строки ----------
            // raw string:  """..."""
            if (c == '"' && i + 2 < n && src[i + 1] == '"' && src[i + 2] == '"') {
                size_t q = 0;
                while (i + q < n && src[i + q] == '"') ++q;
                out.append(src, i, q); i += q;
                std::string close(q, '"');
                while (i < n) {
                    if (src.compare(i, q, close) == 0) {
                        out.append(src, i, q); i += q; break;
                    }
                    out += src[i++];
                }
                continue;
            }

            // @"..." (verbatim) либо @identifier
            if (c == '@' && i + 1 < n) {
                if (src[i + 1] == '"') {
                    out += src[i++]; out += src[i++];
                    while (i < n) {
                        if (src[i] == '"') {
                            if (i + 1 < n && src[i + 1] == '"') { out += src[i++]; out += src[i++]; }
                            else { out += src[i++]; break; }
                        }
                        else out += src[i++];
                    }
                    continue;
                }
                if (std::isalpha(static_cast<unsigned char>(src[i + 1])) || src[i + 1] == '_') {
                    out += src[i++]; // '@'
                    const size_t start = i;
                    while (i < n && (std::isalnum(static_cast<unsigned char>(src[i])) || src[i] == '_'))
                        ++i;
                    const std::string id = src.substr(start, i - start);
                    // force-rename: @identifier может совпадать с ключевым словом
                    out += renamed(id, lastKeyword, src, i);
                    lastKeyword.clear();
                    continue;
                }
                out += src[i++]; continue;
            }

            // $"..." и $@"..." Ч упрощЄнно пропускаем целиком
            if (c == '$' && i + 1 < n) {
                size_t j = i + 1; bool verbatim = false;
                if (src[j] == '@') { verbatim = true; ++j; }
                if (j < n && src[j] == '"') {
                    out.append(src, i, j - i + 1);
                    i = j + 1;
                    while (i < n) {
                        if (!verbatim && src[i] == '\\' && i + 1 < n) { out += src[i++]; out += src[i++]; }
                        else if (verbatim && src[i] == '"' && i + 1 < n && src[i + 1] == '"') { out += src[i++]; out += src[i++]; }
                        else if (src[i] == '"') { out += src[i++]; break; }
                        else out += src[i++];
                    }
                    continue;
                }
            }

            // обычна€ строка
            if (c == '"') {
                out += src[i++];
                while (i < n) {
                    if (src[i] == '\\' && i + 1 < n) { out += src[i++]; out += src[i++]; }
                    else if (src[i] == '"') { out += src[i++]; break; }
                    else out += src[i++];
                }
                continue;
            }

            // символьный литерал
            if (c == '\'') {
                out += src[i++];
                while (i < n) {
                    if (src[i] == '\\' && i + 1 < n) { out += src[i++]; out += src[i++]; }
                    else if (src[i] == '\'') { out += src[i++]; break; }
                    else out += src[i++];
                }
                continue;
            }

            // препроцессор #if / #region / #pragma / #define ...
            if (c == '#') {
                while (i < n && src[i] != '\n') out += src[i++];
                continue;
            }

            // using-директива: using X.Y.Z;   (но не using (...) и не using X = ...;)
            if (c == 'u' && i + 5 <= n && src.compare(i, 5, "using") == 0 &&
                (i + 5 == n || (!std::isalnum(static_cast<unsigned char>(src[i + 5])) && src[i + 5] != '_'))) {
                size_t j = i + 5;
                while (j < n && std::isspace(static_cast<unsigned char>(src[j]))) ++j;
                if (j < n && src[j] != '(') {
                    while (i < n && src[i] != ';' && src[i] != '\n') out += src[i++];
                    if (i < n && src[i] == ';') out += src[i++];
                    lastKeyword.clear();
                    continue;
                }
            }

            // ---------- идентификаторы ----------
            if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
                const size_t start = i;
                while (i < n && (std::isalnum(static_cast<unsigned char>(src[i])) || src[i] == '_'))
                    ++i;
                const std::string id = src.substr(start, i - start);

                if (reserved.count(id)) {
                    out += id;
                    lastKeyword = id;
                }
                else {
                    out += renamed(id, lastKeyword, src, i);
                    lastKeyword.clear();
                }
                continue;
            }

            // числа (упрощЄнно)
            if (std::isdigit(static_cast<unsigned char>(c))) {
                while (i < n && (std::isalnum(static_cast<unsigned char>(src[i])) ||
                    src[i] == '.' || src[i] == '_' ||
                    ((src[i] == '+' || src[i] == '-') && i > 0 &&
                        (src[i - 1] == 'e' || src[i - 1] == 'E'))))
                    out += src[i++];
                lastKeyword.clear();
                continue;
            }

            // прочие символы (операторы, скобки, зап€тые, точки)
            const char p = src[i++];
            out += p;
            if (p == ';' || p == '=' || p == ',' || p == ')' || p == '{' || p == '}')
                lastKeyword.clear();
        }
        return out;
    }

    const std::unordered_map<std::string, std::string>& mapping() const { return mapping_; }

private:
    std::unordered_map<std::string, std::string> mapping_;   // old -> new
    std::unordered_map<std::string, int>         counters_;  // prefix -> counter

    std::string renamed(const std::string& id, const std::string& lastKeyword,
        const std::string& src, size_t posAfterId) {
        auto it = mapping_.find(id);
        if (it != mapping_.end()) return it->second;

        const std::string prefix = choosePrefix(lastKeyword, src, posAfterId);
        const std::string newName = prefix + std::to_string(++counters_[prefix]);
        mapping_.emplace(id, newName);
        return newName;
    }

    std::string choosePrefix(const std::string& lastKeyword,
        const std::string& src, size_t posAfterId) const {
        if (lastKeyword == "class" || lastKeyword == "struct" ||
            lastKeyword == "interface" || lastKeyword == "enum" ||
            lastKeyword == "record" || lastKeyword == "namespace" ||
            lastKeyword == "delegate" || lastKeyword == "event")
            return typePrefix;

        // смотрим вперЄд: если идЄт '(' Ч это метод
        size_t j = posAfterId;
        while (j < src.size() && std::isspace(static_cast<unsigned char>(src[j]))) ++j;
        if (j < src.size() && src[j] == '(') return funcPrefix;

        return varPrefix;
    }
};