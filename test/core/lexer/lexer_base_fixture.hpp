#pragma once
#include <gtest/gtest.h>

namespace docgen {
namespace core {

struct lexer_base_fixture : ::testing::Test
{
protected:
    using status_t = status_t;
    using token_t = lexer_details::token_t;
    using symbol_t = lexer_details::symbol_t;

    static constexpr const char* filename = ".lexer_routines_unittest.data.txt";
    static constexpr size_t buf_size = 20;
    FILE* file;

    lexer_base_fixture()
        : file(fopen(filename, "r"))
    {}

    ~lexer_base_fixture()
    {
        fclose(file);
    }

    void write_file(const char* content)
    {
        FILE* fp = fopen(filename, "w");
        fwrite(content, sizeof(char), strlen(content), fp);
        fclose(fp);
    }
};

} // namespace core
} // namespace docgen
