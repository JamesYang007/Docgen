#pragma once
#include <nlohmann/json.hpp>

namespace docgen {
namespace parse {
namespace core {

// Tags indicating possible states of a ChunkParser object.
enum class State {
    READ = 0,
    SINGLE_LINE,
    BLOCK,
    NUM_STATES
}; 

// These routines will parse the chunk of text starting at begin
// until it reaches end and update the json object.
// When a state changes or the chunk has been fully read, the routine will finish.
inline void read_routine(const char *&begin, const char *end, nlohmann::json&)
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

// Parse a file given a filepath.
void parse_file(const char *filepath)
{

    // All routine functions must have the same prototype.
    // We may (arbitrarily) use function pointer type 
    // of read_routine to set the following alias. 
    using parser_routine_t = decltype(&core::read_routine);
    static constexpr size_t array_size = static_cast<size_t>(core::State::NUM_STATES);
    using parser_t = std::array<parser_routine_t, array_size>;

    // parser_[i] is the ith routine function associated with ith enum value in State
    // NOTE: the order of enum State values must match exactly with the order of routines below.
    static constexpr parser_t parser = {
        core::read_routine,
        core::single_line_routine,
        core::block_routine
    };
    core::State state = core::State::READ;
    nlohmann::json parsed;

    // open filepath
    // while (reading chunk) {
    // set begin and end accordingly
    // while (begin != end) {
    //     // begin will get updated inside the parser function
    //     // the return value is the next state 
    //     state = core::parser[state](begin, end, parsed);
    // }
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
