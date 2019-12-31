#pragma once
#include <nlohmann/json.hpp>
#include "state.hpp"
#include "symbol.hpp"

namespace docgen {
namespace parse {
namespace core {

// Cache is a structure that needs to be remembered
// across batches and between routines.
// MaxSymbolSize is the maximum symbol length (not including null-terminator).
template <size_t MaxSymbolSize>
struct Cache
{
    State state;        
    Symbol<MaxSymbolSize> symbol;
    nlohmann::json parsed;
};

} // namespace core
} // namespace parse
} // namespace docgen
