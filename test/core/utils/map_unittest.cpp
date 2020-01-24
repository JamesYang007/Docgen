#include <core/utils/map.hpp>
#include <gtest/gtest.h>

namespace docgen {
namespace core {
namespace utils {

struct map_fixture : ::testing::Test
{
protected:
};

TEST_F(map_fixture, map_ctor)
{
    static constexpr auto mapper = make_map<char, int, true>({
        {'a', 1},
        {'b', 3}
    });
    static_cast<void>(mapper);
}

TEST_F(map_fixture, map_find)
{
    static constexpr auto mapper = make_map<char, int, true>({
        {'a', 1},
        {'b', 3}
    });

    static_assert(mapper.find('a') != mapper.end());
    static_assert((*mapper.find('a')).first == 'a');
    static_assert(mapper.find('b') != mapper.end());
    static_assert((*mapper.find('b')).first == 'b');
    static_assert(mapper.find('c') == mapper.end());
}

TEST_F(map_fixture, map_find_nonconstexpr)
{
    auto mapper = make_map<char, int, true>({
        {'a', 1},
        {'b', 3}
    });

    EXPECT_NE(mapper.find('a'), mapper.end());
    EXPECT_EQ((*mapper.find('a')).first, 'a');
    EXPECT_NE(mapper.find('b'), mapper.end());
    EXPECT_EQ((*mapper.find('b')).first, 'b');
    EXPECT_EQ(mapper.find('c'), mapper.end());
}

} // namespace utils
} // namespace core
} // namespace docgen
