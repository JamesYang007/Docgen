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
        slash,
        single_line,
        block
    };

private:

    

    State state;

};

} // namespace docgen
