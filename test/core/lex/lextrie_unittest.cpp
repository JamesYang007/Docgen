#include <core/lex/lextrie.hpp>
#include <core/utils/trie_base_fixture.hpp>

namespace docgen {
namespace core {
namespace utils {

struct lextrie_fixture : trie_base_fixture
{};

TEST_F(lextrie_fixture, is_accept)
{
    using namespace std::literals;
    static constexpr std::string_view str1 = "a"sv;
    static constexpr std::string_view str2 = "ab"sv;
    static constexpr std::string_view str3 = "ba"sv;
    using list_t = typelist<
        typelist<sv_to_valuelist_t<str1>, std::integral_constant<symbol_t, symbol_t::symbol_0>>,
        typelist<sv_to_valuelist_t<str2>, std::integral_constant<symbol_t, symbol_t::symbol_1>>,
        typelist<sv_to_valuelist_t<str3>, std::integral_constant<symbol_t, symbol_t::symbol_1>>
    >;
    using param_t = trie_params_t<list_t>;
    LexTrie<param_t> trie;

    EXPECT_TRUE(!trie.is_accept());
}

TEST_F(lextrie_fixture, transition_depth_1)
{
    using namespace std::literals;
    static constexpr std::string_view str1 = "a"sv;
    static constexpr std::string_view str2 = "ab"sv;
    static constexpr std::string_view str3 = "ba"sv;
    using list_t = typelist<
        typelist<sv_to_valuelist_t<str1>, std::integral_constant<symbol_t, symbol_t::symbol_0>>,
        typelist<sv_to_valuelist_t<str2>, std::integral_constant<symbol_t, symbol_t::symbol_1>>,
        typelist<sv_to_valuelist_t<str3>, std::integral_constant<symbol_t, symbol_t::symbol_1>>
    >;
    using param_t = trie_params_t<list_t>;
    LexTrie<param_t> trie;

    EXPECT_TRUE(trie.transition('a', [](){})); // transition successful
    EXPECT_TRUE(trie.is_accept());
}

TEST_F(lextrie_fixture, transition_depth_2)
{
    using namespace std::literals;
    static constexpr std::string_view str1 = "a"sv;
    static constexpr std::string_view str2 = "ab"sv;
    static constexpr std::string_view str3 = "ba"sv;
    using list_t = typelist<
        typelist<sv_to_valuelist_t<str1>, std::integral_constant<symbol_t, symbol_t::symbol_0>>,
        typelist<sv_to_valuelist_t<str2>, std::integral_constant<symbol_t, symbol_t::symbol_1>>,
        typelist<sv_to_valuelist_t<str3>, std::integral_constant<symbol_t, symbol_t::symbol_1>>
    >;
    using param_t = trie_params_t<list_t>;
    LexTrie<param_t> trie;

    EXPECT_TRUE(trie.transition('a', [](){})); // transition successful
    EXPECT_TRUE(trie.transition('b', [](){})); // transition successful
    EXPECT_TRUE(trie.is_accept());
}

TEST_F(lextrie_fixture, back_transition_once)
{
    using namespace std::literals;
    static constexpr std::string_view str1 = "a"sv;
    static constexpr std::string_view str2 = "ab"sv;
    static constexpr std::string_view str3 = "ba"sv;
    using list_t = typelist<
        typelist<sv_to_valuelist_t<str1>, std::integral_constant<symbol_t, symbol_t::symbol_0>>,
        typelist<sv_to_valuelist_t<str2>, std::integral_constant<symbol_t, symbol_t::symbol_1>>,
        typelist<sv_to_valuelist_t<str3>, std::integral_constant<symbol_t, symbol_t::symbol_1>>
    >;
    using param_t = trie_params_t<list_t>;
    LexTrie<param_t> trie;

    EXPECT_TRUE(trie.transition('a', [](){})); // transition successful
    EXPECT_TRUE(trie.is_accept());
    EXPECT_EQ(trie.back_transition(), static_cast<size_t>(1)); 
    EXPECT_TRUE(!trie.is_accept());
}

TEST_F(lextrie_fixture, back_transition_twice)
{
    using namespace std::literals;
    static constexpr std::string_view str1 = "a"sv;
    static constexpr std::string_view str2 = "ab"sv;
    static constexpr std::string_view str3 = "ba"sv;
    using list_t = typelist<
        typelist<sv_to_valuelist_t<str1>, std::integral_constant<symbol_t, symbol_t::symbol_0>>,
        typelist<sv_to_valuelist_t<str2>, std::integral_constant<symbol_t, symbol_t::symbol_1>>,
        typelist<sv_to_valuelist_t<str3>, std::integral_constant<symbol_t, symbol_t::symbol_1>>
    >;
    using param_t = trie_params_t<list_t>;
    LexTrie<param_t> trie;

    EXPECT_TRUE(!trie.is_accept());
    EXPECT_TRUE(trie.transition('a', [](){})); // transition successful
    EXPECT_TRUE(trie.is_accept());
    EXPECT_TRUE(trie.transition('b', [](){})); // transition successful
    EXPECT_TRUE(trie.is_accept());
    EXPECT_EQ(trie.back_transition(2), static_cast<size_t>(2)); 
    EXPECT_TRUE(!trie.is_accept());
}

TEST_F(lextrie_fixture, is_reset_true)
{
    using namespace std::literals;
    static constexpr std::string_view str1 = "ada"sv;
    static constexpr std::string_view str2 = "abd"sv;
    static constexpr std::string_view str3 = "bacc"sv;
    static constexpr std::string_view str4 = "adbc"sv;

    using list_t = typelist<
        typelist<sv_to_valuelist_t<str1>, std::integral_constant<symbol_t, symbol_t::symbol_0>>,
        typelist<sv_to_valuelist_t<str2>, std::integral_constant<symbol_t, symbol_t::symbol_1>>,
        typelist<sv_to_valuelist_t<str3>, std::integral_constant<symbol_t, symbol_t::symbol_1>>,
        typelist<sv_to_valuelist_t<str4>, std::integral_constant<symbol_t, symbol_t::symbol_0>>
    >;

    using param_t = trie_params_t<list_t>;
    LexTrie<param_t> trie;

    EXPECT_TRUE(trie.is_reset());
    EXPECT_TRUE(trie.transition('a', [](){})); // transition successful
    EXPECT_TRUE(!trie.is_reset());
    EXPECT_TRUE(trie.transition('d', [](){})); // transition successful
    EXPECT_TRUE(!trie.is_reset());
    EXPECT_TRUE(trie.transition('a', [](){})); // transition successful
    EXPECT_TRUE(!trie.is_reset());
    EXPECT_EQ(trie.back_transition(3), static_cast<size_t>(3));
    EXPECT_TRUE(trie.is_reset());
}

TEST_F(lextrie_fixture, reset)
{
    using namespace std::literals;
    static constexpr std::string_view str1 = "ada"sv;
    static constexpr std::string_view str2 = "abd"sv;
    static constexpr std::string_view str3 = "bacc"sv;
    static constexpr std::string_view str4 = "adbc"sv;

    using list_t = typelist<
        typelist<sv_to_valuelist_t<str1>, std::integral_constant<symbol_t, symbol_t::symbol_0>>,
        typelist<sv_to_valuelist_t<str2>, std::integral_constant<symbol_t, symbol_t::symbol_1>>,
        typelist<sv_to_valuelist_t<str3>, std::integral_constant<symbol_t, symbol_t::symbol_1>>,
        typelist<sv_to_valuelist_t<str4>, std::integral_constant<symbol_t, symbol_t::symbol_0>>
    >;

    using param_t = trie_params_t<list_t>;
    LexTrie<param_t> trie;
    
    EXPECT_TRUE(trie.transition('a', [](){})); // transition successful
    EXPECT_TRUE(trie.transition('b', [](){})); // transition successful
    trie.reset(); 
    EXPECT_TRUE(trie.is_reset());
    EXPECT_TRUE(trie.transition('a', [](){})); // transition successful
    EXPECT_TRUE(trie.transition('b', [](){})); // transition successful
}

TEST_F(lextrie_fixture, get_symbol)
{
    using namespace std::literals;
    static constexpr std::string_view str1 = "ada"sv;
    static constexpr std::string_view str2 = "abd"sv;
    static constexpr std::string_view str3 = "bacc"sv;
    static constexpr std::string_view str4 = "adbc"sv;

    using list_t = typelist<
        typelist<sv_to_valuelist_t<str1>, std::integral_constant<symbol_t, symbol_t::symbol_0>>,
        typelist<sv_to_valuelist_t<str2>, std::integral_constant<symbol_t, symbol_t::symbol_1>>,
        typelist<sv_to_valuelist_t<str3>, std::integral_constant<symbol_t, symbol_t::symbol_1>>,
        typelist<sv_to_valuelist_t<str4>, std::integral_constant<symbol_t, symbol_t::symbol_0>>
    >;

    using param_t = trie_params_t<list_t>;
    LexTrie<param_t> trie;
    
    EXPECT_TRUE(trie.transition('a', [](){})); // transition successful
    EXPECT_TRUE(trie.transition('d', [](){})); // transition successful
    EXPECT_TRUE(trie.transition('a', [](){})); // transition successful
    EXPECT_EQ(*(trie.get_symbol()), symbol_t::symbol_0);

    trie.back_transition();
    EXPECT_TRUE(trie.transition('b', [](){})); // transition successful
    EXPECT_TRUE(trie.transition('c', [](){})); // transition successful
    EXPECT_EQ(*trie.get_symbol(), symbol_t::symbol_0);

    trie.back_transition(3);
    EXPECT_TRUE(trie.transition('b', [](){})); // transition successful
    EXPECT_TRUE(trie.transition('d', [](){})); // transition successful
    EXPECT_EQ(*trie.get_symbol(), symbol_t::symbol_1);

    trie.back_transition(3);
    EXPECT_TRUE(trie.transition('b', [](){})); // transition successful
    EXPECT_TRUE(trie.transition('a', [](){})); // transition successful
    EXPECT_TRUE(trie.transition('c', [](){})); // transition successful
    EXPECT_TRUE(trie.transition('c', [](){})); // transition successful
    EXPECT_EQ(*trie.get_symbol(), symbol_t::symbol_1);
}

} // namespace utils
} // namespace core
} // namespace docgen
