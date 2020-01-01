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

template <class Cache>
inline Routine process_slash(Cache& cache, const char*& begin, const char* end)
{
    // symbol should always be equivalent to "/"
    assert(cache.symbol.get()[0] == '/' && 
           cache.symbol.get()[1] == '\0');

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

    assert(slash_match || strcmp(cache.symbol.get(), ""));

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

/////////////////////////////////////////////////
// Parser (array of routines) 
/////////////////////////////////////////////////

template <class Cache>
using routine_t = decltype(&routine<Routine::READ>::template run<Cache>);

template <class Cache>
using routine_array_t = std::array<routine_t<Cache>, static_cast<size_t>(Routine::NUM_ROUTINES)>;

// make_routines<Cache> creates a routine_array_t<Cache> at compile-time.
// It is guaranteed that the order of routines in the array matches exactly 
// that of Routine enum class value list.
namespace details {

template <class Cache, size_t... I>
constexpr routine_array_t<Cache> make_routines(std::index_sequence<I...>)
{
    return {&routine<static_cast<Routine>(I)>::template run<Cache>...};
}

} // namespace details

template <class Cache, size_t Routines = static_cast<size_t>(Routine::NUM_ROUTINES)>
constexpr routine_array_t<Cache> make_routines()
{
    return details::make_routines<Cache>(std::make_index_sequence<Routines>());
}

} // namespace core
} // namespace parse
} // namespace docgen
