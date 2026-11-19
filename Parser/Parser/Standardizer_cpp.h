#pragma once
#include "Header.h"

class CppStandardizer {
public:
    // -------- настройки ----------
    string typePrefix = "T_";
    string funcPrefix = "f_";
    string varPrefix = "v_";

    // »мена, которые Ќ≈ переименовываютс€:
    // ключевые слова + то, что приходит из стандартной библиотеки.
    set<string> reserved = {
        // ключевые слова C++20
        "alignas","alignof","and","and_eq","asm","auto","bitand","bitor","bool",
        "break","case","catch","char","char8_t","char16_t","char32_t","class",
        "compl","concept","const","consteval","constexpr","constinit","const_cast",
        "continue","co_await","co_return","co_yield","decltype","default","delete",
        "do","double","dynamic_cast","else","enum","explicit","export","extern",
        "false","float","for","friend","goto","if","inline","int","long","mutable",
        "namespace","new","noexcept","not","not_eq","nullptr","operator","or",
        "or_eq","private","protected","public","register","reinterpret_cast",
        "requires","return","short","signed","sizeof","static","static_assert",
        "static_cast","struct","switch","template","this","thread_local","throw",
        "true","try","typedef","typeid","typename","union","unsigned","using",
        "virtual","void","volatile","wchar_t","while","xor","xor_eq",
        // то, что нельз€ ломать
        "main","std",
        // частые типы/имена из STL (расшир€йте при необходимости)
        "size_t","nullptr_t","int8_t","int16_t","int32_t","int64_t",
        "uint8_t","uint16_t","uint32_t","uint64_t",
        "cout","cin","cerr","endl","string","vector","map","set","pair"
    };

    // »мена, которые считаем числами и замен€ем на NUM. ƒополн€йте при необходимости.
    set<string> numberNames = { "pi", "M_E", "M_PI" };

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
            else if (c == '"' || c == '\'') {          // строки и символы -> STR
                skipString(src, i);
                out += "STR";
                lastKeyword.clear();
            }
            else if (c == '#') {                       // препроцессор Ч строка целиком
                out += readLine(src, i);
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
                else if (numberNames.count(id)) {      // pi и подобные -> NUM
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
                // сбиваем контекст на разделител€х, но не на пробелах/скобках
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
        if (lastKeyword == "class" || lastKeyword == "struct" || lastKeyword == "union" ||
            lastKeyword == "enum" || lastKeyword == "typename" || lastKeyword == "namespace")
            prefix = typePrefix;
        else if (looksLikeFunc)                    // дальше идЄт '(' Ч функци€
            prefix = funcPrefix;

        const string newName = prefix + to_string(++counters_[prefix]);
        mapping_[id] = newName;
        return newName;
    }

    // -------- маленькие читатели токенов (сдвигают i за токен) ----------

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

    static void skipString(const string& s, size_t& i) {
        const char q = s[i++];
        while (i < s.size() && s[i] != q) {
            if (s[i] == '\\') ++i;                 // экранированный символ
            ++i;
        }
        if (i < s.size()) ++i;                     // закрывающа€ кавычка
    }

    // 123, 3.14f, 0x1F, 1'000, 10ULL, 1e-5
    static void skipNumber(const string& s, size_t& i) {
        while (i < s.size()) {
            const char c = s[i];
            if (alnum(c) || c == '.' || c == '\'') { ++i; continue; }
            if ((c == '+' || c == '-') &&
                (s[i - 1] == 'e' || s[i - 1] == 'E' || s[i - 1] == 'p' || s[i - 1] == 'P'))
            {
                ++i; continue;
            }                     // знак экспоненты: 1e-5
            break;
        }
    }

    // первый непробельный символ начина€ с i ('\0', если строка кончилась)
    static char nextNonSpace(const string& s, size_t i) {
        while (i < s.size() && space(s[i])) ++i;
        return i < s.size() ? s[i] : '\0';
    }

    static bool space(char c) { return isspace(static_cast<unsigned char>(c)) != 0; }
    static bool digit(char c) { return isdigit(static_cast<unsigned char>(c)) != 0; }
    static bool letter(char c) { return isalpha(static_cast<unsigned char>(c)) != 0; }
    static bool alnum(char c) { return isalnum(static_cast<unsigned char>(c)) != 0; }
};