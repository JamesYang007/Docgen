#include <core/lexer/lexer.hpp>
#include <gtest/gtest.h>

namespace docgen {
namespace core {
namespace lexer {

struct lexer_fixture : ::testing::Test
{
protected:
    using status_t = typename Lexer::status_t;
    using token_t = typename Lexer::token_t;
    using symbol_t = typename Lexer::symbol_t;

    Lexer lexer;
    std::optional<token_t> token;

    void setup_lexer(const char* content)
    {
        std::string str(content);
        for (char c : str) {
            lexer.process(c);
        }
        lexer.flush();
    }
};

////////////////////////////////////////////////////////////////////
// Individual Symbol TESTS
////////////////////////////////////////////////////////////////////

// NEWLINE
TEST_F(lexer_fixture, lexer_newline)
{
    static constexpr const char* content =
        "somecrazy1492text\nmvn2b"
        ;
    setup_lexer(content);

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "somecrazy1492text");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::NEWLINE);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "mvn2b");

    // check that there are no more tokens
    token = lexer.next_token();
    EXPECT_FALSE(static_cast<bool>(token));
}

// WHITESPACE (  )
TEST_F(lexer_fixture, lexer_whitespace_space)
{
    static constexpr const char* content =
        ",m.,m. abn"
        ;
    setup_lexer(content);

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, ",m.,m.");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::WHITESPACE);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "abn");

    // check that there are no more tokens
    token = lexer.next_token();
    EXPECT_FALSE(static_cast<bool>(token));
}

// WHITESPACE (\t)
TEST_F(lexer_fixture, lexer_whitespace_t)
{
    static constexpr const char* content =
        "h0f2n.1\t1234|"
        ;
    setup_lexer(content);

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "h0f2n.1");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::WHITESPACE);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "1234|");

    // check that there are no more tokens
    token = lexer.next_token();
    EXPECT_FALSE(static_cast<bool>(token));
}

// WHITESPACE (\v)
TEST_F(lexer_fixture, lexer_whitespace_v)
{
    static constexpr const char* content =
        "hello!\v"
        ;
    setup_lexer(content);

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "hello!");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::WHITESPACE);
	EXPECT_EQ(token->content, "");

    // check that there are no more tokens
    token = lexer.next_token();
    EXPECT_FALSE(static_cast<bool>(token));
}

// WHITESPACE (\r)
TEST_F(lexer_fixture, lexer_whitespace_r)
{
    static constexpr const char* content =
        "hello!\rwsdescorrld!!"
        ;
    setup_lexer(content);

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "hello!");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::WHITESPACE);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "wsdescorrld!!");

    // check that there are no more tokens
    token = lexer.next_token();
    EXPECT_FALSE(static_cast<bool>(token));
}

// WHITESPACE (\f)
TEST_F(lexer_fixture, lexer_whitespace_f)
{
    static constexpr const char* content =
        "hello!\fwsdescorrld!!"
        ;
    setup_lexer(content);

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "hello!");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::WHITESPACE);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "wsdescorrld!!");

    // check that there are no more tokens
    token = lexer.next_token();
    EXPECT_FALSE(static_cast<bool>(token));
}

// WHITESPACE (;)
TEST_F(lexer_fixture, lexer_semicolon)
{
    static constexpr const char* content =
        ";wsdescorrld!!"
        ;
    setup_lexer(content);

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::SEMICOLON);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "wsdescorrld!!");

    // check that there are no more tokens
    token = lexer.next_token();
    EXPECT_FALSE(static_cast<bool>(token));
}

// BEGIN_SLINE_COMMENT
TEST_F(lexer_fixture, lexer_begin_sline_comment)
{
    static constexpr const char* content =
        "abc///"
        ;
    setup_lexer(content);

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "abc");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::BEGIN_SLINE_COMMENT);
	EXPECT_EQ(token->content, "");

    // check that there are no more tokens
    token = lexer.next_token();
    EXPECT_FALSE(static_cast<bool>(token));
}

