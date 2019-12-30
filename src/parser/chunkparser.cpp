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

} // namespace docgen
