#include "chunkparser.hpp"

namespace docgen {

void ChunkParser::parse(const char *begin, 
                        const char *end)
{
    while (begin != end) {
        // begin will get updated inside the parser function
        // the return value is the next state 
        state_ = parser_[state_](begin, end);
    }
}

ChunkParser::State ChunkParser::read_routine(const char *&begin,
                                             const char *end)
{
    // TODO: implement
}

ChunkParser::State ChunkParser::single_line_routine(const char *&begin,
                                                    const char *end)
{
    // TODO: implement
}

ChunkParser::State ChunkParser::block_routine(const char *&begin,
                                              const char *end)
{
    // TODO: implement
}


} // namespace docgen
