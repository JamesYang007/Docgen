#define private public

#include <core/lexer.hpp>
#include <gtest/gtest.h>

namespace docgen {
namespace core {

struct lexer_fixture : ::testing::Test
{
protected:
    using status_t = Lexer::status_t;
    using token_t = Lexer::token_t;
    using symbol_t = Lexer::symbol_t;

    Lexer lexer;

    void setup_lexer(const char* content)
    {
        std::string str(content);
        for (char c : str) {
            lexer.process(c);
        }
        lexer.process(0);
    }

    void check_token(symbol_t actual_sym, symbol_t expected_sym,
                     const std::string& actual_str, const std::string& expected_str)
    {
        EXPECT_EQ(actual_sym, expected_sym);
        EXPECT_EQ(actual_str, expected_str);
    }
};

TEST_F(lexer_fixture, lexer)
{
    static constexpr const char* content =
        "#include <core/lexer_trie.hpp> // some comment\n"
        "\n"
        "void f();"
        ;

    setup_lexer(content);

    auto token = *lexer.next_token();
    check_token(token.name, symbol_t::HASHTAG,
                token.content, "");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::TEXT,
                token.content, "include");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::WHITESPACE,
                token.content, "");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::TEXT,
                token.content, "<core/lexer_trie.hpp>");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::WHITESPACE,
                token.content, "");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::BEGIN_NLINE_COMMENT,
                token.content, "");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::WHITESPACE,
                token.content, "");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::TEXT,
                token.content, "some");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::WHITESPACE,
                token.content, "");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::TEXT,
                token.content, "comment");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::NEWLINE,
                token.content, "");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::NEWLINE,
                token.content, "");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::TEXT,
                token.content, "void");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::WHITESPACE,
                token.content, "");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::TEXT,
                token.content, "f()");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::SEMICOLON,
                token.content, "");
}

TEST_F(lexer_fixture, process_no_comment)
{
    static constexpr const char* content =
        "#include <gtest/gtest.h>\n"
        "\n"
        "  // just a normal comment\n"
        "\n"
        ;

    setup_lexer(content);

    auto token = *lexer.next_token();
    check_token(token.name, symbol_t::HASHTAG,
                token.content, "");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::TEXT,
                token.content, "include");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::WHITESPACE,
                token.content, "");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::TEXT,
                token.content, "<gtest/gtest.h>");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::NEWLINE,
                token.content, "");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::NEWLINE,
                token.content, "");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::WHITESPACE,
                token.content, "");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::WHITESPACE,
                token.content, "");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::BEGIN_NLINE_COMMENT,
                token.content, "");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::WHITESPACE,
                token.content, "");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::TEXT,
                token.content, "just");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::WHITESPACE,
                token.content, "");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::TEXT,
                token.content, "a");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::WHITESPACE,
                token.content, "");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::TEXT,
                token.content, "normal");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::WHITESPACE,
                token.content, "");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::TEXT,
                token.content, "comment");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::NEWLINE,
                token.content, "");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::NEWLINE,
                token.content, "");
}

TEST_F(lexer_fixture, process_one_line_comment)
{
    static constexpr const char* content =
        "// comment\n"
        " /// special_comment \n"
        ;

    setup_lexer(content);

    auto token = *lexer.next_token();
    check_token(token.name, symbol_t::BEGIN_NLINE_COMMENT,
                token.content, "");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::WHITESPACE,
                token.content, "");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::TEXT,
                token.content, "comment");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::NEWLINE,
                token.content, "");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::WHITESPACE,
                token.content, "");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::BEGIN_SLINE_COMMENT,
                token.content, "");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::WHITESPACE,
                token.content, "");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::TEXT,
                token.content, "special_comment");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::WHITESPACE,
                token.content, "");

    token = *lexer.next_token();
    check_token(token.name, symbol_t::NEWLINE,
                token.content, "");


}