// BEGIN_SBLOCK_COMMENT
TEST_F(lexer_fixture, lexer_begin_sblock_comment)
{
    static constexpr const char* content =
        "abc/*!"
        ;
    setup_lexer(content);

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "abc");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::BEGIN_SBLOCK_COMMENT);
	EXPECT_EQ(token->content, "");

    // check that there are no more tokens
    token = lexer.next_token();
    EXPECT_FALSE(static_cast<bool>(token));
}

// BEGIN_NBLOCK_COMMENT
TEST_F(lexer_fixture, lexer_begin_nblock_comment)
{
    static constexpr const char* content =
        "abc/**!"
        ;
    setup_lexer(content);

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "abc");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::BEGIN_NBLOCK_COMMENT);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::STAR);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "!");

    // check that there are no more tokens
    token = lexer.next_token();
    EXPECT_FALSE(static_cast<bool>(token));
}

// END_BLOCK_COMMENT
TEST_F(lexer_fixture, lexer_end_block_comment_no_star)
{
    static constexpr const char* content =
        "abc*/f"
        ;
    setup_lexer(content);

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "abc");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::END_BLOCK_COMMENT);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "f");

    // check that there are no more tokens
    token = lexer.next_token();
    EXPECT_FALSE(static_cast<bool>(token));
}

TEST_F(lexer_fixture, lexer_end_block_comment_star)
{
    static constexpr const char* content =
        "abc**/f"
        ;
    setup_lexer(content);

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "abc");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::STAR);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::END_BLOCK_COMMENT);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "f");

    // check that there are no more tokens
    token = lexer.next_token();
    EXPECT_FALSE(static_cast<bool>(token));
}

// SDESC
TEST_F(lexer_fixture, lexer_sdesc)
{
    static constexpr const char* content =
        "ssdesc@@sdescf@sdesscf"
        ;
    setup_lexer(content);

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "ssdesc@");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::SDESC);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "f@sdesscf");

    // check that there are no more tokens
    token = lexer.next_token();
    EXPECT_FALSE(static_cast<bool>(token));
}

////////////////////////////////////////////////////////////////////
// Mix TESTS
////////////////////////////////////////////////////////////////////

// line comment mix
TEST_F(lexer_fixture, lexer_line_comment_4)
{
    static constexpr const char* content =
        "abc////"
        ;
    setup_lexer(content);

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "abc");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::BEGIN_SLINE_COMMENT);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "/");

    // check that there are no more tokens
    token = lexer.next_token();
    EXPECT_FALSE(static_cast<bool>(token));
}

// line comment mix
TEST_F(lexer_fixture, lexer_line_comment_5)
{
    static constexpr const char* content =
        "abc/////"
        ;
    setup_lexer(content);

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "abc");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::BEGIN_SLINE_COMMENT);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::BEGIN_NLINE_COMMENT);
	EXPECT_EQ(token->content, "");

    // check that there are no more tokens
    token = lexer.next_token();
    EXPECT_FALSE(static_cast<bool>(token));
}

// line comment mix
TEST_F(lexer_fixture, lexer_line_comment_6)
{
    static constexpr const char* content =
        "abc//////"
        ;
    setup_lexer(content);

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "abc");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::BEGIN_SLINE_COMMENT);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::BEGIN_SLINE_COMMENT);
	EXPECT_EQ(token->content, "");

    // check that there are no more tokens
    token = lexer.next_token();
    EXPECT_FALSE(static_cast<bool>(token));
}

TEST_F(lexer_fixture, lexer_test_1)
{
    static constexpr const char* content =
        "#include <core/lexer_trie.hpp> // some comment\n"
        "\n"
        "void f();"
        ;

    setup_lexer(content);

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::HASHTAG);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "include");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::WHITESPACE);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "<core/lexer_trie.hpp>");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::WHITESPACE);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::BEGIN_NLINE_COMMENT);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::WHITESPACE);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "some");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::WHITESPACE);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "comment");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::NEWLINE);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::NEWLINE);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "void");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::WHITESPACE);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "f()");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::SEMICOLON);
	EXPECT_EQ(token->content, "");
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

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::HASHTAG);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "include");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::WHITESPACE);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "<gtest/gtest.h>");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::NEWLINE);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::NEWLINE);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::WHITESPACE);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::WHITESPACE);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::BEGIN_NLINE_COMMENT);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::WHITESPACE);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "just");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::WHITESPACE);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "a");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::WHITESPACE);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "normal");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::WHITESPACE);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "comment");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::NEWLINE);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::NEWLINE);
	EXPECT_EQ(token->content, "");
}

