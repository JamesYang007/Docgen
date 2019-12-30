#pragma once
#include <nlohmann/json.hpp>
#include <array>

namespace docgen {

// Tags indicating possible states of a DocParser object.
enum class State {
    read = 0,
    slash,
    single_line,
    block,
    last = block // update this variable if last enum changes
};

namespace details {

// forward declaration
// Parse function depending on a particular state.
template <State state>
inline const char *parse(const char *begin, 
                         const char *end,
                         nlohmann::json& json);


} // namespace details

//////////////////////////////////////////////////////
// DocParser Declaration
//////////////////////////////////////////////////////

// DocParser is a class that will parse a chunk of text into JSON format.
class DocParser
{
public:

    // Main function that user will call to parse a chunk of text.
    // This function will update the json object with information extracted from chunk.
    void parse(const char *begin, 
               const char *end,
               nlohmann::json& json);

private:
    using parse_fn_type = std::function<
        const char *(const char *, const char *, nlohmann::json&)
        >;
    static constexpr size_t array_size = static_cast<size_t>(State::last);
    static const std::array<parse_fn_type, array_size> parse_map;
    State state;
};

//////////////////////////////////////////////////////
// parse Definition
//////////////////////////////////////////////////////
namespace details {

template <>
inline const char *parse<State::read>(
        const char *begin, const char *end, nlohmann::json& json)
{
    // TODO: implement parsing for read state
}

template <>
inline const char *parse<State::slash>(
        const char *begin, const char *end, nlohmann::json& json)
{
    // TODO: implement parsing for slash state
}

// TODO: implement other parse functions

} // namespace details

} // namespace docgen
