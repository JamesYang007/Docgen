#include <core/utils/list.hpp>
#include <gtest/gtest.h>

namespace docgen {
namespace core {
namespace utils {

template <class T>
struct IsInt : std::false_type
{};

template <>
struct IsInt<int> : std::true_type
{};

template <class T>
struct MockTransformer
{
    using type = T;
};

template <>
struct MockTransformer<int>
{
    using type = double;
};

struct list_fixture : ::testing::Test
{
protected:
};

TEST_F(list_fixture, valuelist_char_init) 
{
    using list_t = valuelist<char, 'a', 'b', 'c'>;
    static_assert(list_t::value == 'a');
    static_assert(std::is_same_v<typename list_t::value_type, char>);

    using next_list_t = typename list_t::next_type;
    static_assert(next_list_t::value == 'b');
    static_assert(std::is_same_v<typename next_list_t::value_type, char>);

    using next_next_list_t = typename next_list_t::next_type;
    static_assert(next_next_list_t::value == 'c');
    static_assert(std::is_same_v<typename next_next_list_t::value_type, char>);

    using last_list_t = typename next_next_list_t::next_type;
    static_assert(std::is_same_v<typename last_list_t::value_type, char>);
    static_assert(std::is_convertible_v<last_list_t, empty_tag>);
}

TEST_F(list_fixture, is_valuelist_true) 
{
    using list_t = valuelist<char>;
    static_assert(is_valuelist_v<list_t>); 
}

TEST_F(list_fixture, is_valuelist_false_one) 
{
    using list_t = std::tuple<char, int>;
    static_assert(!is_valuelist_v<list_t>); 
}

TEST_F(list_fixture, is_typelist_true) 
{
    using list_t = typelist<char, float>;
    static_assert(is_typelist_v<list_t>); 
}

TEST_F(list_fixture, is_typelist_false) 
{
    using list_t = std::tuple<char, int>;
    static_assert(!is_typelist_v<list_t>); 
}

TEST_F(list_fixture, is_empty_true) 
{
    using list_t = valuelist<char>;
    static_assert(is_empty_v<list_t>); 
}

TEST_F(list_fixture, is_empty_false_one) 
{
    using list_t = valuelist<char, 'a'>;
    static_assert(!is_empty_v<list_t>); 
}

TEST_F(list_fixture, is_empty_false_two) 
{
    using list_t = valuelist<int, 3, 1>;
    static_assert(!is_empty_v<list_t>); 
}

TEST_F(list_fixture, size_empty) 
{
    using list_t = valuelist<char>;
    static_assert(size_v<list_t> == 0);
}

TEST_F(list_fixture, size_non_empty) 
{
    using list_t = valuelist<char, 'a', 'c'>;
    static_assert(size_v<list_t> == 2);
}

TEST_F(list_fixture, get_0) 
{
    using list_t = valuelist<char, 'a', 'c', 'b'>;
    static_assert(get_v<list_t, 0> == 'a');
}

TEST_F(list_fixture, get_1) 
{
    using list_t = valuelist<char, 'a', 'c', 'b'>;
    static_assert(get_v<list_t, 1> == 'c');
}

TEST_F(list_fixture, get_2) 
{
    using list_t = valuelist<char, 'a', 'c', 'b'>;
    static_assert(get_v<list_t, 2> == 'b');
}

TEST_F(list_fixture, get_t_test)
{
    using list_t = typelist<char, int, float>;
    static_assert(std::is_same_v<get_t<list_t, 0>, char>);
    static_assert(std::is_same_v<get_t<list_t, 1>, int>);
    static_assert(std::is_same_v<get_t<list_t, 2>, float>);
}

TEST_F(list_fixture, concat_empty_empty) 
{
    using list1_t = valuelist<char>;
    using list2_t = valuelist<char>;
    using list_t = concat_t<list1_t, list2_t>;
    static_assert(std::is_same_v<list2_t, list_t>);
}

TEST_F(list_fixture, concat_empty_nonempty) 
{
    using list1_t = valuelist<char>;
    using list2_t = valuelist<char, 'a', 'c'>;
    using list_t = concat_t<list1_t, list2_t>;
    static_assert(std::is_same_v<list2_t, list_t>);
}

TEST_F(list_fixture, concat_nonempty_empty) 
{
    using list1_t = valuelist<char, 'a', 'c'>;
    using list2_t = valuelist<char>;
    using list_t = concat_t<list1_t, list2_t>;
    static_assert(std::is_same_v<list1_t, list_t>);
}

TEST_F(list_fixture, concat_nonempty_nonempty) 
{
    using list1_t = valuelist<char, 'a', 'c'>;
    using list2_t = valuelist<char, 'b'>;
    using list_t = concat_t<list1_t, list2_t>;
    using expected_t = valuelist<char, 'a', 'c', 'b'>;
    static_assert(std::is_same_v<list_t, expected_t>);
}

TEST_F(list_fixture, subset_idx_empty)
{
    using list_t = typelist<>;
    using idx_t = subset_idx_t<list_t, IsInt>;
    static_assert(size_v<idx_t> == 0);
}

TEST_F(list_fixture, subset_idx_four)
{
    using list_t = typelist<int, char, float, int>;
    using idx_t = subset_idx_t<list_t, IsInt>;
    static_assert(size_v<idx_t> == 2);
    static_assert(get_v<idx_t, 0> == 0);
    static_assert(get_v<idx_t, 1> == 3);
}

TEST_F(list_fixture, subset_empty_original)
{
    using list_t = typelist<>;
    using idx_t = subset_idx_t<list_t, IsInt>;
    using subset_list_t = subset_t<list_t, idx_t>;
    static_assert(size_v<subset_list_t> == 0);
}

TEST_F(list_fixture, subset_empty_index)
{
    using list_t = typelist<char, float>;
    using idx_t = subset_idx_t<list_t, IsInt>;
    using subset_list_t = subset_t<list_t, idx_t>;
    static_assert(size_v<subset_list_t> == 0);
}

TEST_F(list_fixture, subset_nonempty)
{
    using list_t = typelist<char, int, float, double, int>;
    using idx_t = subset_idx_t<list_t, IsInt>;
    using subset_list_t = subset_t<list_t, idx_t>;
    static_assert(size_v<subset_list_t> == 2);
    static_assert(std::is_same_v<get_t<subset_list_t, 0>, int>);
    static_assert(std::is_same_v<get_t<subset_list_t, 1>, int>);
}

TEST_F(list_fixture, zip_empty_empty)
{
    using list1_t = typelist<>;
    using list2_t = typelist<>;
    using zipped_t = zip_t<list1_t, list2_t>;
    static_assert(size_v<zipped_t> == 0);
}

TEST_F(list_fixture, zip_empty_nonempty)
{
    using list1_t = typelist<char, float>;
    using list2_t = typelist<char, int>;
    using zipped_t = zip_t<list1_t, list2_t>;
    static_assert(size_v<zipped_t> == 2);
    static_assert(std::is_same_v<get_t<zipped_t, 0>,
            typelist<char, char>>);
    static_assert(std::is_same_v<get_t<zipped_t, 1>,
            typelist<float, int>>);
}

TEST_F(list_fixture, unzip_empty_empty)
{
    using list1_t = typelist<>;
    using list2_t = typelist<>;
    using zipped_t = zip_t<list1_t, list2_t>;
    using unzipped_t = unzip_t<zipped_t>;
    static_assert(size_v<unzipped_t> == 0);
}

TEST_F(list_fixture, unzip_empty_nonempty)
{
    using list1_t = typelist<char, float>;
    using list2_t = typelist<char, int>;
    using zipped_t = zip_t<list1_t, list2_t>;
    using unzipped_t = unzip_t<zipped_t>;
    static_assert(size_v<unzipped_t> == 2);
    static_assert(std::is_same_v<get_t<unzipped_t, 0>, list1_t>);
    static_assert(std::is_same_v<get_t<unzipped_t, 1>, list2_t>);
}

TEST_F(list_fixture, get_first_value)
{
    using list1_t = valuelist<char, 'h', 'e'>;
    using list2_t = valuelist<char>;
    using list3_t = valuelist<char, 'o', 'w'>;
    using list_t = typelist<list1_t, list2_t, list3_t>;
    using first_letters_t = get_first_value_t<list_t>;
    static_assert(get_v<first_letters_t, 0> == 'h');
    static_assert(get_v<first_letters_t, 1> == 'o');
}

TEST_F(list_fixture, is_in_true)
{
    using list_t = valuelist<int, 0, 5, 2>;
    static_assert(is_in_v<list_t, 0>);
    static_assert(is_in_v<list_t, 5>);
    static_assert(is_in_v<list_t, 2>);
}

TEST_F(list_fixture, is_in_false)
{
    using list_t = valuelist<int, 0, 5, 2>;
    static_assert(!is_in_v<list_t, 1>);
    static_assert(!is_in_v<list_t, 3>);
    static_assert(!is_in_v<list_t, 4>);
}

TEST_F(list_fixture, set_empty)
{
    using list_t = valuelist<int>;
    using set_list_t = set_t<list_t>;
    static_assert(size_v<set_list_t> == 0);
}

TEST_F(list_fixture, set_nonempty)
{
    using list_t = valuelist<int, 1, 5, 2, 4, 1, 2, 3>;
    using set_list_t = set_t<list_t>;
    static_assert(size_v<set_list_t> == 5);
    static_assert(is_in_v<set_list_t, 1>);
    static_assert(is_in_v<set_list_t, 2>);
    static_assert(is_in_v<set_list_t, 3>);
    static_assert(is_in_v<set_list_t, 4>);
    static_assert(is_in_v<set_list_t, 5>);
}

TEST_F(list_fixture, left_shift)
{
    using list_t = valuelist<int, 2, 4, 1>;
    using shifted_t = left_shift_t<list_t>;
    static_assert(size_v<shifted_t> == 2);
    static_assert(get_v<shifted_t, 0> == 4);
    static_assert(get_v<shifted_t, 1> == 1);
}

TEST_F(list_fixture, transform)
{
    using list_t = typelist<int, char, float>;
    using transformed_t = transform_t<list_t, MockTransformer>;
    static_assert(size_v<transformed_t> == 3);
    static_assert(std::is_same_v<get_t<transformed_t, 0>, double>);
    static_assert(std::is_same_v<get_t<transformed_t, 1>, char>);
    static_assert(std::is_same_v<get_t<transformed_t, 2>, float>);
}

TEST_F(list_fixture, sort_idx)
{
    using list_t = valuelist<int, 2, 4, 1, 9, 5>;
    using sorted_idx_t = sort_idx_t<list_t>;
    static_assert(get_v<sorted_idx_t, 0> == 2);
    static_assert(get_v<sorted_idx_t, 1> == 0);
    static_assert(get_v<sorted_idx_t, 2> == 1);
    static_assert(get_v<sorted_idx_t, 3> == 4);
    static_assert(get_v<sorted_idx_t, 4> == 3);
}

} // namespace utils
} // namespace core
} // namespace docgen
