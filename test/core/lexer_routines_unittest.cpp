#include "lexer_base_fixture.hpp"

namespace docgen {
namespace core {

struct lexer_routines_fixture : lexer_base_fixture
{};

TEST_F(lexer_routines_fixture, line_comment)
{
    static constexpr const char* content =
        "very special comment \n"
        ;
    static constexpr const char* expected_str =
        "very special comment ";

    write_file(content);
    file_reader reader(filename);
    token_t token = line_comment(reader);
    check_token(token.name, DocSymbol::line_comment,
                token.content, expected_str);
}

TEST_F(lexer_routines_fixture, block_comment)
{
    static constexpr const char* content =
        "very special comment  */"
        ;
    static constexpr const char* expected_str =
        "very special comment  ";

    write_file(content);
    file_reader reader(filename);
    token_t token = block_comment(reader);
    check_token(token.name, DocSymbol::block_comment,
                token.content, expected_str);
}

TEST_F(lexer_routines_fixture, process_line_comment)
{
    static constexpr const char* content =
        "#include <gtest/gtest.h>\n"
        "\n"
        "  // just a normal comment\n"
        "   /// a very special comment   \n"
        "\n"
        ;
    static constexpr const char* expected_str =
        " a very special comment   ";

    write_file(content);
    file_reader reader(filename);
    token_t token = process(reader);
    check_token(token.name, DocSymbol::line_comment,
                token.content, expected_str);
}

TEST_F(lexer_routines_fixture, process_block_comment)
{
    static constexpr const char* content =
        "#include <gtest/gtest.h>\n"
        "\n"
        "  // just a normal comment\n"
        "   /*!  a very special comment   \n"
        "   * another special comment  \n"
        "*/"
        "\n"
        ;
    static constexpr const char* expected_str =
        "  a very special comment   \n"
        "   * another special comment  \n"
        ;

    write_file(content);
    file_reader reader(filename);
    token_t token = process(reader);
    check_token(token.name, DocSymbol::block_comment,
                token.content, expected_str);
}

} // namespace core
} // namespace docgen
