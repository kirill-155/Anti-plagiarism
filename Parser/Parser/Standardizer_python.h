#pragma once
#include "Header.h"

class PythonStandardizer {
public:
    // -------- настройки ----------
    string typePrefix = "T_";
    string funcPrefix = "f_";
    string varPrefix = "v_";

    //  лючевые слова + встроенные имена, которые переименовывать нельз€.
    set<string> reserved = {
        // ключевые слова
        "False","None","True","and","as","assert","async","await","break","class",
        "continue","def","del","elif","else","except","finally","for","from",
        "global","if","import","in","is","lambda","nonlocal","not","or","pass",
        "raise","return","try","while","with","yield",
        // м€гкие ключевые слова (match/case)
        "match","case",
        // встроенные функции и типы
        "print","len","range","int","float","str","bool","list","dict","set",
        "tuple","frozenset","bytes","bytearray","complex","type","object","super",
        "input","open","enumerate","zip","map","filter","reversed","sorted",
        "sum","min","max","abs","round","pow","divmod","any","all","iter","next",
        "id","hash","repr","format","ascii","bin","hex","oct","ord","chr",
        "isinstance","issubclass","getattr","setattr","hasattr","delattr",
        "callable","classmethod","staticmethod","property","vars","dir",
        "globals","locals","eval","exec","compile","breakpoint","help",
        // частые исключени€
        "Exception","BaseException","ValueError","TypeError","KeyError",
        "IndexError","AttributeError","RuntimeError","StopIteration",
        "ZeroDivisionError","FileNotFoundError","ImportError","NameError",
        "OSError","ArithmeticError","LookupError","OverflowError",
        "NotImplementedError","KeyboardInterrupt","SystemExit",
        // служебные имена
        "self","cls","__name__","__main__","__file__","__doc__","__init__",
        "__str__","__repr__","__len__","__getitem__","__setitem__","__iter__",
        "__next__","__call__","__enter__","__exit__","__new__","__del__",
        "__add__","__sub__","__mul__","__eq__","__lt__","__gt__","__hash__",
        // частые модули (расшир€йте при необходимости)
        "math","os","sys","random","re","json","time","datetime","collections",
        "itertools","functools","typing","string","copy","heapq","statistics",
        "fractions","decimal","pathlib","io","abc","enum","dataclasses"
    };

    // »мена, которые считаем числами: math.pi -> math.NUM. ƒополн€йте.
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

            if (space(c)) {                            // пробелы и отступы Ч как есть
                out += c;
                ++i;
            }
            else if (c == '#') {                       // комментарии Ч целиком
                out += readLine(src, i);
            }
            else if (c == '"' || c == '\'') {          // строки -> STR
                skipString(src, i);
                out += "STR";
                lastKeyword.clear();
            }
            else if (digit(c)) {                       // числа -> NUM
                skipNumber(src, i);
                out += "NUM";
                lastKeyword.clear();
            }
            else if (letter(c) || c == '_') {          // идентификаторы и ключевые слова
                const size_t start = i;
                const string id = readWord(src, i);

                if (id.size() <= 2 && i < src.size() && (src[i] == '"' || src[i] == '\'') &&
                    isStringPrefix(id)) {              // r"...", b'...', f"..." и т.п.
                    skipString(src, i);
                    out += "STR";
                    lastKeyword.clear();
                }
                else if ((id == "import" || id == "from") && atLineStart(src, start)) {
                    out += id;                         // import ... / from ... import ... Ч целиком
                    out += readStatement(src, i);
                    lastKeyword.clear();
                }
                else if (skipDotNumber(src, i)) {              // <-- math.pi -> NUM целиком
                    out += "NUM";
                    lastKeyword.clear();
                }
                else if (reserved.count(id)) {
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
                if (c == ';' || c == '=' || c == ',' || c == ')' || c == '{' || c == '}' || c == ':')
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
        if (lastKeyword == "class")                // class X -> T_
            prefix = typePrefix;
        else if (lastKeyword == "def" || looksLikeFunc)   // def f / вызов f( -> f_
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

    // одинарные/двойные/тройные кавычки, с escape-последовательност€ми
    static void skipString(const string& s, size_t& i) {
        const char q = s[i];
        if (i + 2 < s.size() && s[i + 1] == q && s[i + 2] == q) {   // тройные кавычки
            i += 3;
            while (i < s.size() && !(s[i] == q && i + 2 < s.size() && s[i + 1] == q && s[i + 2] == q)) {
                if (s[i] == '\\') ++i;             // экранированный символ
                ++i;
            }
            if (i < s.size()) i += 3;              // закрывающа€ тройка
        }
        else {
            ++i;
            while (i < s.size() && s[i] != q) {
                if (s[i] == '\\') ++i;
                ++i;
            }
            if (i < s.size()) ++i;                 // закрывающа€ кавычка
        }
    }

    // 123, 3.14, 0x1F, 0b1010, 0o777, 1_000, 1e-5, 2j
    static void skipNumber(const string& s, size_t& i) {
        while (i < s.size()) {
            const char c = s[i];
            if (alnum(c) || c == '.' || c == '_') { ++i; continue; }
            if ((c == '+' || c == '-') && (s[i - 1] == 'e' || s[i - 1] == 'E')) { ++i; continue; }
            break;                                 // знак экспоненты: 1e-5
        }
    }

    // сразу после слова идЄт ".pi" (им€ из numberNames)? съедаем ".pi", сдвигаем i за него
    bool skipDotNumber(const std::string& s, size_t& i) const {
        if (i >= s.size() || s[i] != '.') return false;
        size_t j = i + 1;
        const size_t start = j;
        while (j < s.size() && (alnum(s[j]) || s[j] == '_')) ++j;
        if (j == start || numberNames.count(s.substr(start, j - start)) == 0) return false;
        i = j;
        return true;
    }

    // префиксы строковых литералов: r, b, u, f и комбинации (регистр не важен)
    static bool isStringPrefix(const string& w) {
        string p;
        for (char c : w) p += static_cast<char>(tolower(static_cast<unsigned char>(c)));
        return p == "r" || p == "b" || p == "u" || p == "f" ||
            p == "rb" || p == "br" || p == "rf" || p == "fr";
    }

    // перед pos на этой строке только пробелы?
    // (чтобы не прин€ть raise ... from e / yield from за импорт)
    static bool atLineStart(const string& s, size_t pos) {
        while (pos > 0 && s[pos - 1] != '\n') {
            if (!space(s[pos - 1])) return false;
            --pos;
        }
        return true;
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