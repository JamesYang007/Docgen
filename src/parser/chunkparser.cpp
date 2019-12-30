#include "chunkparser.hpp"

namespace docgen {

void ChunkParser::parse(const char *begin, 
                        const char *end, 
                        nlohmann::json& json)
{
    while (begin < end) {
        // begin will get updated inside the parser function
        // the return value is the next state 
        state_ = parser_[state_](begin, end, json);            
    }
}

ChunkParser::State ChunkParser::read_routine(const char *&begin,
                                             const char *end,
                                             nlohmann::json& json)
{
    // TODO: implement
}

ChunkParser::State ChunkParser::slash_routine(const char *&begin,
                                              const char *end,
                                              nlohmann::json& json)
{
    // TODO: implement
}

ChunkParser::State ChunkParser::single_line_routine(const char *&begin,
                                                    const char *end,
                                                    nlohmann::json& json)
{
    // TODO: implement
}

ChunkParser::State ChunkParser::block_routine(const char *&begin,
                                              const char *end,
                                              nlohmann::json& json)
{
    // TODO: implement
}


} // namespace docgen
