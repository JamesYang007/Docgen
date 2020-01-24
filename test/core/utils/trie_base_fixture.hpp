#pragma once
#include <gtest/gtest.h>

namespace docgen {
namespace core {
namespace utils {

enum class MockSymbol {
    symbol_0,
    symbol_1
};

struct trie_base_fixture : ::testing::Test
{
protected:
    using symbol_t = MockSymbol;
};

} // namespace utils
} // namespace core
} // namespace docgen
