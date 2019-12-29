#include "docparser.hpp"

namespace docgen {

void DocParser::parse(const char *begin, 
                      const char *end, 
                      nlohmann::json& json)
{
    switch(state) 
    {
        case State::read:     
            {
                parse<State::read>(begin, end, json);            
                break;
            }
        case State::slash:     
            {
                parse<State::slash>(begin, end, json);            
                break;
            }
        case State::single_line:     
            {
                parse<State::single_line>(begin, end, json);            
                break;
            }
        case State::block:     
            {
                parse<State::block>(begin, end, json);            
                break;
            }
    }
}

} // namespace docgen
