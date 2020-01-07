#pragma once
#include <string>

namespace docgen {
namespace core {

template <class SymbolType>
struct Token
{
    using symbol_t = SymbolType;

    Token(symbol_t name)
        : name(name)
    {}

    Token(symbol_t name, const std::string& content)
        : name(name)
        , content(content)
    {}

    Token(symbol_t name, std::string&& content)
        : name(name)
        , content(std::move(content))
    {}

    symbol_t name;
    std::string content;
};

} // namespace core
} // namespace docgen
