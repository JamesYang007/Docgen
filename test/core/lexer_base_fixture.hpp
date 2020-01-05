#pragma once
#include "core/lexer_routines.hpp"
#include <gtest/gtest.h>

namespace docgen {
namespace core {

struct lexer_base_fixture : ::testing::Test
{
protected:

    static constexpr const char* filename = ".lexer_routines_unittest.data.txt";
    static constexpr size_t buf_size = 20;

    void write_file(const char* content)
    {
        FILE* fp = fopen(filename, "w");
        fwrite(content, sizeof(char), strlen(content), fp);
        fclose(fp);
    }

    void check_token(DocSymbol actual_sym, DocSymbol expected_sym,
                     const std::string& actual_str, const std::string& expected_str)
    {
        EXPECT_EQ(actual_sym, expected_sym);
        EXPECT_EQ(actual_str, expected_str);
    }
};

} // namespace core
} // namespace docgen
