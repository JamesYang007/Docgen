#include <parse/core/symbol.hpp>
#include <gtest/gtest.h>

namespace docgen {
namespace parse {
namespace core {

struct symbol_fixture : ::testing::Test
{
protected:
    static constexpr size_t symbol_size = 10;
    Symbol symbol;

    symbol_fixture()
        : symbol(symbol_size)
    {}
};

TEST_F(symbol_fixture, get)
{
    const char* buf = symbol.get();     
    for (size_t i = 0; i < symbol_size; ++i) {
        EXPECT_EQ(buf[i], 0);
    }
}

TEST_F(symbol_fixture, push_back_once)
{
    symbol.push_back('a');
    const std::string actual = symbol.get();
    const std::string expected = "a";
    EXPECT_EQ(actual, expected);
}

TEST_F(symbol_fixture, push_back_twice)
{
    symbol.push_back('a');
    symbol.push_back('b');
    const std::string actual = symbol.get();
    const std::string expected = "ab";
    EXPECT_EQ(actual, expected);
}

TEST_F(symbol_fixture, push_back_all)
{
    std::string expected;
    for (size_t i = 0; i < symbol_size; ++i) {
        symbol.push_back('a' + i);
        expected.push_back('a' + i);
    }
    const std::string actual = symbol.get();
    EXPECT_EQ(actual, expected);
}

#ifndef NDEBUG
TEST_F(symbol_fixture, push_back_error)
{
    for (size_t i = 0; i < symbol_size; ++i) {
        symbol.push_back('a' + i);
    }
    EXPECT_DEATH(symbol.push_back('a'), "Assertion failed");
}
#endif

TEST_F(symbol_fixture, clear)
{
    symbol.push_back('a');
    symbol.push_back('b');
    symbol.push_back('c');
    symbol.clear();
    std::string actual = symbol.get();     
    std::string expected = "";
    EXPECT_EQ(actual, expected);
}

} // namespace core
} // namespace parse
} // namespace docgen
