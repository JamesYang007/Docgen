#include "core/lexer.hpp"
#include "lexer_base_fixture.hpp"

namespace docgen {
namespace core {

struct lexer_fixture : lexer_base_fixture
{
protected:
};

TEST_F(lexer_fixture, process_no_comment)
{
    static constexpr const char* content =
        "#include <gtest/gtest.h>\n"
        "\n"
        "  // just a normal comment\n"
        "\n"
        ;

    write_file(content);
    Lexer lexer(filename);
    lexer.process();
    const auto& tokens = lexer.get_tokens();

    EXPECT_EQ(tokens.size(), static_cast<size_t>(1));

    check_token(tokens[0].name, DocSymbol::eof,
                tokens[0].content, "");
}

TEST_F(lexer_fixture, process_one_line_comment)
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
    Lexer lexer(filename);
    lexer.process();
    const auto& tokens = lexer.get_tokens();

    EXPECT_EQ(tokens.size(), static_cast<size_t>(2));

    check_token(tokens[0].name, DocSymbol::line_comment,
                tokens[0].content, expected_str);

    check_token(tokens[1].name, DocSymbol::eof,
                tokens[1].content, "");
}

TEST_F(lexer_fixture, process_two_line_comment)
{
    static constexpr const char* content =
        "#include <gtest/gtest.h>\n"
        "\n"
        "  // just a normal comment\n"
        "   /// a very special comment   \n"
        "\n"
        "  // just a normal comment\n"
        "   /// another very special comment   \n"
        "  // just a normal comment\n"
        ;
    static constexpr const char* expected_str_1 =
        " a very special comment   ";
    static constexpr const char* expected_str_2 =
        " another very special comment   ";

    write_file(content);
    Lexer lexer(filename);
    lexer.process();
    const auto& tokens = lexer.get_tokens();

    EXPECT_EQ(tokens.size(), static_cast<size_t>(3));

    check_token(tokens[0].name, DocSymbol::line_comment,
                tokens[0].content, expected_str_1);

    check_token(tokens[1].name, DocSymbol::line_comment,
                tokens[1].content, expected_str_2);

    check_token(tokens[2].name, DocSymbol::eof,
                tokens[2].content, "");
}

TEST_F(lexer_fixture, process_one_block_comment)
{
    static constexpr const char* content =
        "#include <gtest/gtest.h>\n"
        "\n"
        "  // just a normal comment\n"
        "   /*! a very special comment   */\n"
        "\n"
        ;
    static constexpr const char* expected_str =
        " a very special comment   ";

    write_file(content);
    Lexer lexer(filename);
    lexer.process();
    const auto& tokens = lexer.get_tokens();

    EXPECT_EQ(tokens.size(), static_cast<size_t>(2));

    check_token(tokens[0].name, DocSymbol::block_comment,
                tokens[0].content, expected_str);

    check_token(tokens[1].name, DocSymbol::eof,
                tokens[1].content, "");
}

TEST_F(lexer_fixture, process_two_block_comment)
{
    static constexpr const char* content =
        "#include <gtest/gtest.h>\n"
        "\n"
        "  // just a normal comment\n"
        "   /*! a very special comment   */\n"
        "\n"
        "  // just a normal comment\n"
        "   /*! another very \n"
        "    * special comment   \n"
        "*/"
        "  /* just a normal comment\n */"
        ;
    static constexpr const char* expected_str_1 =
        " a very special comment   ";
    static constexpr const char* expected_str_2 =
        " another very \n    * special comment   \n";

    write_file(content);
    Lexer lexer(filename);
    lexer.process();
    const auto& tokens = lexer.get_tokens();

    EXPECT_EQ(tokens.size(), static_cast<size_t>(3));

    check_token(tokens[0].name, DocSymbol::block_comment,
                tokens[0].content, expected_str_1);

    check_token(tokens[1].name, DocSymbol::block_comment,
                tokens[1].content, expected_str_2);

    check_token(tokens[2].name, DocSymbol::eof,
                tokens[2].content, "");
}

TEST_F(lexer_fixture, process_line_block_comment)
{
    static constexpr const char* content =
        "#include <gtest/gtest.h>\n"
        "\n"
        "  // just a normal comment\n"
        "   /// a very special comment   */\n"
        "\n"
        "  // just a normal comment\n"
        "   /*! another very \n"
        "    * special comment   \n"
        "*/"
        "  /* just a normal comment\n */"
        ;
    static constexpr const char* expected_str_1 =
        " a very special comment   */";
    static constexpr const char* expected_str_2 =
        " another very \n    * special comment   \n";

    write_file(content);
    Lexer lexer(filename);
    lexer.process();
    const auto& tokens = lexer.get_tokens();

    EXPECT_EQ(tokens.size(), static_cast<size_t>(3));

    check_token(tokens[0].name, DocSymbol::line_comment,
                tokens[0].content, expected_str_1);

    check_token(tokens[1].name, DocSymbol::block_comment,
                tokens[1].content, expected_str_2);

    check_token(tokens[2].name, DocSymbol::eof,
                tokens[2].content, "");
}

} // namespace core
} // namespace docgen
