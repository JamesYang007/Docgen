#pragma once 

namespace docgen {
namespace parse {
namespace core {

// Tags indicate possible states 
enum class State {
    DEFAULT = 0,
    SINGLE_LINE,
    BLOCK
}; 

} // namespace core
} // namespace parse
} // namespace docgen
