#pragma once
#include <parse/core/routine.hpp>
#include <gtest/gtest.h>

namespace docgen {
namespace parse {
namespace core {

struct routine_fixture : ::testing::Test
{
protected:
    static constexpr uint32_t symbol_size = 10;
    Status status;
    Routine next_routine;
    const char* begin = nullptr;
    const char* end = nullptr;

    routine_fixture()
        : status(symbol_size)
    {}

    void process_slash_setup()
    {
        status.symbol.clear();
        status.symbol.push_back('/');
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
        EXPECT_EQ(status.state, expected_state);
        EXPECT_FALSE(strcmp(status.symbol.get(), expected_symbol)); 
        EXPECT_EQ(begin, expected_begin); 
    }
};

} // namespace core
} // namespace parse
} // namespace docgen
