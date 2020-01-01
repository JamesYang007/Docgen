#pragma once
#include <nlohmann/json.hpp>
#include "state.hpp"
#include "symbol.hpp"

namespace docgen {
namespace parse {
namespace core {

// Status is a structure that needs to be remembered
// across batches and between routines.
// MaxSymbolSize is the maximum symbol length (not including null-terminator).
struct Status
{
    Status(uint32_t symbol_size)
        : symbol(symbol_size)
    {}

    State state = State::DEFAULT; 
    Symbol symbol;
    nlohmann::json parsed;
};

} // namespace core
} // namespace parse
} // namespace docgen
