#pragma once
#include <nlohmann/json.hpp>
#include <array>

namespace docgen {
namespace details {

// forward declaration
// Parse function depending on a particular state.
inline void read_routine(const char *&begin, const char *end, nlohmann::json& json)
{
    // TODO: implement parsing 
}

inline void slash_routine(const char *&begin, const char *end, nlohmann::json& json)
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

} // namespace details

//////////////////////////////////////////////////////
// ChunkParser Declaration
//////////////////////////////////////////////////////

// ChunkParser is a class that will parse a chunk of text into JSON format.
class ChunkParser
{
public:

    // Main function that user will call to parse a chunk of text.
    // This function will update the json object with information extracted from chunk.
    void parse(const char *begin, 
               const char *end,
               nlohmann::json& json);

private:

    // Tags indicating possible states of a ChunkParser object.
    enum State {
        STATE_READ = 0,
        STATE_SLASH,
        STATE_SINGLE_LINE,
        STATE_BLOCK,
        NUM_STATES
    } state_ = STATE_READ;   

    // These private routine functions use the ones in namespace details
    // but additionally return the changed state.
    static State read_routine(const char *&begin,
                              const char *end,
                              nlohmann::json& json);

    static State slash_routine(const char *&begin,
                               const char *end,
                               nlohmann::json& json);

    static State single_line_routine(const char *&begin,
                                     const char *end,
                                     nlohmann::json& json);

    static State block_routine(const char *&begin,
                               const char *end,
                               nlohmann::json& json);

    // All routine functions must have the same prototype.
    // We may (arbitrarily) use function pointer type 
    // of read_routine to set the following alias. 
    using parser_routine_t = decltype(&read_routine);

    using parser_t = std::array<parser_routine_t, NUM_STATES>;

    // parser_[i] is the ith routine function associated with ith enum value in State
    // NOTE: the order of enum State values must match exactly with the order of routines below.
    static constexpr parser_t parser_ = {
        read_routine,
        slash_routine,
        single_line_routine,
        block_routine
    };
};

} // namespace docgen
