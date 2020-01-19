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

inline bool operator==(const Token<Symbol>& t1, const Token<Symbol>& t2) {
	return t1.name == t2.name;
}

} // namespace core
} // namespace docgen

namespace std {
	
template <>
struct hash<docgen::core::Token<docgen::core::Symbol>>
{
	size_t operator()(const docgen::core::Token<docgen::core::Symbol>& t) const
	{
		return hash<size_t>()(static_cast<size_t>(t.name));
	}
};

} // namespace std