//TEST_F(lexer_fixture, process_two_line_comment)
//{
//    static constexpr const char* content =
//        "#include <gtest/gtest.h>\n"
//        "\n"
//        "  // just a normal comment\n"
//        "   /// a very special comment   \n"
//        "\n"
//        "  // just a normal comment\n"
//        "   /// another very special comment   \n"
//        "  // just a normal comment\n"
//        ;
//
//    write_file(content);
//    Lexer lexer(file);
//    lexer.process();
//    const auto& tokens = lexer.get_tokens();
//
//    EXPECT_EQ(tokens.size(), static_cast<size_t>(11));
//
//    check_token(tokens[0].name, symbol_t::TEXT,
//                tokens[0].content, "#include <gtest/gtest.h>");
//    check_token(tokens[1].name, symbol_t::NEWLINE,
//                tokens[1].content, "");
//    check_token(tokens[2].name, symbol_t::NEWLINE,
//                tokens[2].content, "");
//    check_token(tokens[3].name, symbol_t::BEGIN_LINE_COMMENT,
//                tokens[3].content, "");
//    check_token(tokens[4].name, symbol_t::TEXT,
//                tokens[4].content, "a very special comment");
//    EXPECT_EQ(tokens[4].leading_ws_count, static_cast<uint32_t>(1));
//    check_token(tokens[5].name, symbol_t::NEWLINE,
//                tokens[5].content, "");
//    check_token(tokens[6].name, symbol_t::NEWLINE,
//                tokens[6].content, "");
//    check_token(tokens[7].name, symbol_t::BEGIN_LINE_COMMENT,
//                tokens[7].content, "");
//    check_token(tokens[8].name, symbol_t::TEXT,
//                tokens[8].content, "another very special comment");
//    EXPECT_EQ(tokens[8].leading_ws_count, static_cast<uint32_t>(1));
//    check_token(tokens[9].name, symbol_t::NEWLINE,
//                tokens[9].content, "");
//    check_token(tokens[10].name, symbol_t::END_OF_FILE,
//                tokens[10].content, "");
//}
//
//TEST_F(lexer_fixture, process_one_block_comment)
//{
//    static constexpr const char* content =
//        "#include <gtest/gtest.h>\n"
//        "\n"
//        "  // just a normal comment\n"
//        "   /*! a very special comment   */\n"
//        "\n"
//        ;
//
//    write_file(content);
//    Lexer lexer(file);
//    lexer.process();
//    const auto& tokens = lexer.get_tokens();
//
//    EXPECT_EQ(tokens.size(), static_cast<size_t>(9));
//
//    check_token(tokens[0].name, symbol_t::TEXT,
//                tokens[0].content, "#include <gtest/gtest.h>");
//    check_token(tokens[1].name, symbol_t::NEWLINE,
//                tokens[1].content, "");
//    check_token(tokens[2].name, symbol_t::NEWLINE,
//                tokens[2].content, "");
//    check_token(tokens[3].name, symbol_t::BEGIN_BLOCK_COMMENT,
//                tokens[3].content, "");
//    check_token(tokens[4].name, symbol_t::TEXT,
//                tokens[4].content, "a very special comment");
//    EXPECT_EQ(tokens[4].leading_ws_count, static_cast<uint32_t>(1));
//    check_token(tokens[5].name, symbol_t::END_BLOCK_COMMENT,
//                tokens[5].content, "");
//    check_token(tokens[6].name, symbol_t::NEWLINE,
//                tokens[6].content, "");
//    check_token(tokens[7].name, symbol_t::NEWLINE,
//                tokens[7].content, "");
//    check_token(tokens[8].name, symbol_t::END_OF_FILE,
//                tokens[8].content, "");
//}
//
//TEST_F(lexer_fixture, process_two_block_comment)
//{
//    static constexpr const char* content =
//        "#include <gtest/gtest.h>\n"
//        "\n"
//        "  // just a normal comment\n"
//        "   /*! a very special comment   */\n"
//        "\n"
//        "  // just a normal comment\n"
//        "   /*! another very \n"
//        "    * special comment   \n"
//        "*/"
//        "  /* just a normal comment\n */"
//        ;
//
//    write_file(content);
//    Lexer lexer(file);
//    lexer.process();
//    const auto& tokens = lexer.get_tokens();
//
//    EXPECT_EQ(tokens.size(), static_cast<size_t>(16));
//
//    check_token(tokens[0].name, symbol_t::TEXT,
//                tokens[0].content, "#include <gtest/gtest.h>");
//    check_token(tokens[1].name, symbol_t::NEWLINE,
//                tokens[1].content, "");
//    check_token(tokens[2].name, symbol_t::NEWLINE,
//                tokens[2].content, "");
//    check_token(tokens[3].name, symbol_t::BEGIN_BLOCK_COMMENT,
//                tokens[3].content, "");
//    check_token(tokens[4].name, symbol_t::TEXT,
//                tokens[4].content, "a very special comment");
//    EXPECT_EQ(tokens[4].leading_ws_count, static_cast<uint32_t>(1));
//    check_token(tokens[5].name, symbol_t::END_BLOCK_COMMENT,
//                tokens[5].content, "");
//    check_token(tokens[6].name, symbol_t::NEWLINE,
//                tokens[6].content, "");
//    check_token(tokens[7].name, symbol_t::NEWLINE,
//                tokens[7].content, "");
//    check_token(tokens[8].name, symbol_t::BEGIN_BLOCK_COMMENT,
//                tokens[8].content, "");
//    check_token(tokens[9].name, symbol_t::TEXT,
//                tokens[9].content, "another very");
//    EXPECT_EQ(tokens[9].leading_ws_count, static_cast<uint32_t>(1));
//    check_token(tokens[10].name, symbol_t::NEWLINE,
//                tokens[10].content, "");
//    check_token(tokens[11].name, symbol_t::STAR,
//                tokens[11].content, "");
//    check_token(tokens[12].name, symbol_t::TEXT,
//                tokens[12].content, "special comment");
//    EXPECT_EQ(tokens[12].leading_ws_count, static_cast<uint32_t>(1));
//    check_token(tokens[13].name, symbol_t::NEWLINE,
//                tokens[13].content, "");
//    check_token(tokens[14].name, symbol_t::END_BLOCK_COMMENT,
//                tokens[14].content, "");
//    check_token(tokens[15].name, symbol_t::END_OF_FILE,
//                tokens[15].content, "");
//}
//
//TEST_F(lexer_fixture, process_line_block_comment)
//{
//    static constexpr const char* content =
//        "#include <gtest/gtest.h>\n"
//        "\n"
//        "  // just a normal comment\n"
//        "   /// a very special comment   */\n"
//        "\n"
//        "  // just a normal comment\n"
//        "   /*! another very \n"
//        "    * special comment   \n"
//        "*/"
//        "  /* just a normal comment\n */"
//        ;
//
//    write_file(content);
//    Lexer lexer(file);
//    lexer.process();
//    const auto& tokens = lexer.get_tokens();
//
//    EXPECT_EQ(tokens.size(), static_cast<size_t>(16));
//
//    check_token(tokens[0].name, symbol_t::TEXT,
//                tokens[0].content, "#include <gtest/gtest.h>");
//    check_token(tokens[1].name, symbol_t::NEWLINE,
//                tokens[1].content, "");
//    check_token(tokens[2].name, symbol_t::NEWLINE,
//                tokens[2].content, "");
//    check_token(tokens[3].name, symbol_t::BEGIN_LINE_COMMENT,
//                tokens[3].content, "");
//    check_token(tokens[4].name, symbol_t::TEXT,
//                tokens[4].content, "a very special comment");
//    EXPECT_EQ(tokens[4].leading_ws_count, static_cast<uint32_t>(1));
//    check_token(tokens[5].name, symbol_t::END_BLOCK_COMMENT,
//                tokens[5].content, "");
//    check_token(tokens[6].name, symbol_t::NEWLINE,
//                tokens[6].content, "");
//    check_token(tokens[7].name, symbol_t::NEWLINE,
//                tokens[7].content, "");
//    check_token(tokens[8].name, symbol_t::BEGIN_BLOCK_COMMENT,
//                tokens[8].content, "");
//    check_token(tokens[9].name, symbol_t::TEXT,
//                tokens[9].content, "another very");
//    EXPECT_EQ(tokens[9].leading_ws_count, static_cast<uint32_t>(1));
//    check_token(tokens[10].name, symbol_t::NEWLINE,
//                tokens[10].content, "");
//    check_token(tokens[11].name, symbol_t::STAR,
//                tokens[11].content, "");
//    check_token(tokens[12].name, symbol_t::TEXT,
//                tokens[12].content, "special comment");
//    EXPECT_EQ(tokens[12].leading_ws_count, static_cast<uint32_t>(1));
//    check_token(tokens[13].name, symbol_t::NEWLINE,
//                tokens[13].content, "");
//    check_token(tokens[14].name, symbol_t::END_BLOCK_COMMENT,
//                tokens[14].content, "");
//    check_token(tokens[15].name, symbol_t::END_OF_FILE,
//                tokens[15].content, "");
//}

} // namespace core
} // namespace docgen
