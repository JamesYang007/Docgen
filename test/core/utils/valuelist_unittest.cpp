#include <core/utils/valuelist.hpp>
#include <gtest/gtest.h>

namespace docgen {
namespace core {
namespace utils {

struct IsEven
{
    static constexpr bool evaluate(int x)
    {
        return (x % 2) == 0;
    }
};

struct valuelist_fixture : ::testing::Test
{
protected:
};

TEST_F(valuelist_fixture, valuelist_char_init) 
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

TEST_F(valuelist_fixture, is_valuelist_true) 
{
    using list_t = valuelist<char>;
    static_assert(is_valuelist_v<list_t>); 
}

TEST_F(valuelist_fixture, is_valuelist_false_one) 
{
    using list_t = std::tuple<char, int>;
    static_assert(!is_valuelist_v<list_t>); 
}

TEST_F(valuelist_fixture, is_empty_true) 
{
    using list_t = valuelist<char>;
    static_assert(is_empty_v<list_t>); 
}

TEST_F(valuelist_fixture, is_empty_false_one) 
{
    using list_t = valuelist<char, 'a'>;
    static_assert(!is_empty_v<list_t>); 
}

TEST_F(valuelist_fixture, is_empty_false_two) 
{
    using list_t = valuelist<int, 3, 1>;
    static_assert(!is_empty_v<list_t>); 
}

TEST_F(valuelist_fixture, size_empty) 
{
    using list_t = valuelist<char>;
    static_assert(size_v<list_t> == 0);
}

TEST_F(valuelist_fixture, size_non_empty) 
{
    using list_t = valuelist<char, 'a', 'c'>;
    static_assert(size_v<list_t> == 2);
}

TEST_F(valuelist_fixture, get_0) 
{
    using list_t = valuelist<char, 'a', 'c', 'b'>;
    static_assert(get_v<list_t, 0> == 'a');
}

TEST_F(valuelist_fixture, get_1) 
{
    using list_t = valuelist<char, 'a', 'c', 'b'>;
    static_assert(get_v<list_t, 1> == 'c');
}

TEST_F(valuelist_fixture, get_2) 
{
    using list_t = valuelist<char, 'a', 'c', 'b'>;
    static_assert(get_v<list_t, 2> == 'b');
}

TEST_F(valuelist_fixture, concat_empty_empty) 
{
    using list1_t = valuelist<char>;
    using list2_t = valuelist<char>;
    using list_t = concat_t<list1_t, list2_t>;
    static_assert(std::is_same_v<list2_t, list_t>);
}

TEST_F(valuelist_fixture, concat_empty_nonempty) 
{
    using list1_t = valuelist<char>;
    using list2_t = valuelist<char, 'a', 'c'>;
    using list_t = concat_t<list1_t, list2_t>;
    static_assert(std::is_same_v<list2_t, list_t>);
}

TEST_F(valuelist_fixture, concat_nonempty_empty) 
{
    using list1_t = valuelist<char, 'a', 'c'>;
    using list2_t = valuelist<char>;
    using list_t = concat_t<list1_t, list2_t>;
    static_assert(std::is_same_v<list1_t, list_t>);
}

TEST_F(valuelist_fixture, concat_nonempty_nonempty) 
{
    using list1_t = valuelist<char, 'a', 'c'>;
    using list2_t = valuelist<char, 'b'>;
    using list_t = concat_t<list1_t, list2_t>;
    using expected_t = valuelist<char, 'a', 'c', 'b'>;
    static_assert(std::is_same_v<list_t, expected_t>);
}

TEST_F(valuelist_fixture, subset_idx_empty)
{
    using list_t = valuelist<int>;
    using idx_t = subset_idx_t<list_t, IsEven>;
    static_assert(size_v<idx_t> == 0);
}

TEST_F(valuelist_fixture, subset_idx_four)
{
    using list_t = valuelist<int, 3, 4, 2, 1>;
    using idx_t = subset_idx_t<list_t, IsEven>;
    static_assert(size_v<idx_t> == 2);
    static_assert(get_v<idx_t, 0> == 1);
    static_assert(get_v<idx_t, 1> == 2);
}

TEST_F(valuelist_fixture, subset_empty_original)
{
    using list_t = valuelist<int>;
    using idx_t = subset_idx_t<list_t, IsEven>;
    using subset_list_t = subset_t<list_t, idx_t>;
    static_assert(size_v<subset_list_t> == 0);
}

TEST_F(valuelist_fixture, subset_empty_index)
{
    using list_t = valuelist<int, 1, 3>;
    using idx_t = subset_idx_t<list_t, IsEven>;
    using subset_list_t = subset_t<list_t, idx_t>;
    static_assert(size_v<subset_list_t> == 0);
}

TEST_F(valuelist_fixture, subset_nonempty)
{
    using list_t = valuelist<int, 1, 3, 2, 1, 4>;
    using idx_t = subset_idx_t<list_t, IsEven>;
    using subset_list_t = subset_t<list_t, idx_t>;
    static_assert(size_v<subset_list_t> == 2);
    static_assert(get_v<subset_list_t, 0> == 2);
    static_assert(get_v<subset_list_t, 1> == 4);
}

TEST_F(valuelist_fixture, zip_empty_empty)
{
    using list1_t = valuelist<char>;
    using list2_t = valuelist<char>;
    using zipped_t = zip_t<list1_t, list2_t>;
    static_assert(std::tuple_size_v<zipped_t> == 0);
}

TEST_F(valuelist_fixture, zip_empty_nonempty)
{
    using list1_t = valuelist<char, 'c', 'a'>;
    using list2_t = valuelist<char, 'a', 'd'>;
    using zipped_t = zip_t<list1_t, list2_t>;
    static_assert(std::tuple_size_v<zipped_t> == 0);
}

} // namespace utils
} // namespace core
} // namespace docgen
