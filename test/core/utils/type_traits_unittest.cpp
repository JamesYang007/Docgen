#include <core/utils/type_traits.hpp>
#include <gtest/gtest.h>

namespace docgen {
namespace core {
namespace utils {

struct type_traits_fixture : ::testing::Test
{
protected:
};

TEST_F(type_traits_fixture, make_pair_array_different_data_types)
{
    static constexpr auto pair_arr = make_pair_array<int, double>({
        {0, 3.14},
        {1, 2.71}
    });
    static_assert(pair_arr[0].first == 0);
    static_assert(pair_arr[0].second == 3.14);
    static_assert(pair_arr[1].first == 1);
    static_assert(pair_arr[1].second == 2.71);
}
    
} // namespace utils
} // namespace core
} // namespace docgen
