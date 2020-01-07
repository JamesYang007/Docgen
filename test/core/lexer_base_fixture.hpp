#pragma once
#include "core/lexer_routines.hpp"
#include <gtest/gtest.h>

namespace docgen {
namespace core {

struct lexer_base_fixture : ::testing::Test
{
protected:
    using status_t = core::status_t;
    using token_t = core::token_t;
    using symbol_t = typename token_t::symbol_t;

    static constexpr const char* filename = ".lexer_routines_unittest.data.txt";
    static constexpr size_t buf_size = 20;

    void write_file(const char* content)
    {
        FILE* fp = fopen(filename, "w");
        fwrite(content, sizeof(char), strlen(content), fp);
        fclose(fp);
    }

    void check_token(symbol_t actual_sym, symbol_t expected_sym,
                     const std::string& actual_str, const std::string& expected_str)
    {
        EXPECT_EQ(actual_sym, expected_sym);
        EXPECT_EQ(actual_str, expected_str);
    }
};

} // namespace core
} // namespace docgen
