#include "lexer_base_fixture.hpp"

namespace docgen {
namespace core {

struct lexer_routines_fixture : lexer_base_fixture
{
protected:

    template <char c>
    static bool is_not(char x) 
    {
        return x != c;
    }
};

////////////////////////////////////////////////////////////////////////
// read_until TESTS
////////////////////////////////////////////////////////////////////////

TEST_F(lexer_routines_fixture, read_until_file_newline)
{
    static constexpr const char* content =
        "very special comment \n"
        ;
    static constexpr const char* expected_str =
        "very special comment ";

    write_file(content);
    file_reader reader(filename);
    std::string actual_str = read_until(reader, is_not<'\n'>);
    EXPECT_EQ(actual_str, expected_str);
}

TEST_F(lexer_routines_fixture, read_until_file_two_newline)
{
    static constexpr const char* content =
        "very special \ncomment \n"
        ;
    static constexpr const char* expected_str =
        "very special ";

    write_file(content);
    file_reader reader(filename);
    std::string actual_str = read_until(reader, is_not<'\n'>);
    EXPECT_EQ(actual_str, expected_str);
}

TEST_F(lexer_routines_fixture, read_until_string_newline)
{
    static constexpr const char* content =
        "very special comment @"
        ;
    static constexpr const char* expected_str =
        "very special comment ";

    string_reader reader(content);
    std::string actual_str = read_until(reader, is_not<'@'>);
    EXPECT_EQ(actual_str, expected_str);
}

TEST_F(lexer_routines_fixture, read_until_string_two_newline)
{
    static constexpr const char* content =
        "very special @comment @"
        ;
    static constexpr const char* expected_str =
        "very special ";

    string_reader reader(content);
    std::string actual_str = read_until(reader, is_not<'@'>);
    EXPECT_EQ(actual_str, expected_str);
}

////////////////////////////////////////////////////////////////////////
// process_tags TESTS
////////////////////////////////////////////////////////////////////////

TEST_F(lexer_routines_fixture, process_tags_ignore)
{
    static constexpr const char* content =
        "some text to ignore "
        ;
    string_reader reader(content);
    status_t status;    // context is none
    process_tags(reader, status);
    EXPECT_EQ(status.tokens.size(), static_cast<size_t>(0));
}

TEST_F(lexer_routines_fixture, process_tags_ignore_one_at_tag)
{
    static constexpr const char* content =
        "some text to ignore   h @desc"
        ;
    string_reader reader(content);
    status_t status;    // context is none
    process_tags(reader, status);
    EXPECT_EQ(status.tokens.size(), static_cast<size_t>(1));
    check_token(status.tokens[0].name, symbol_t::DESC,
                status.tokens[0].content, "desc");
}

TEST_F(lexer_routines_fixture, process_tags_ignore_one_at_tag_info)
{
    static constexpr const char* content =
        "some text to ignore   h @tparam name description."
        ;
    string_reader reader(content);
    status_t status;    // context is none
    process_tags(reader, status);
    EXPECT_EQ(status.tokens.size(), static_cast<size_t>(2));

    check_token(status.tokens[0].name, symbol_t::TPARAM,
                status.tokens[0].content, "tparam");

    check_token(status.tokens[1].name, symbol_t::TAGINFO,
                status.tokens[1].content, "name description.");
}

////////////////////////////////////////////////////////////////////////
// line_comment TESTS
////////////////////////////////////////////////////////////////////////

TEST_F(lexer_routines_fixture, line_comment)
{
    static constexpr const char* content =
        "   very special comment  \n"
        ;
    write_file(content);
    file_reader reader(filename);
    status_t status; // context is none
    line_comment(reader, status);
    EXPECT_EQ(status.tokens.size(), static_cast<size_t>(0));
}

TEST_F(lexer_routines_fixture, line_comment_initial_no_tag)
{
    static constexpr const char* content =
        "   ignore stuff here...    "
        "@notag ignore stuff here...    "
        "\n"
        ;
    write_file(content);
    file_reader reader(filename);
    status_t status; // context is none
    line_comment(reader, status);
    EXPECT_EQ(status.tokens.size(), static_cast<size_t>(0));
}

TEST_F(lexer_routines_fixture, line_comment_no_tag_in_between)
{
    static constexpr const char* content =
        "   @notag ignore @notag stuff here...    "
        " @desc This is a description.  "
        "@notag Further description...    "
        "\n"
        ;
    write_file(content);
    file_reader reader(filename);
    status_t status; // context is none
    line_comment(reader, status);
    EXPECT_EQ(status.tokens.size(), static_cast<size_t>(4));

    check_token(status.tokens[0].name, symbol_t::DESC,
                status.tokens[0].content, "desc");

    check_token(status.tokens[1].name, symbol_t::TAGINFO,
                status.tokens[1].content, "This is a description.  ");

    check_token(status.tokens[2].name, symbol_t::TAGINFO,
                status.tokens[2].content, "@notag ");

    check_token(status.tokens[3].name, symbol_t::TAGINFO,
                status.tokens[3].content, "Further description...");
}

TEST_F(lexer_routines_fixture, line_comment_multiple_tags_and_no_tag)
{
    static constexpr const char* content =
        "   ignore stuff here...    "
        " @desc This is a description.  "
        "@sdesc This is a short description.    "
        "@tparam Type   some type.  "
        "@notag ignore stuff here..."
        "\n"
        ;
    write_file(content);
    file_reader reader(filename);
    status_t status; // context is none
    line_comment(reader, status);
    EXPECT_EQ(status.tokens.size(), static_cast<size_t>(8));

    check_token(status.tokens[0].name, symbol_t::DESC,
                status.tokens[0].content, "desc");

    check_token(status.tokens[1].name, symbol_t::TAGINFO,
                status.tokens[1].content, "This is a description.  ");

    check_token(status.tokens[2].name, symbol_t::SDESC,
                status.tokens[2].content, "sdesc");

    check_token(status.tokens[3].name, symbol_t::TAGINFO,
                status.tokens[3].content, "This is a short description.    ");

    check_token(status.tokens[4].name, symbol_t::TPARAM,
                status.tokens[4].content, "tparam");

    check_token(status.tokens[5].name, symbol_t::TAGINFO,
                status.tokens[5].content, "Type   some type.  ");

    check_token(status.tokens[6].name, symbol_t::TAGINFO,
                status.tokens[6].content, "@notag ");

    check_token(status.tokens[7].name, symbol_t::TAGINFO,
                status.tokens[7].content, "ignore stuff here...");
}

////////////////////////////////////////////////////////////////////////
// block_comment TESTS
////////////////////////////////////////////////////////////////////////

//TEST_F(lexer_routines_fixture, block_comment)
//{
//    static constexpr const char* content =
//        "very special comment  */"
//        ;
//    static constexpr const char* expected_str =
//        "very special comment  ";
//
//    write_file(content);
//    file_reader reader(filename);
//    token_t token = block_comment(reader);
//    check_token(token.name, DocSymbol::block_comment,
//                token.content, expected_str);
//}
//
//TEST_F(lexer_routines_fixture, process_line_comment)
//{
//    static constexpr const char* content =
//        "#include <gtest/gtest.h>\n"
//        "\n"
//        "  // just a normal comment\n"
//        "   /// a very special comment   \n"
//        "\n"
//        ;
//    static constexpr const char* expected_str =
//        " a very special comment   ";
//
//    write_file(content);
//    file_reader reader(filename);
//    token_t token = process(reader);
//    check_token(token.name, DocSymbol::line_comment,
//                token.content, expected_str);
//}
//
//TEST_F(lexer_routines_fixture, process_block_comment)
//{
//    static constexpr const char* content =
//        "#include <gtest/gtest.h>\n"
//        "\n"
//        "  // just a normal comment\n"
//        "   /*!  a very special comment   \n"
//        "   * another special comment  \n"
//        "*/"
//        "\n"
//        ;
//    static constexpr const char* expected_str =
//        "  a very special comment   \n"
//        "   * another special comment  \n"
//        ;
//
//    write_file(content);
//    file_reader reader(filename);
//    token_t token = process(reader);
//    check_token(token.name, DocSymbol::block_comment,
//                token.content, expected_str);
//}

} // namespace core
} // namespace docgen
