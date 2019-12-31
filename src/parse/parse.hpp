#pragma once
#include <nlohmann/json.hpp>
#include <array>

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
inline Routine routine<Routine::READ>(State& state, const char *&begin, 
                                      const char *end, nlohmann::json&)
{}

template <>
inline Routine routine<Routine::SLASH>(State& state, const char *&begin, 
                                       const char *end, nlohmann::json& parsed)
{}

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

// Parse a file given a filepath.
void parse_file(const char *filepath)
{
    using Routine = core::Routine;
    using State = core::State;

    // All routine functions must have the same prototype.
    // We may arbitrarily use function pointer type of one of them.
    using parser_routine_t = decltype(&core::routine<Routine::READ>);
    static constexpr size_t array_size = static_cast<size_t>(Routine::NUM_ROUTINES);
    using parser_t = std::array<parser_routine_t, array_size>;

    // parser[i] is the ith routine function.
    // NOTE: the order of enum class Routine values 
    // MUST exactly match the order of routines below.
    static constexpr parser_t parser = {
        core::routine<Routine::READ>,
        core::routine<Routine::SLASH>,
        core::routine<Routine::IGNORE_WS>,
        core::routine<Routine::IGNORE_WS_END_BLOCK>,
        core::routine<Routine::PROCESS>,
        core::routine<Routine::PROCESS_END_BLOCK>
    };

    State state = State::DEFAULT;       // initially in default state
    Routine routine = Routine::READ;    // initially in read routine
    nlohmann::json parsed;              // store parsed json information

    // open filepath
    // while (reading chunk) {
    // set begin and end accordingly
    //      while (begin != end) {
    //          routine = parser[static_cast<size_t>(routine)](state, begin, end, parsed);
    //      }
    // }
}

// Parse files in a given directory
void parse_dir(const char *dirpath)
{
    //for (auto& file_path : std::directory_iterator(dirpath)) {
    //        parse_file(file_path);
    //}   
}

} // namespace parse
} // namespace docgen
