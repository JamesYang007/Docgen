#pragma once
#include <nlohmann/json.hpp>

namespace docgen {

// DocParser is a class that will parse a chunk of text into JSON format.
class DocParser
{
public:

    // Main function that user will call to parse a chunk of text.
    // This function will update the json object with information extracted from chunk.
    void parse(const char *chunk, nlohmann::json& json);

    // Tags indicating possible states of a DocParser object.
    enum class State {
        read,
        slash,
        single_line,
        block
    };

private:

    // Parse function depending on a particular state.
    template <State state>
    const char *parse(const char *chunk, nlohmann::json& json);

    State state;

};

template <>
const char *DocParser::parse<DocParser::State::read>(
        const char *chunk, nlohmann::json& json)
{
    // TODO: implement parsing for read state
}

template <>
const char *DocParser::parse<DocParser::State::slash>(
        const char *chunk, nlohmann::json& json)
{
    // TODO: implement parsing for slash state
}

} // namespace docgen