TEST_F(lexer_fixture, process_one_line_comment)
{
    static constexpr const char* content =
        "// comment\n"
        " /// special_comment \n"
        ;

    setup_lexer(content);

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::BEGIN_NLINE_COMMENT);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::WHITESPACE);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "comment");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::NEWLINE);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::WHITESPACE);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::BEGIN_SLINE_COMMENT);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::WHITESPACE);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::TEXT);
	EXPECT_EQ(token->content, "special_comment");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::WHITESPACE);
	EXPECT_EQ(token->content, "");

    token = lexer.next_token();
    EXPECT_EQ(token->name, symbol_t::NEWLINE);
	EXPECT_EQ(token->content, "");
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
//    check_token(tokens[0]->name, symbol_t::TEXT,
//                tokens[0]->content, "#include <gtest/gtest.h>");
//    check_token(tokens[1]->name, symbol_t::NEWLINE,
//                tokens[1]->content, "");
//    check_token(tokens[2]->name, symbol_t::NEWLINE,
//                tokens[2]->content, "");
//    check_token(tokens[3]->name, symbol_t::BEGIN_LINE_COMMENT,
//                tokens[3]->content, "");
//    check_token(tokens[4]->name, symbol_t::TEXT,
//                tokens[4]->content, "a very special comment");
//    EXPECT_EQ(tokens[4].leading_ws_count, static_cast<uint32_t>(1));
//    check_token(tokens[5]->name, symbol_t::NEWLINE,
//                tokens[5]->content, "");
//    check_token(tokens[6]->name, symbol_t::NEWLINE,
//                tokens[6]->content, "");
//    check_token(tokens[7]->name, symbol_t::BEGIN_LINE_COMMENT,
//                tokens[7]->content, "");
//    check_token(tokens[8]->name, symbol_t::TEXT,
//                tokens[8]->content, "another very special comment");
//    EXPECT_EQ(tokens[8].leading_ws_count, static_cast<uint32_t>(1));
//    check_token(tokens[9]->name, symbol_t::NEWLINE,
//                tokens[9]->content, "");
//    check_token(tokens[10]->name, symbol_t::END_OF_FILE,
//                tokens[10]->content, "");
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
//    check_token(tokens[0]->name, symbol_t::TEXT,
//                tokens[0]->content, "#include <gtest/gtest.h>");
//    check_token(tokens[1]->name, symbol_t::NEWLINE,
//                tokens[1]->content, "");
//    check_token(tokens[2]->name, symbol_t::NEWLINE,
//                tokens[2]->content, "");
//    check_token(tokens[3]->name, symbol_t::BEGIN_BLOCK_COMMENT,
//                tokens[3]->content, "");
//    check_token(tokens[4]->name, symbol_t::TEXT,
//                tokens[4]->content, "a very special comment");
//    EXPECT_EQ(tokens[4].leading_ws_count, static_cast<uint32_t>(1));
//    check_token(tokens[5]->name, symbol_t::END_BLOCK_COMMENT,
//                tokens[5]->content, "");
//    check_token(tokens[6]->name, symbol_t::NEWLINE,
//                tokens[6]->content, "");
//    check_token(tokens[7]->name, symbol_t::NEWLINE,
//                tokens[7]->content, "");
//    check_token(tokens[8]->name, symbol_t::END_OF_FILE,
//                tokens[8]->content, "");
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
//    check_token(tokens[0]->name, symbol_t::TEXT,
//                tokens[0]->content, "#include <gtest/gtest.h>");
//    check_token(tokens[1]->name, symbol_t::NEWLINE,
//                tokens[1]->content, "");
//    check_token(tokens[2]->name, symbol_t::NEWLINE,
//                tokens[2]->content, "");
//    check_token(tokens[3]->name, symbol_t::BEGIN_BLOCK_COMMENT,
//                tokens[3]->content, "");
//    check_token(tokens[4]->name, symbol_t::TEXT,
//                tokens[4]->content, "a very special comment");
//    EXPECT_EQ(tokens[4].leading_ws_count, static_cast<uint32_t>(1));
//    check_token(tokens[5]->name, symbol_t::END_BLOCK_COMMENT,
//                tokens[5]->content, "");
//    check_token(tokens[6]->name, symbol_t::NEWLINE,
//                tokens[6]->content, "");
//    check_token(tokens[7]->name, symbol_t::NEWLINE,
//                tokens[7]->content, "");
//    check_token(tokens[8]->name, symbol_t::BEGIN_BLOCK_COMMENT,
//                tokens[8]->content, "");
//    check_token(tokens[9]->name, symbol_t::TEXT,
//                tokens[9]->content, "another very");
//    EXPECT_EQ(tokens[9].leading_ws_count, static_cast<uint32_t>(1));
//    check_token(tokens[10]->name, symbol_t::NEWLINE,
//                tokens[10]->content, "");
//    check_token(tokens[11]->name, symbol_t::STAR,
//                tokens[11]->content, "");
//    check_token(tokens[12]->name, symbol_t::TEXT,
//                tokens[12]->content, "special comment");
//    EXPECT_EQ(tokens[12].leading_ws_count, static_cast<uint32_t>(1));
//    check_token(tokens[13]->name, symbol_t::NEWLINE,
//                tokens[13]->content, "");
//    check_token(tokens[14]->name, symbol_t::END_BLOCK_COMMENT,
//                tokens[14]->content, "");
//    check_token(tokens[15]->name, symbol_t::END_OF_FILE,
//                tokens[15]->content, "");
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
//    check_token(tokens[0]->name, symbol_t::TEXT,
//                tokens[0]->content, "#include <gtest/gtest.h>");
//    check_token(tokens[1]->name, symbol_t::NEWLINE,
//                tokens[1]->content, "");
//    check_token(tokens[2]->name, symbol_t::NEWLINE,
//                tokens[2]->content, "");
//    check_token(tokens[3]->name, symbol_t::BEGIN_LINE_COMMENT,
//                tokens[3]->content, "");
//    check_token(tokens[4]->name, symbol_t::TEXT,
//                tokens[4]->content, "a very special comment");
//    EXPECT_EQ(tokens[4].leading_ws_count, static_cast<uint32_t>(1));
//    check_token(tokens[5]->name, symbol_t::END_BLOCK_COMMENT,
//                tokens[5]->content, "");
//    check_token(tokens[6]->name, symbol_t::NEWLINE,
//                tokens[6]->content, "");
//    check_token(tokens[7]->name, symbol_t::NEWLINE,
//                tokens[7]->content, "");
//    check_token(tokens[8]->name, symbol_t::BEGIN_BLOCK_COMMENT,
//                tokens[8]->content, "");
//    check_token(tokens[9]->name, symbol_t::TEXT,
//                tokens[9]->content, "another very");
//    EXPECT_EQ(tokens[9].leading_ws_count, static_cast<uint32_t>(1));
//    check_token(tokens[10]->name, symbol_t::NEWLINE,
//                tokens[10]->content, "");
//    check_token(tokens[11]->name, symbol_t::STAR,
//                tokens[11]->content, "");
//    check_token(tokens[12]->name, symbol_t::TEXT,
//                tokens[12]->content, "special comment");
//    EXPECT_EQ(tokens[12].leading_ws_count, static_cast<uint32_t>(1));
//    check_token(tokens[13]->name, symbol_t::NEWLINE,
//                tokens[13]->content, "");
//    check_token(tokens[14]->name, symbol_t::END_BLOCK_COMMENT,
//                tokens[14]->content, "");
//    check_token(tokens[15]->name, symbol_t::END_OF_FILE,
//                tokens[15]->content, "");
//}

} // namespace lexer 
} // namespace core
} // namespace docgen
