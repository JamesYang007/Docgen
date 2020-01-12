#pragma once
#include <string>
#include "symbol.hpp"

namespace docgen {
namespace core {

template <class SymbolType>
struct Token
{
    using symbol_t = SymbolType;

    Token(symbol_t name, std::string&& content, uint32_t leading_ws_count=0)
        : name(name)
        , content(std::move(content))
        , leading_ws_count(leading_ws_count)
    {}

    Token(symbol_t name)
        : Token(name, "")
    {}

    // left undefined for SymbolType != Symbol
    const char* c_str() const;

    symbol_t name;
    std::string content;
    uint32_t leading_ws_count;
};

template <>
inline const char* Token<Symbol>::c_str() const
{
    return (symbol_map.find(name) != symbol_map.end()) ?
        symbol_map.at(name).c_str() : content.c_str();
}

} // namespace core
} // namespace docgen
