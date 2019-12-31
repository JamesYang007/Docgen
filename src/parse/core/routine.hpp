#pragma once
#include <nlohmann/json.hpp>

namespace docgen {
namespace parse {
namespace core {

// Tags indicating possible states 
enum class State {
    DEFAULT = 0,
    SINGLE_LINE,
    BLOCK
}; 

// Tags indicating possible routines 
enum class Routine {
    READ = 0,
    SLASH,
    IGNORE_WS,
    IGNORE_WS_END_BLOCK,
    PROCESS,
    PROCESS_END_BLOCK,
    NUM_ROUTINES
};

// Forward-declaration of a routine.
// routine will parse the chunk of text starting at begin
// until it reaches end, given current state, and update the json object.
// If a state changes, routine will modify state.
// When a state changes or the chunk has been fully read, the routine will finish.
// It will always return the next routine to execute.
// routine will perform differently depending on routine_tag.
template <Routine routine_tag>
inline Routine routine(State& state, const char *&begin, 
                       const char *end, nlohmann::json& parsed);

template <>
inline Routine routine<Routine::READ>(State&, const char *&begin, 
                                      const char *end, nlohmann::json&)
{
    while (begin != end) {
        if (*begin == '/') {
            ++begin;
            return Routine::SLASH;
        }
        // TODO: function/class declaration
        ++begin;
    }
    return Routine::READ;
}

template <>
inline Routine routine<Routine::SLASH>(State& state, const char *&begin, 
                                       const char *end, nlohmann::json&)
{
    while (begin != end) {
        // single-line comment
        if (*begin == '/') {
            ++begin; 
            state = State::SINGLE_LINE;
            return Routine::IGNORE_WS;
        }

        // block comment
        if (*begin == '*') {
            ++begin;
            state = State::BLOCK;
            return Routine::IGNORE_WS;
        }

        ++begin;
    }

    return Routine::SLASH;
}

template <>
inline Routine routine<Routine::IGNORE_WS>(State& state, const char *&begin, 
                                           const char *end, nlohmann::json& parsed)
{}

template <>
inline Routine routine<Routine::IGNORE_WS_END_BLOCK>(State& state, const char *&begin, 
                                                     const char *end, nlohmann::json& parsed)
{}

template <>
inline Routine routine<Routine::PROCESS>(State& state, const char *&begin, 
                                         const char *end, nlohmann::json& parsed)
{}

template <>
inline Routine routine<Routine::PROCESS_END_BLOCK>(State& state, const char *&begin, 
                                                   const char *end, nlohmann::json& parsed)
{}

} // namespace core
} // namespace parse
} // namespace docgen
