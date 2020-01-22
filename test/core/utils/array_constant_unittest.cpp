#include <core/utils/array_constant.hpp>
#include <gtest/gtest.h>

namespace docgen {
namespace core {
namespace utils {

struct IsEven
{
    static constexpr bool evaluate(size_t i)
    {
        return (i % 2 == 0);
    }
};

struct array_constant_fixture : ::testing::Test
{
protected:

    // Needed for for_each test
    template <class IdxConstant>
    struct Functor 
    {
    private:
        static constexpr auto arr = make_array({3, 1, 10, 2});
    public:
        static constexpr auto evaluate()
        {
            return arr[IdxConstant::value] * 2;
        }
    };

    template <class Unpacked>
    struct check_unpacked;

    template <class T1, class T2, class T3, class... Ts>
    struct check_unpacked<unpacked<T1, T2, T3, Ts...>> 
    {
        using value_t = std::common_type_t<
            typename T1::value_type,
            typename T2::value_type,
            typename T3::value_type
                >;

        template <value_t x1, value_t x2, value_t x3>
        static constexpr void evaluate()
        {
            static_assert(T1::value == x1);
            static_assert(T2::value == x2);
            static_assert(T3::value == x3);
        }
    };
};

TEST_F(array_constant_fixture, integral_constant_instantiation)
{
    // must be static!
    static constexpr auto x = make_array<size_t>({3});
    using raw_arr_t = std::decay_t<decltype(x)>;
    static_assert(std::is_same_v<raw_arr_t, std::array<size_t, 1>>);
    using array_t = std::integral_constant<const raw_arr_t&, x>;
    static_assert(array_t::value[0] == 3);
}

TEST_F(array_constant_fixture, array_constant_instantiation)
{
    // must be static!
    static constexpr auto x = make_array({3});
    using raw_arr_t = std::decay_t<decltype(x)>;
    static_assert(std::is_same_v<raw_arr_t, std::array<int, 1>>);
    using array_t = array_constant<raw_arr_t, x>;
    static_assert(array_t::value[0] == 3);
}

TEST_F(array_constant_fixture, get_element)
{
    // must be static!
    static constexpr auto arr = make_array({0, 1, 2});
    using array_t = make_array_constant_t<arr>;
    using first_element_t = get_element_t<array_t, 0>;
    using second_element_t = get_element_t<array_t, 1>;
    using third_element_t = get_element_t<array_t, 2>;
    static_assert(first_element_t::value == 0);
    static_assert(second_element_t::value == 1);
    static_assert(third_element_t::value == 2);
}

TEST_F(array_constant_fixture, unpack)
{
    // must be static!
    static constexpr auto arr = make_array({0, 1, 2});
    using array_t = make_array_constant_t<arr>;
    using unpacked_array_t = unpack_t<array_t>;
    check_unpacked<unpacked_array_t>::template evaluate<0, 1, 2>();
}

TEST_F(array_constant_fixture, concat_element_array)
{
    // must be static!
    static constexpr auto arr = make_array<size_t>({1, 2});
    using array_t = make_array_constant_t<arr>;
    static constexpr size_t x = 0;
    using x_t = make_constant_t<x>;
    using array_concat_t = concat_t<x_t, array_t>;
    static_assert(array_concat_t::value[0] == 0);
    static_assert(array_concat_t::value[1] == 1);
    static_assert(array_concat_t::value[2] == 2);
}

TEST_F(array_constant_fixture, concat_array_element)
{
    // must be static!
    static constexpr auto arr = make_array<size_t>({1, 2});
    using array_t = make_array_constant_t<arr>;
    static constexpr size_t x = 0;
    using x_t = make_constant_t<x>;
    using array_concat_t = concat_t<array_t, x_t>;
    static_assert(array_concat_t::value[0] == 1);
    static_assert(array_concat_t::value[1] == 2);
    static_assert(array_concat_t::value[2] == 0);
}

TEST_F(array_constant_fixture, concat_array_array)
{
    // must be static!
    static constexpr auto arr = make_array({0, 1});
    using array_t = make_array_constant_t<arr>;
    static constexpr auto arr2 = make_array({2, 3});
    using array2_t = make_array_constant_t<arr2>;

    using array_concat_t = concat_t<array_t, array2_t>;
    static_assert(array_concat_t::value[0] == 0);
    static_assert(array_concat_t::value[1] == 1);
    static_assert(array_concat_t::value[2] == 2);
    static_assert(array_concat_t::value[3] == 3);
}

TEST_F(array_constant_fixture, subset_idx)
{
    // must be static!
    static constexpr auto arr = make_array({0, 1, 1, 2});
    using array_t = make_array_constant_t<arr>;

    using even_idx_t = subset_idx_t<array_t, IsEven>;

    static_assert(even_idx_t::value[0] == 0);
    static_assert(even_idx_t::value[1] == 3);
}

//TEST_F(array_constant_fixture, subset)
//{
//    // must be static!
//    static constexpr auto arr = make_array({0, 1, 1, 2});
//    using array_t = make_array_constant_t<arr>;
//
//    using even_idx_t = subset_idx_t<array_t, IsEven>;
//    using array_subset_t = subset_t<array_t, even_idx_t>;
//
//    static_assert(array_subset_t::value[0] == 0);
//    static_assert(array_subset_t::value[1] == 2);
//}

//TEST_F(array_constant_fixture, zip)
//{
//    // must be static!
//    static constexpr auto arr1 = make_array<int>({0, 1, 1, 2});
//    static constexpr auto arr2 = make_array<int>({10, 11, 11, 12});
//    using first_array_t = make_array_constant_t<arr1>;
//    using second_array_t = make_array_constant_t<arr2>;
//
//    using array_zip_t = zip_t<first_array_t, second_array_t>;
//
//    static_assert(std::get<0>(array_zip_t::value[0]) == 0);
//    static_assert(std::get<0>(array_zip_t::value[1]) == 1);
//    static_assert(std::get<0>(array_zip_t::value[2]) == 1);
//    static_assert(std::get<0>(array_zip_t::value[3]) == 2);
//
//    static_assert(std::get<1>(array_zip_t::value[0]) == 10);
//    static_assert(std::get<1>(array_zip_t::value[1]) == 11);
//    static_assert(std::get<1>(array_zip_t::value[2]) == 11);
//    static_assert(std::get<1>(array_zip_t::value[3]) == 12);
//}
//
//TEST_F(array_constant_fixture, unzip)
//{
//    // must be static!
//    static constexpr auto arr = make_array<std::pair<int, int>>({
//        {0, 10}, 
//        {1, 11}, 
//        {1, 11}, 
//        {2, 12}
//    });
//    using array_t = make_array_constant_t<arr>;
//
//    using array_unzip_first_t = unzip_t<array_t>;
//
//    static_assert(array_unzip_first_t::value[0] == 0);
//    static_assert(array_unzip_first_t::value[1] == 1);
//    static_assert(array_unzip_first_t::value[2] == 1);
//    static_assert(array_unzip_first_t::value[3] == 2);
//
//    using array_unzip_second_t = unzip_t<array_t, 1>;
//
//    static_assert(array_unzip_second_t::value[0] == 10);
//    static_assert(array_unzip_second_t::value[1] == 11);
//    static_assert(array_unzip_second_t::value[2] == 11);
//    static_assert(array_unzip_second_t::value[3] == 12);
//}
//
//TEST_F(array_constant_fixture, is_in_true)
//{
//    static constexpr auto arr = make_array({0, 1, 1, 2});
//    using array_t = make_array_constant_t<arr>;
//    static constexpr int x = 1;
//    using x_t = make_constant_t<x>;
//    static_assert(is_in_v<x_t, array_t>);
//}
//
//TEST_F(array_constant_fixture, is_in_false)
//{
//    static constexpr auto arr = make_array({0, 1, 1, 2});
//    using array_t = make_array_constant_t<arr>;
//    static constexpr int x = 3;
//    using x_t = make_constant_t<x>;
//    static_assert(!is_in_v<x_t, array_t>);
//}
//
//TEST_F(array_constant_fixture, set_no_repeat)
//{
//    static constexpr auto arr = make_array({0, 1, 2});
//    using array_t = make_array_constant_t<arr>;
//    using set_array_t = set_t<array_t>;
//    static_assert(set_array_t::value[0] == 0);
//    static_assert(set_array_t::value[1] == 1);
//    static_assert(set_array_t::value[2] == 2);
//}
//
//TEST_F(array_constant_fixture, set_repeat)
//{
//    static constexpr auto arr = make_array({0, 1, 2, 1});
//    using array_t = make_array_constant_t<arr>;
//    using set_array_t = set_t<array_t>;
//    static_assert(set_array_t::value[0] == 0);
//    static_assert(set_array_t::value[1] == 2);
//    static_assert(set_array_t::value[2] == 1);
//}
//
//TEST_F(array_constant_fixture, is_unique_true)
//{
//    static constexpr auto arr = make_array({0, 1, 2});
//    using array_t = make_array_constant_t<arr>;
//    static_assert(is_unique_v<array_t>);
//}
//
//TEST_F(array_constant_fixture, is_unique_false)
//{
//    static constexpr auto arr = make_array({0, 1, 2, 1});
//    using array_t = make_array_constant_t<arr>;
//    static_assert(!is_unique_v<array_t>);
//}

} // namespace utils
} // namespace core
} // namespace docgen
