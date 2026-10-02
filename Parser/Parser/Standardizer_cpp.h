#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <cctype>

class CppStandardizer {
public:
    // -------- настройки ----------
    std::string typePrefix = "T_";
    std::string funcPrefix = "f_";
    std::string varPrefix = "v_";

    // Имена, которые НЕ переименовываются:
    // ключевые слова + то, что приходит из стандартной библиотеки.
    std::unordered_set<std::string> reserved = {
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
        // то, что нельзя ломать
        "main","std",
        // частые типы/имена из STL (расширяйте при необходимости)
        "size_t","nullptr_t","int8_t","int16_t","int32_t","int64_t",
        "uint8_t","uint16_t","uint32_t","uint64_t",
        "cout","cin","cerr","endl","string","vector","map","set","pair"
    };

    // ---------- основной вход ----------
    std::string standardize(const std::string& src) {
        mapping_.clear();
        counters_.clear();

        std::string out;
        out.reserve(src.size());

        size_t i = 0, n = src.size();
        std::string lastKeyword;   // последнее встреченное ключевое слово

        while (i < n) {
            const char c = src[i];

            // пробелы сохраняем как есть
            if (std::isspace(static_cast<unsigned char>(c))) {
                out += src[i++];
                continue;
            }

            // // комментарий до конца строки
            if (c == '/' && i + 1 < n && src[i + 1] == '/') {
                while (i < n && src[i] != '\n') out += src[i++];
                continue;
            }
            // /* блочный комментарий */
            if (c == '/' && i + 1 < n && src[i + 1] == '*') {
                out += src[i++]; out += src[i++];
                while (i < n) {
                    if (src[i] == '*' && i + 1 < n && src[i + 1] == '/') {
                        out += src[i++]; out += src[i++];
                        break;
                    }
                    out += src[i++];
                }
                continue;
            }

            // строковый / символьный литерал (с escape-последовательностями)
            if (c == '"' || c == '\'') {
                const char q = c;
                out += src[i++];
                while (i < n) {
                    if (src[i] == '\\' && i + 1 < n) {
                        out += src[i++]; out += src[i++];
                    }
                    else if (src[i] == q) {
                        out += src[i++];
                        break;
                    }
                    else {
                        out += src[i++];
                    }
                }
                continue;
            }

            // препроцессор: пропускаем всю строку
            if (c == '#') {
                while (i < n && src[i] != '\n') out += src[i++];
                continue;
            }

            // идентификатор / ключевое слово
            if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
                const size_t start = i;
                while (i < n &&
                    (std::isalnum(static_cast<unsigned char>(src[i])) || src[i] == '_'))
                    ++i;
                const std::string id = src.substr(start, i - start);

                if (reserved.count(id)) {
                    out += id;
                    lastKeyword = id;          // для контекста (class / struct / int / ...)
                }
                else {
                    out += renamed(id, lastKeyword, src, i);
                    lastKeyword.clear();
                }
                continue;
            }

            // числа (упрощённо; суффиксы ULL/_ms попадут в следующий токен-идентификатор)
            if (std::isdigit(static_cast<unsigned char>(c))) {
                while (i < n && (std::isalnum(static_cast<unsigned char>(src[i])) ||
                    src[i] == '.' || src[i] == '\''))
                    out += src[i++];
                lastKeyword.clear();
                continue;
            }

            // прочие символы (операторы, скобки, запятые, точки и т.п.)
            const char p = src[i++];
            out += p;
            // «сбиваем» контекст на операторах-разделителях, но не на пробелах/скобках
            if (p == ';' || p == '=' || p == ',' || p == ')' || p == '{' || p == '}')
                lastKeyword.clear();
        }
        return out;
    }

    const std::unordered_map<std::string, std::string>& mapping() const { return mapping_; }

private:
    std::unordered_map<std::string, std::string> mapping_;   // old -> new
    std::unordered_map<std::string, int>         counters_;  // prefix -> счётчик

    // Подбираем префикс по контексту и, если имя новое, генерируем новый идентификатор.
    std::string renamed(const std::string& id,
        const std::string& lastKeyword,
        const std::string& src,
        size_t posAfterId) {
        auto it = mapping_.find(id);
        if (it != mapping_.end()) return it->second;   // уже знаем это имя

        std::string prefix = choosePrefix(lastKeyword, src, posAfterId);
        const std::string newName = prefix + std::to_string(++counters_[prefix]);
        mapping_.emplace(id, newName);
        return newName;
    }

    std::string choosePrefix(const std::string& lastKeyword,
        const std::string& src,
        size_t posAfterId) const {
        // 1) контекст типа
        if (lastKeyword == "class" || lastKeyword == "struct" ||
            lastKeyword == "union" || lastKeyword == "enum" ||
            lastKeyword == "typename" || lastKeyword == "namespace")
            return typePrefix;

        // 2) заглядываем вперёд: если дальше идёт '(', это вызов/объявление функции
        size_t j = posAfterId;
        while (j < src.size() && std::isspace(static_cast<unsigned char>(src[j]))) ++j;
        if (j < src.size() && src[j] == '(') return funcPrefix;

        // 3) всё остальное — переменная
        return varPrefix;
    }
};