#include <core/lex/trie.hpp>
#include <gtest/gtest.h>

namespace docgen {
namespace core {
namespace lex {

enum class MockSymbol {
    symbol_0,
    symbol_1,
    symbol_2,
    symbol_3,
};

struct trie_fixture : ::testing::Test
{
protected:
    using symbol_t = MockSymbol;
    using trie_t = Trie<symbol_t>;
};

////////////////////////////////////////////
// State TESTS
////////////////////////////////////////////

TEST_F(trie_fixture, trie_root)
{
    trie_t trie({
        {"a", symbol_t::symbol_0},
        {"abc", symbol_t::symbol_1},
        {"ac", symbol_t::symbol_2},
    });

    EXPECT_TRUE(!trie.get_symbol());
    EXPECT_TRUE(!trie.is_accept());
    EXPECT_TRUE(trie.is_reset());

    EXPECT_EQ(trie.get_children().size(), static_cast<size_t>(1));
    EXPECT_NE(trie.get_children().find('a'), trie.get_children().end());  // found
}

TEST_F(trie_fixture, trie_transition_child_a) 
{
    trie_t trie({
        {"a", symbol_t::symbol_0},
        {"abc", symbol_t::symbol_1},
        {"ac", symbol_t::symbol_2},
    });

    trie.transition('a');

    EXPECT_TRUE(trie.get_symbol());
    EXPECT_EQ(*trie.get_symbol(), symbol_t::symbol_0);
    EXPECT_TRUE(trie.is_accept());
    EXPECT_TRUE(!trie.is_reset());

    EXPECT_EQ(trie.get_children().size(), static_cast<size_t>(2));
    EXPECT_NE(trie.get_children().find('b'), trie.get_children().end());  // found
    EXPECT_NE(trie.get_children().find('c'), trie.get_children().end());  // found
}

TEST_F(trie_fixture, trie_transition_child_b)
{
    trie_t trie({
        {"a", symbol_t::symbol_0},
        {"abc", symbol_t::symbol_1},
        {"ac", symbol_t::symbol_2},
    });

    trie.transition('a');
    trie.transition('b');

    EXPECT_TRUE(!trie.get_symbol());
    EXPECT_TRUE(!trie.is_accept());
    EXPECT_TRUE(!trie.is_reset());

    EXPECT_EQ(trie.get_children().size(), static_cast<size_t>(1));
    EXPECT_NE(trie.get_children().find('c'), trie.get_children().end());  // found
}

TEST_F(trie_fixture, trie_transition_child_bc)
{
    trie_t trie({
        {"a", symbol_t::symbol_0},
        {"abc", symbol_t::symbol_1},
        {"ac", symbol_t::symbol_2},
    });

    trie.transition('a');
    trie.transition('b');
    trie.transition('c');

    EXPECT_TRUE(trie.get_symbol());
    EXPECT_EQ(*trie.get_symbol(), symbol_t::symbol_1);
    EXPECT_TRUE(trie.is_accept());
    EXPECT_TRUE(!trie.is_reset());

    EXPECT_EQ(trie.get_children().size(), static_cast<size_t>(0));
}

TEST_F(trie_fixture, trie_transition_child_c)
{
    trie_t trie({
        {"a", symbol_t::symbol_0},
        {"abc", symbol_t::symbol_1},
        {"ac", symbol_t::symbol_2},
    });

    trie.transition('a');
    trie.transition('c');

    EXPECT_TRUE(trie.get_symbol());
    EXPECT_EQ(*trie.get_symbol(), symbol_t::symbol_2);
    EXPECT_TRUE(trie.is_accept());
    EXPECT_TRUE(!trie.is_reset());

    EXPECT_EQ(trie.get_children().size(), static_cast<size_t>(0));
}

TEST_F(trie_fixture, trie_reset_root)
{
    trie_t trie({
        {"a", symbol_t::symbol_0},
        {"abc", symbol_t::symbol_1},
        {"ac", symbol_t::symbol_2},
    });

    trie.reset();

    EXPECT_TRUE(!trie.get_symbol());
    EXPECT_TRUE(!trie.is_accept());
    EXPECT_TRUE(trie.is_reset());

    EXPECT_EQ(trie.get_children().size(), static_cast<size_t>(1));
    EXPECT_NE(trie.get_children().find('a'), trie.get_children().end());  // found
}

TEST_F(trie_fixture, trie_reset_child_a)
{
    trie_t trie({
        {"a", symbol_t::symbol_0},
        {"abc", symbol_t::symbol_1},
        {"ac", symbol_t::symbol_2},
    });

    trie.transition('a');

    trie.reset();

    EXPECT_TRUE(!trie.get_symbol());
    EXPECT_TRUE(!trie.is_accept());
    EXPECT_TRUE(trie.is_reset());

    EXPECT_EQ(trie.get_children().size(), static_cast<size_t>(1));
    EXPECT_NE(trie.get_children().find('a'), trie.get_children().end());  // found
}

TEST_F(trie_fixture, trie_reset_child_a_b)
{
    trie_t trie({
        {"a", symbol_t::symbol_0},
        {"abc", symbol_t::symbol_1},
        {"ac", symbol_t::symbol_2},
    });

    trie.transition('a');
    trie.transition('b');

    trie.reset();

    EXPECT_TRUE(!trie.get_symbol());
    EXPECT_TRUE(!trie.is_accept());
    EXPECT_TRUE(trie.is_reset());

    EXPECT_EQ(trie.get_children().size(), static_cast<size_t>(1));
    EXPECT_NE(trie.get_children().find('a'), trie.get_children().end());  // found
}

TEST_F(trie_fixture, trie_back_transition_root)
{
    trie_t trie({
        {"a", symbol_t::symbol_0},
        {"abc", symbol_t::symbol_1},
        {"ac", symbol_t::symbol_2},
    });

    EXPECT_THROW(trie.back_transition(), exceptions::control_flow_error);
}

TEST_F(trie_fixture, trie_back_transition_child_a)
{
    trie_t trie({
        {"a", symbol_t::symbol_0},
        {"abc", symbol_t::symbol_1},
        {"ac", symbol_t::symbol_2},
    });

    trie.transition('a');
    trie.back_transition();

    EXPECT_TRUE(!trie.get_symbol());
    EXPECT_TRUE(!trie.is_accept());
    EXPECT_TRUE(trie.is_reset());

    EXPECT_EQ(trie.get_children().size(), static_cast<size_t>(1));
    EXPECT_NE(trie.get_children().find('a'), trie.get_children().end());  // found
}

TEST_F(trie_fixture, trie_back_transition_child_ab)
{
    trie_t trie({
        {"a", symbol_t::symbol_0},
        {"abc", symbol_t::symbol_1},
        {"ac", symbol_t::symbol_2},
    });

    trie.transition('a');
    trie.transition('b');

    // back to child 'a'
    trie.back_transition();

    EXPECT_TRUE(trie.get_symbol());
    EXPECT_TRUE(trie.is_accept());
    EXPECT_TRUE(!trie.is_reset());

    EXPECT_EQ(trie.get_children().size(), static_cast<size_t>(2));
    EXPECT_NE(trie.get_children().find('b'), trie.get_children().end());  // found
    EXPECT_NE(trie.get_children().find('c'), trie.get_children().end());  // found

    // back to root
    trie.back_transition();

    EXPECT_TRUE(!trie.get_symbol());
    EXPECT_TRUE(!trie.is_accept());
    EXPECT_TRUE(trie.is_reset());

    EXPECT_EQ(trie.get_children().size(), static_cast<size_t>(1));
    EXPECT_NE(trie.get_children().find('a'), trie.get_children().end());  // found
}

////////////////////////////////////////////
// Structural Checks
////////////////////////////////////////////

TEST_F(trie_fixture, trie_off_by_one_prefix)
{
    trie_t trie({
        {"ab", symbol_t::symbol_1},
        {"bab", symbol_t::symbol_1},
    });

    // check root
    EXPECT_TRUE(!trie.get_symbol());
    EXPECT_TRUE(!trie.is_accept());
    EXPECT_TRUE(trie.is_reset());

    EXPECT_EQ(trie.get_children().size(), static_cast<size_t>(2));
    EXPECT_NE(trie.get_children().find('a'), trie.get_children().end());  // found
    EXPECT_NE(trie.get_children().find('b'), trie.get_children().end());  // found

    // check child 'a'
    trie.transition('a');

    EXPECT_TRUE(!trie.get_symbol());
    EXPECT_TRUE(!trie.is_accept());
    EXPECT_TRUE(!trie.is_reset());

    EXPECT_EQ(trie.get_children().size(), static_cast<size_t>(1));
    EXPECT_NE(trie.get_children().find('b'), trie.get_children().end());  // found

    // check child 'a'->'b'
    trie.transition('b');

    EXPECT_TRUE(trie.get_symbol());
    EXPECT_TRUE(trie.is_accept());
    EXPECT_TRUE(!trie.is_reset());

    EXPECT_EQ(trie.get_children().size(), static_cast<size_t>(0));

    // up to child 'a'
    trie.back_transition();

    // up to root
    trie.back_transition();

    // check child 'b'
    trie.transition('b');

    EXPECT_TRUE(!trie.get_symbol());
    EXPECT_TRUE(!trie.is_accept());
    EXPECT_TRUE(!trie.is_reset());

    EXPECT_EQ(trie.get_children().size(), static_cast<size_t>(1));
    EXPECT_NE(trie.get_children().find('a'), trie.get_children().end());  // found

    // check child 'b'->'a'
    trie.transition('a');

    EXPECT_TRUE(!trie.get_symbol());
    EXPECT_TRUE(!trie.is_accept());
    EXPECT_TRUE(!trie.is_reset());

    EXPECT_EQ(trie.get_children().size(), static_cast<size_t>(1));
    EXPECT_NE(trie.get_children().find('b'), trie.get_children().end());  // found

    // check child 'b'->'a'->'b'
    trie.transition('b');

    EXPECT_TRUE(trie.get_symbol());
    EXPECT_TRUE(trie.is_accept());
    EXPECT_TRUE(!trie.is_reset());

    EXPECT_EQ(trie.get_children().size(), static_cast<size_t>(0));
}

} // namespace lex
} // namespace core
} // namespace docgen
