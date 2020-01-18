#define private public

#include <core/trie.hpp>
#include <iostream>
#include <gtest/gtest.h>

namespace docgen {
namespace core {

enum class MockSymbol {
    symbol1,
    symbol2
};

struct trie_fixture : ::testing::Test
{
protected:
    using symbol_t = MockSymbol;
    using trie_t = Trie<symbol_t>;

    void print_trie(const trie_t& trie)
    {
        print_trie(trie.root_); 
    }

    void print_trie(const trie_t::TrieNode& node)
    {
        if (node.is_accept()) {
            std::cout << "symbol: " << (int) *node.get_symbol() << std::endl;
        }
        std::cout << "\nsize: " << node.children_.size() << std::endl;
        for (auto it = node.children_.begin(); it != node.children_.end(); ++it) {
            std::cout << it->first << "--" << std::endl;;
            print_trie(it->second);
            std::cout << "--" << std::endl;
        }
    }
};

TEST_F(trie_fixture, trie_ctor)
{
    trie_t trie({
        {"adf", symbol_t::symbol1},
        {"asdf", symbol_t::symbol2},
        {"bscdf", symbol_t::symbol1}
    });

    auto symbol = trie.get_symbol();
    EXPECT_FALSE((bool) symbol);

    //trie.transition('a');
    //EXPECT_FALSE((bool) trie.get_symbol());
    //trie.transition('d');
    //EXPECT_FALSE((bool) trie.get_symbol());
    //trie.transition('b');
    //EXPECT_FALSE((bool) trie.get_symbol());

    trie.transition('a');
    EXPECT_FALSE((bool) trie.get_symbol());
    trie.transition('d');
    EXPECT_FALSE((bool) trie.get_symbol());
    trie.transition('f');
    EXPECT_TRUE((bool) trie.get_symbol());

    trie.back_transition();
    trie.back_transition();
    trie.back_transition();

    trie.transition('a');
    EXPECT_FALSE((bool) trie.get_symbol());
    trie.transition('d');
    EXPECT_FALSE((bool) trie.get_symbol());
    trie.transition('f');
    EXPECT_TRUE((bool) trie.get_symbol());

    trie.reset();

    trie.transition('a');
    EXPECT_FALSE((bool) trie.get_symbol());
    trie.transition('s');
    EXPECT_FALSE((bool) trie.get_symbol());
    trie.transition('d');
    EXPECT_FALSE((bool) trie.get_symbol());
    trie.transition('f');
    EXPECT_TRUE((bool) trie.get_symbol());

    trie.reset();

    trie.transition('b');
    EXPECT_FALSE((bool) trie.get_symbol());
    trie.transition('s');
    EXPECT_FALSE((bool) trie.get_symbol());
    trie.transition('c');
    EXPECT_FALSE((bool) trie.get_symbol());
    trie.transition('d');
    EXPECT_FALSE((bool) trie.get_symbol());
    trie.transition('f');
    EXPECT_TRUE((bool) trie.get_symbol());
}

} // namespace core
} // namespace docgen
