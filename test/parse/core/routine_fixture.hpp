#pragma once

#include <parse/core/routine.hpp>
#include <parse/core/cache.hpp>
#include <gtest/gtest.h>

namespace docgen {
namespace parse {
namespace core {

struct routine_fixture : ::testing::Test
{
protected:
    using cache_t = Cache<10>;
    cache_t cache;
    Routine next_routine;
    const char* begin = nullptr;
    const char* end = nullptr;

    void process_slash_setup()
    {
        cache.symbol.clear();
        cache.symbol.push_back('/');
    }

    void text_setup(const char* text) 
    {
        begin = text;
        end = text + strlen(text);
    }

    void check_routine(Routine expected_routine, State expected_state, 
                       const char* expected_symbol, const char* expected_begin)
    {
        EXPECT_EQ(next_routine, expected_routine);
        EXPECT_EQ(cache.state, expected_state);
        EXPECT_FALSE(strcmp(cache.symbol.get(), expected_symbol)); 
        EXPECT_EQ(begin, expected_begin); 
    }
};

} // namespace core
} // namespace parse
} // namespace docgen
