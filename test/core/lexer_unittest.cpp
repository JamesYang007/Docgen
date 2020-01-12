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

    EXPECT_EQ(tokens.size(), static_cast<size_t>(6));

    check_token(tokens[0].name, symbol_t::TEXT,
                tokens[0].content, "#include <gtest/gtest.h>");
    check_token(tokens[1].name, symbol_t::NEWLINE,
                tokens[1].content, "");
    check_token(tokens[2].name, symbol_t::NEWLINE,
                tokens[2].content, "");
    check_token(tokens[3].name, symbol_t::TEXT,
                tokens[3].content, "");
    EXPECT_EQ(tokens[3].leading_ws_count, static_cast<uint32_t>(2));
    check_token(tokens[4].name, symbol_t::NEWLINE,
                tokens[4].content, "");
    check_token(tokens[5].name, symbol_t::END_OF_FILE,
                tokens[5].content, "");
}

TEST_F(lexer_fixture, process_one_line_comment_empty)
{
    static constexpr const char* content =
        "  ///\n"
        ;

    write_file(content);
    Lexer lexer(filename);
    lexer.process();
    const auto& tokens = lexer.get_tokens();

    EXPECT_EQ(tokens.size(), static_cast<size_t>(4));

    check_token(tokens[0].name, symbol_t::TEXT,
                tokens[0].content, "");
    EXPECT_EQ(tokens[0].leading_ws_count, static_cast<uint32_t>(2));
    check_token(tokens[1].name, symbol_t::BEGIN_LINE_COMMENT,
                tokens[1].content, "");
    check_token(tokens[2].name, symbol_t::NEWLINE,
                tokens[2].content, "");
    check_token(tokens[3].name, symbol_t::END_OF_FILE,
                tokens[3].content, "");
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

    write_file(content);
    Lexer lexer(filename);
    lexer.process();
    const auto& tokens = lexer.get_tokens();

    EXPECT_EQ(tokens.size(), static_cast<size_t>(9));

    check_token(tokens[0].name, symbol_t::TEXT,
                tokens[0].content, "#include <gtest/gtest.h>");
    check_token(tokens[1].name, symbol_t::NEWLINE,
                tokens[1].content, "");
    check_token(tokens[2].name, symbol_t::NEWLINE,
                tokens[2].content, "");
    check_token(tokens[3].name, symbol_t::TEXT,
                tokens[3].content, "");
    EXPECT_EQ(tokens[3].leading_ws_count, static_cast<uint32_t>(5));

    check_token(tokens[4].name, symbol_t::BEGIN_LINE_COMMENT,
                tokens[4].content, "");
    check_token(tokens[5].name, symbol_t::TEXT,
                tokens[5].content, "a very special comment");
    check_token(tokens[6].name, symbol_t::NEWLINE,
                tokens[6].content, "");
    check_token(tokens[7].name, symbol_t::NEWLINE,
                tokens[7].content, "");
    check_token(tokens[8].name, symbol_t::END_OF_FILE,
                tokens[8].content, "");
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

    write_file(content);
    Lexer lexer(filename);
    lexer.process();
    const auto& tokens = lexer.get_tokens();

    EXPECT_EQ(tokens.size(), static_cast<size_t>(14));

    check_token(tokens[0].name, symbol_t::TEXT,
                tokens[0].content, "#include <gtest/gtest.h>");
    check_token(tokens[1].name, symbol_t::NEWLINE,
                tokens[1].content, "");
    check_token(tokens[2].name, symbol_t::NEWLINE,
                tokens[2].content, "");
    check_token(tokens[3].name, symbol_t::TEXT,
                tokens[3].content, "");
    EXPECT_EQ(tokens[3].leading_ws_count, static_cast<uint32_t>(5));

    check_token(tokens[4].name, symbol_t::BEGIN_LINE_COMMENT,
                tokens[4].content, "");
    check_token(tokens[5].name, symbol_t::TEXT,
                tokens[5].content, "a very special comment");
    EXPECT_EQ(tokens[5].leading_ws_count, static_cast<uint32_t>(1));

    check_token(tokens[6].name, symbol_t::NEWLINE,
                tokens[6].content, "");
    check_token(tokens[7].name, symbol_t::NEWLINE,
                tokens[7].content, "");
    check_token(tokens[8].name, symbol_t::TEXT,
                tokens[8].content, "");
    EXPECT_EQ(tokens[8].leading_ws_count, static_cast<uint32_t>(5));

    check_token(tokens[9].name, symbol_t::BEGIN_LINE_COMMENT,
                tokens[9].content, "");
    check_token(tokens[10].name, symbol_t::TEXT,
                tokens[10].content, "another very special comment");
    EXPECT_EQ(tokens[10].leading_ws_count, static_cast<uint32_t>(1));

    check_token(tokens[11].name, symbol_t::NEWLINE,
                tokens[11].content, "");
    check_token(tokens[12].name, symbol_t::TEXT,
                tokens[12].content, "");
    EXPECT_EQ(tokens[12].leading_ws_count, static_cast<uint32_t>(2));

    check_token(tokens[13].name, symbol_t::END_OF_FILE,
                tokens[13].content, "");
}

