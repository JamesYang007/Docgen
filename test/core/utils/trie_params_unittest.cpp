#include <core/utils/trie_params.hpp>
#include <core/utils/trie_base_fixture.hpp>

namespace docgen {
namespace core {
namespace utils {

struct trie_params_fixture : trie_base_fixture
{};

TEST_F(trie_params_fixture, has_empty_valuelist_true)
{
    using list1_t = valuelist<int, 0, 1>;
    using list2_t = valuelist<char>;
    using list_t = typelist<list1_t, list2_t>;
    static_assert(has_empty_valuelist_v<list_t>);
}

TEST_F(trie_params_fixture, trie_params_depth_1)
{
    using list_t = typelist<
        typelist<valuelist<char, 'a'>, std::integral_constant<symbol_t, symbol_t::symbol_0>>,
        typelist<valuelist<char, 'b'>, std::integral_constant<symbol_t, symbol_t::symbol_1>>
    >;
    using param_t = trie_params_t<list_t>;

    using first_t = get_t<param_t, 0>;  // unique set of chars
    using second_t = get_t<param_t, 1>; // children information

    // check unique char set
    static_assert(size_v<first_t> == 2);
    static_assert(get_v<first_t, 0> == 'a');
    static_assert(get_v<first_t, 1> == 'b');

    static_assert(std::is_same_v<
            // first child's first information (trie param)
            get_t<get_t<second_t, 0>, 0>,
            utils::leaf_tag
            >);

    static_assert(
            // first child's second information (bool indicating accept state)
            get_t<get_t<second_t, 0>, 1>::value 
            ==
            true
            );

    static_assert(
            // first child's third information (symbol)
            get_t<get_t<second_t, 0>, 2>::value 
            ==
            symbol_t::symbol_0
            );

    static_assert(std::is_same_v<
            get_t<get_t<second_t, 1>, 0>,
            utils::leaf_tag
            >);

    static_assert(
            get_t<get_t<second_t, 1>, 1>::value
            ==
            true
            );

    static_assert(
            get_t<get_t<second_t, 1>, 2>::value
            ==
            symbol_t::symbol_1
            );
}

TEST_F(trie_params_fixture, trie_params_depth_2)
{
    using list_t = typelist<
        typelist<valuelist<char, 'a'>, std::integral_constant<symbol_t, symbol_t::symbol_0>>,
        typelist<valuelist<char, 'a', 'b'>, std::integral_constant<symbol_t, symbol_t::symbol_1>>,
        typelist<valuelist<char, 'b', 'a'>, std::integral_constant<symbol_t, symbol_t::symbol_1>>
    >;
    using param_t = trie_params_t<list_t>;

    using first_t = get_t<param_t, 0>;  // unique set of chars
    using second_t = get_t<param_t, 1>; // children information

    // check unique char set
    static_assert(size_v<first_t> == 2);
    static_assert(get_v<first_t, 0> == 'a');
    static_assert(get_v<first_t, 1> == 'b');

    // first child's second information (bool indicating accept state)
    static_assert(get_t<get_t<second_t, 0>, 1>::value);

    // first child's third information (symbol)
    static_assert(get_t<get_t<second_t, 0>, 2>::value == symbol_t::symbol_0);

    static_assert(!get_t<get_t<second_t, 1>, 1>::value);

    ////////////////////////////////////////////////////////////////
    // check first child
    ////////////////////////////////////////////////////////////////
    
    // check its first (and only) child
    using child_1_param_t = get_t<get_t<second_t, 0>, 0>;
    using first_child_1_param_t = get_t<child_1_param_t, 0>;
    using second_child_1_param_t = get_t<child_1_param_t, 1>;

    static_assert(size_v<first_child_1_param_t> == 1);
    static_assert(get_v<first_child_1_param_t, 0> == 'b');

    static_assert(std::is_same_v<
            get_t<get_t<second_child_1_param_t, 0>, 0>,
            utils::leaf_tag
            >);

    static_assert(get_t<get_t<second_child_1_param_t, 0>, 1>::value);

    static_assert(get_t<get_t<second_child_1_param_t, 0>, 2>::value == symbol_t::symbol_1);

    ////////////////////////////////////////////////////////////////
    // check second child
    ////////////////////////////////////////////////////////////////

    using child_2_param_t = get_t<get_t<second_t, 1>, 0>;
    using first_child_2_param_t = get_t<child_2_param_t, 0>;
    using second_child_2_param_t = get_t<child_2_param_t, 1>;

    static_assert(size_v<first_child_2_param_t> == 1);
    static_assert(get_v<first_child_2_param_t, 0> == 'a');

    static_assert(std::is_same_v<
            get_t<get_t<second_child_2_param_t, 0>, 0>,
            utils::leaf_tag
            >);

    static_assert(get_t<get_t<second_child_2_param_t, 0>, 1>::value);

    static_assert(get_t<get_t<second_child_2_param_t, 0>, 2>::value == symbol_t::symbol_1);
}
    
} // namespace utils
} // namespace core
} // namespace docgen
