#pragma once
#include <cstring>
#include <array>
#include <utility>
#include "state.hpp"

namespace docgen {
namespace parse {
namespace core {

// Tags indicate possible routines 
enum class Routine {
    READ = 0,
    IGNORE_WS,
    PROCESS,
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
struct routine
{
    template <class Cache>
    static Routine run(Cache& cache, const char*& begin, const char* end);
};

namespace details {

// process_slash will read a character pointed by begin so long as begin != end.
// It is expected that symbol is "/".
// If begin points to '/', change state -> SINGLE_LINE, next routine -> IGNORE_WS, clear symbol.
// If begin points to '*', change state -> BLOCK, next routine -> IGNORE_WS, clear symbol.
// If begin == end, this implies end of batch and hence we do not change state/symbol, and next routine -> READ.
template <class Cache>
inline Routine process_slash(Cache& cache, const char* begin, const char* end)
{
    // symbol should always be equivalent to "/"
    assert((cache.symbol.get()[0] == '/') && 
           (cache.symbol.get()[1] == '\0'));

    Routine next_routine = Routine::READ;

    if (begin == end) {
        return next_routine;
    }

    // single-line
    if (*begin == '/') {
        cache.state = State::SINGLE_LINE;
        next_routine = Routine::IGNORE_WS;
    }

    // block
    else if (*begin == '*') {
        cache.state = State::BLOCK;
        next_routine = Routine::IGNORE_WS;
    }

    cache.symbol.clear();

    return next_routine;
}

} // namespace details

// Read routine will read from begin to end.
// It is expected that symbol is either empty or "/" at time of call.
// At end of call, begin will point to next character not read by this routine or
// be the same as end if batch has been fully read.
// If begin == end, no changes other than:
//      next routine -> READ
// If symbol is empty at time of call and there exists a comment in batch,
//      next routine -> IGNORE_WS
//      state -> SINGLE_LINE/BLOCK (depending on if comment were single line or block)
//      symbol -> empty
//      begin -> points to first char after "//" or "/*"
// If symbol is "/" at time of call, it checks for comment starting at begin continuing from symbol.
// Then, changes are identical to the above case.
// Otherwise, batch will be fully read and the only changes are:
//      next routine -> READ
//      begin == end
template <>
template <class Cache>
inline Routine routine<Routine::READ>::run(Cache& cache, const char*& begin, 
                                           const char* end)
{
    if (begin == end) {
        return Routine::READ;
    }

    bool slash_match = ((cache.symbol.get()[0] == '/') &&
                        (cache.symbol.get()[1] == '\0'));

    assert(slash_match || (cache.symbol.get()[0] == '\0'));

    // slash match from previous batch
    if (slash_match) {
        Routine next_routine = details::process_slash(cache, begin, end);
        ++begin;
        if (next_routine != Routine::READ) {
            return next_routine;
        }
        slash_match = false;
    }

    for (; begin != end; ++begin) {

        // if previous character were a slash
        if (slash_match) {
            Routine next_routine = details::process_slash(cache, begin, end);
            if (next_routine != Routine::READ) {
                ++begin;
                return next_routine;
            }
            slash_match = false;
        }

        if (*begin == '/') {
            cache.symbol.push_back('/');
            slash_match = true;
        }

        // TODO: function/class declaration
        
    }

    return Routine::READ;
}

template <>
template <class Cache>
inline Routine routine<Routine::IGNORE_WS>::run(Cache& cache, const char*& begin, 
                                                const char* end)
{
    // TODO: implement and replace dummy code
    static_cast<void>(cache);
    static_cast<void>(begin);
    static_cast<void>(end);
    return Routine::IGNORE_WS;
}

template <>
template <class Cache>
inline Routine routine<Routine::PROCESS>::run(Cache& cache, const char*& begin, 
                                                const char* end)
{
    // TODO: implement and replace dummy code
    static_cast<void>(cache);
    static_cast<void>(begin);
    static_cast<void>(end);
    return Routine::PROCESS;
}

} // namespace core
} // namespace parse
} // namespace docgen
