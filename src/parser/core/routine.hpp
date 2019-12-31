#pragma once
#include <nlohmann/json.hpp>

namespace docgen {
namespace parser {
namespace core {

// These routines will parse the chunk of text starting at begin
// until it reaches end and update the json object.
// When a state changes or the chunk has been fully read, the routine will finish.
inline void read_routine(const char *&begin, const char *end, nlohmann::json& json)
{
    // TODO: implement parsing 
}

inline void single_line_routine(const char *&begin, const char *end, nlohmann::json& json)
{
    // TODO: implement parsing 
}

inline void block_routine(const char *&begin, const char *end, nlohmann::json& json)
{
    // TODO: implement parsing 
}

} // namespace core
} // namespace parser
} // namespace docgen
