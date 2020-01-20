#include <core/lex/experimental/string_array_utils.hpp>
#include <gtest/gtest.h>

namespace docgen {
namespace core {
namespace lex {
namespace experimental {

enum class MockSymbol {
    symbol_0,
    symbol_1
};

struct StringCondition
{
    static constexpr bool check(const std::string_view& str)
    {
        return !str.empty();
    }
};

struct string_array_utils_fixture : ::testing::Test
{
protected:
    using symbol_t = MockSymbol;
};

TEST_F(string_array_utils_fixture, test)
{
    using namespace std::literals;

    static constexpr auto arr = std::array<std::string_view, 3>{{
            ""sv,
            "ab"sv,
            "abc"sv
    }};

    static constexpr auto arr_subset_idx = 
        subset_idx<StringCondition, arr>();

    static_assert(std::is_same_v<std::decay_t<decltype(arr_subset_idx)>, 
                                 std::array<size_t, 2>>);
    static_assert(arr_subset_idx[0] == 1);
    static_assert(arr_subset_idx[1] == 2);

    static constexpr auto arr_subset =
        subset(arr, arr_subset_idx);

    static constexpr auto arr_2 = concat("hello"sv, arr_subset);

    EXPECT_EQ(arr_2[0], "hello");
    EXPECT_EQ(arr_2[1], "ab");
    EXPECT_EQ(arr_2[2], "abc");

    constexpr auto trie = make_trie(
            std::make_pair("ab"sv, symbol_t::symbol_0),
            std::make_pair("abc"sv, symbol_t::symbol_0),
            std::make_pair("abcd"sv, symbol_t::symbol_0),
            std::make_pair("ba"sv, symbol_t::symbol_0)
            );

    static_cast<void>(trie);
}

} // namespace experimental
} // namespace lex
} // namespace core
} // namespace docgen
