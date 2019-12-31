#pragma once
#include <cstring>
#include <array>
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

template <class Cache>
inline Routine process_slash(Cache& cache, const char*& begin, const char* end)
{
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
    ++begin;

    return next_routine;
}

} // namespace details

// Read routine will read from begin until end change states
// only to parse function/class declarations or single-line/block comments.
// It is expected that symbol inside cache is either 
// an empty string or equivalent to "/" at time of call.
template <>
template <class Cache>
inline Routine routine<Routine::READ>::run(Cache& cache, const char*& begin, 
                                           const char* end)
{
    const bool slash_match = !strcmp(cache.symbol.get(), "/");

    // at time of call, slash match with symbol from previous call
    if (slash_match) {
        return details::process_slash(cache, begin, end);
    }

    while (begin != end) {

        // first time seeing '/'
        if (*begin == '/') {
            cache.symbol.push_back('/');
            return details::process_slash(cache, ++begin, end);
        }

        // TODO: function/class declaration
        
        ++begin;
    }

    return Routine::READ;
}

/////////////////////////////////////////////////
// Parser (array of routines) 
/////////////////////////////////////////////////

template <class Cache>
using routine_t = decltype(&routine<Routine::READ>::template run<Cache>);

template <class Cache>
struct RoutineArray
    : std::array<routine_t<Cache>, static_cast<size_t>(Routine::NUM_ROUTINES)>
{};

namespace details {

template <bool cond, class T=void>
using enable_if_t = typename std::enable_if<cond, T>::type;

template <size_t I, class Cache>
constexpr enable_if_t<(I == static_cast<size_t>(Routine::NUM_ROUTINES)), bool>
is_properly_ordered(const RoutineArray<Cache>& parser)
{
    return true;
}

template <size_t I, class Cache>
constexpr enable_if_t<(I < static_cast<size_t>(Routine::NUM_ROUTINES)), bool>
is_properly_ordered(const RoutineArray<Cache>& parser)
{
    return (parser[I] == &routine<static_cast<Routine>(I)>::template run<Cache>) &&
            is_properly_ordered<I+1>(parser);
}

} // namespace details

template <class Cache>
constexpr bool is_properly_ordered(const RoutineArray<Cache>& parser)
{
    return details::is_properly_ordered<0>(parser);
}

} // namespace core
} // namespace parse
} // namespace docgen