TEST_F(lexer_fixture, process_one_block_comment_empty)
{
    static constexpr const char* content =
        "   /*!\n"
        "   */\n"
        ;

    write_file(content);
    Lexer lexer(filename);
    lexer.process();
    const auto& tokens = lexer.get_tokens();

    EXPECT_EQ(tokens.size(), static_cast<size_t>(7));

    check_token(tokens[0].name, symbol_t::TEXT,
                tokens[0].content, "");
    EXPECT_EQ(tokens[0].leading_ws_count, static_cast<uint32_t>(3));
    check_token(tokens[1].name, symbol_t::BEGIN_BLOCK_COMMENT,
                tokens[1].content, "");
    check_token(tokens[2].name, symbol_t::NEWLINE,
                tokens[2].content, "");

    check_token(tokens[3].name, symbol_t::TEXT,
                tokens[3].content, "");
    EXPECT_EQ(tokens[3].leading_ws_count, static_cast<uint32_t>(3));
    check_token(tokens[4].name, symbol_t::END_BLOCK_COMMENT,
                tokens[4].content, "");
    check_token(tokens[5].name, symbol_t::NEWLINE,
                tokens[5].content, "");
    check_token(tokens[6].name, symbol_t::END_OF_FILE,
                tokens[6].content, "");
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

    write_file(content);
    Lexer lexer(filename);
    lexer.process();
    const auto& tokens = lexer.get_tokens();

    EXPECT_EQ(tokens.size(), static_cast<size_t>(10));

    check_token(tokens[0].name, symbol_t::TEXT,
                tokens[0].content, "#include <gtest/gtest.h>");
    check_token(tokens[1].name, symbol_t::NEWLINE,
                tokens[1].content, "");
    check_token(tokens[2].name, symbol_t::NEWLINE,
                tokens[2].content, "");

    check_token(tokens[3].name, symbol_t::TEXT,
                tokens[3].content, "");
    EXPECT_EQ(tokens[3].leading_ws_count, static_cast<uint32_t>(5));
    check_token(tokens[4].name, symbol_t::BEGIN_BLOCK_COMMENT,
                tokens[4].content, "");
    check_token(tokens[5].name, symbol_t::TEXT,
                tokens[5].content, "a very special comment");
    EXPECT_EQ(tokens[5].leading_ws_count, static_cast<uint32_t>(1));

    check_token(tokens[6].name, symbol_t::END_BLOCK_COMMENT,
                tokens[6].content, "");
    check_token(tokens[7].name, symbol_t::NEWLINE,
                tokens[7].content, "");
    check_token(tokens[8].name, symbol_t::NEWLINE,
                tokens[8].content, "");
    check_token(tokens[9].name, symbol_t::END_OF_FILE,
                tokens[9].content, "");
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

    write_file(content);
    Lexer lexer(filename);
    lexer.process();
    const auto& tokens = lexer.get_tokens();

    EXPECT_EQ(tokens.size(), static_cast<size_t>(20));

    check_token(tokens[0].name, symbol_t::TEXT,
                tokens[0].content, "#include <gtest/gtest.h>");
    check_token(tokens[1].name, symbol_t::NEWLINE,
                tokens[1].content, "");
    check_token(tokens[2].name, symbol_t::NEWLINE,
                tokens[2].content, "");

    check_token(tokens[3].name, symbol_t::TEXT,
                tokens[3].content, "");
    EXPECT_EQ(tokens[3].leading_ws_count, static_cast<uint32_t>(5));
    check_token(tokens[4].name, symbol_t::BEGIN_BLOCK_COMMENT,
                tokens[4].content, "");
    check_token(tokens[5].name, symbol_t::TEXT,
                tokens[5].content, "a very special comment");
    EXPECT_EQ(tokens[5].leading_ws_count, static_cast<uint32_t>(1));

    check_token(tokens[6].name, symbol_t::END_BLOCK_COMMENT,
                tokens[6].content, "");
    check_token(tokens[7].name, symbol_t::NEWLINE,
                tokens[7].content, "");
    check_token(tokens[8].name, symbol_t::NEWLINE,
                tokens[8].content, "");

    check_token(tokens[9].name, symbol_t::TEXT,
                tokens[9].content, "");
    EXPECT_EQ(tokens[9].leading_ws_count, static_cast<uint32_t>(5));
    check_token(tokens[10].name, symbol_t::BEGIN_BLOCK_COMMENT,
                tokens[10].content, "");
    check_token(tokens[11].name, symbol_t::TEXT,
                tokens[11].content, "another very");
    EXPECT_EQ(tokens[11].leading_ws_count, static_cast<uint32_t>(1));
    check_token(tokens[12].name, symbol_t::NEWLINE,
                tokens[12].content, "");

    check_token(tokens[13].name, symbol_t::TEXT,
                tokens[13].content, "");
    EXPECT_EQ(tokens[13].leading_ws_count, static_cast<uint32_t>(4));
    check_token(tokens[14].name, symbol_t::STAR,
                tokens[14].content, "");
    check_token(tokens[15].name, symbol_t::TEXT,
                tokens[15].content, "special comment");
    EXPECT_EQ(tokens[15].leading_ws_count, static_cast<uint32_t>(1));

    check_token(tokens[16].name, symbol_t::NEWLINE,
                tokens[16].content, "");
    check_token(tokens[17].name, symbol_t::END_BLOCK_COMMENT,
                tokens[17].content, "");
    check_token(tokens[18].name, symbol_t::TEXT,
                tokens[18].content, "");
    EXPECT_EQ(tokens[18].leading_ws_count, static_cast<uint32_t>(2));

    check_token(tokens[19].name, symbol_t::END_OF_FILE,
                tokens[19].content, "");
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

    write_file(content);
    Lexer lexer(filename);
    lexer.process();
    const auto& tokens = lexer.get_tokens();

    EXPECT_EQ(tokens.size(), static_cast<size_t>(20));

    check_token(tokens[0].name, symbol_t::TEXT,
                tokens[0].content, "#include <gtest/gtest.h>");
    check_token(tokens[1].name, symbol_t::NEWLINE,
                tokens[1].content, "");
    check_token(tokens[2].name, symbol_t::NEWLINE,
                tokens[2].content, "");

    check_token(tokens[3].name, symbol_t::TEXT,
                tokens[3].content, "");
    EXPECT_EQ(tokens[3].leading_ws_count, static_cast<uint32_t>(5));
    check_token(tokens[4].name, symbol_t::BEGIN_LINE_COMMENT,
                tokens[4].content, "");
    check_token(tokens[5].name, symbol_t::TEXT,
                tokens[5].content, "a very special comment");
    EXPECT_EQ(tokens[5].leading_ws_count, static_cast<uint32_t>(1));

    check_token(tokens[6].name, symbol_t::END_BLOCK_COMMENT,
                tokens[6].content, "");
    check_token(tokens[7].name, symbol_t::NEWLINE,
                tokens[7].content, "");
    check_token(tokens[8].name, symbol_t::NEWLINE,
                tokens[8].content, "");

    check_token(tokens[9].name, symbol_t::TEXT,
                tokens[9].content, "");
    EXPECT_EQ(tokens[9].leading_ws_count, static_cast<uint32_t>(5));
    check_token(tokens[10].name, symbol_t::BEGIN_BLOCK_COMMENT,
                tokens[10].content, "");
    check_token(tokens[11].name, symbol_t::TEXT,
                tokens[11].content, "another very");
    EXPECT_EQ(tokens[11].leading_ws_count, static_cast<uint32_t>(1));
    check_token(tokens[12].name, symbol_t::NEWLINE,
                tokens[12].content, "");

    check_token(tokens[13].name, symbol_t::TEXT,
                tokens[13].content, "");
    EXPECT_EQ(tokens[13].leading_ws_count, static_cast<uint32_t>(4));
    check_token(tokens[14].name, symbol_t::STAR,
                tokens[14].content, "");
    check_token(tokens[15].name, symbol_t::TEXT,
                tokens[15].content, "special comment");
    EXPECT_EQ(tokens[15].leading_ws_count, static_cast<uint32_t>(1));

    check_token(tokens[16].name, symbol_t::NEWLINE,
                tokens[16].content, "");
    check_token(tokens[17].name, symbol_t::END_BLOCK_COMMENT,
                tokens[17].content, "");
    check_token(tokens[18].name, symbol_t::TEXT,
                tokens[18].content, "");
    EXPECT_EQ(tokens[18].leading_ws_count, static_cast<uint32_t>(2));

    check_token(tokens[19].name, symbol_t::END_OF_FILE,
                tokens[19].content, "");
}

} // namespace core
} // namespace docgen
