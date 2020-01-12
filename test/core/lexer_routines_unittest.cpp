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

    void read(file_reader& reader, std::string& str) 
    {
        int c = 0;
        while ((c = reader.read()) != file_reader::termination) {
            str.push_back(c);
        }
    }

    template <class Condition>
    void ignore_until_test(const char* content, const char* expected_str, 
                           char expected_last_char, Condition condition) 
    {
        write_file(content);
        file_reader reader(filename);
        std::string actual;
        int last_char = ignore_until(reader, condition);
        EXPECT_EQ(last_char, expected_last_char);
        read(reader, actual);
        EXPECT_EQ(actual, expected_str);
    }

    template <class Condition>
    void read_until_test(const char* content, const char* expected_str,
                         char expected_last_char, Condition condition)
    {
        write_file(content);
        file_reader reader(filename);
        std::string actual;
        int last_char = read_until(reader, condition, actual);
        EXPECT_EQ(last_char, expected_last_char);
        EXPECT_EQ(actual, expected_str);
    }

    void trim_test(const char* content, const char* expected) 
    {
        std::string actual(content);
        trim(actual);
        EXPECT_EQ(actual, expected);
    }

    void tokenize_text_check(const std::string& actual, const token_t& token, 
                             const char* expected)
    {
        check_token(token.name, symbol_t::TEXT,
                    token.content, expected);
        EXPECT_EQ(actual.size(), static_cast<size_t>(0));
        EXPECT_GT(actual.capacity(), DEFAULT_STRING_RESERVE_SIZE);
    }

    void process_char_check(bool res, const status_t& status,
                            const std::string& actual, const char* expected, 
                            symbol_t expected_symbol)
    {
        EXPECT_TRUE(res);
        EXPECT_EQ(status.tokens.size(), static_cast<size_t>(2));
        tokenize_text_check(actual, status.tokens[0], expected);
        check_token(status.tokens[1].name, expected_symbol,
                    status.tokens[1].content, "");
    }

};

////////////////////////////////////////////////////////////////////////
// ignore_until TESTS
////////////////////////////////////////////////////////////////////////

TEST_F(lexer_routines_fixture, ignore_until_newline)
{
    static constexpr const char* content =
        "to ignore here \ndo not ignore"
        ;
    static constexpr const char* expected =
        "do not ignore";
    ignore_until_test(content, expected, '\n', is_not<'\n'>);
}

TEST_F(lexer_routines_fixture, ignore_until_empty_content)
{
    static constexpr const char* content =
        ""
        ;
    static constexpr const char* expected =
        "";
    ignore_until_test(content, expected, file_reader::termination, is_not<'a'>);
}

TEST_F(lexer_routines_fixture, ignore_until_first_char)
{
    static constexpr const char* content =
        "hello"
        ;
    static constexpr const char* expected =
        "ello";
    ignore_until_test(content, expected, 'h', is_not<'h'>);
}

TEST_F(lexer_routines_fixture, ignore_until_last_char)
{
    static constexpr const char* content =
        "hello"
        ;
    static constexpr const char* expected =
        "";
    ignore_until_test(content, expected, 'o', is_not<'o'>);
}

////////////////////////////////////////////////////////////////////////
// read_until TESTS
////////////////////////////////////////////////////////////////////////

TEST_F(lexer_routines_fixture, read_until_newline)
{
    static constexpr const char* content =
        "very special comment \n not read here"
        ;
    static constexpr const char* expected =
        "very special comment ";
    read_until_test(content, expected, '\n', is_not<'\n'>);
}

TEST_F(lexer_routines_fixture, read_until_two_newline)
{
    static constexpr const char* content =
        "very special \ncomment \n"
        ;
    static constexpr const char* expected =
        "very special ";
    read_until_test(content, expected, '\n', is_not<'\n'>);
}

TEST_F(lexer_routines_fixture, read_until_empty) 
{
    static constexpr const char* content =
        ""
        ;
    static constexpr const char* expected =
        "";
    read_until_test(content, expected, file_reader::termination, is_not<'c'>);
}

TEST_F(lexer_routines_fixture, read_until_first_char)
{
    static constexpr const char* content =
        "very special \ncomment \n"
        ;
    static constexpr const char* expected =
        "";
    read_until_test(content, expected, 'v', is_not<'v'>);
}

TEST_F(lexer_routines_fixture, read_until_last_char)
{
    static constexpr const char* content =
        "very special comment #"
        ;
    static constexpr const char* expected =
        "very special comment ";
    read_until_test(content, expected, '#', is_not<'#'>);
}

////////////////////////////////////////////////////////////////////////
// trim TESTS
////////////////////////////////////////////////////////////////////////

TEST_F(lexer_routines_fixture, trim_empty)
{
    static constexpr const char* content =
        ""
        ;
    static constexpr const char* expected =
        "";
    trim_test(content, expected);
}

TEST_F(lexer_routines_fixture, trim_only_leading)
{
    static constexpr const char* content =
        "    \n\t hello\tworld!"
        ;
    static constexpr const char* expected =
        "hello\tworld!";
    trim_test(content, expected);
}

TEST_F(lexer_routines_fixture, trim_only_trailing)
{
    static constexpr const char* content =
        "hello\tworld!\v\r\t\f    \n\t "
        ;
    static constexpr const char* expected =
        "hello\tworld!";
    trim_test(content, expected);
}

TEST_F(lexer_routines_fixture, trim_leading_trailing)
{
    static constexpr const char* content =
        "\n \r\t \f     hello\tworld!\v\r\t\f    \n\t "
        ;
    static constexpr const char* expected =
        "hello\tworld!";
    trim_test(content, expected);
}

////////////////////////////////////////////////////////////////////////
// tokenize_text TESTS
////////////////////////////////////////////////////////////////////////

TEST_F(lexer_routines_fixture, tokenize_text_empty) 
{
    static constexpr const char* content =
        ""
        ;
    static constexpr const char* expected =
        "";

    std::string actual(content);
    status_t status;
    tokenize_text(actual, status);
    EXPECT_EQ(status.tokens.size(), static_cast<size_t>(0));
    EXPECT_EQ(actual, expected);
}

TEST_F(lexer_routines_fixture, tokenize_text) 
{
    static constexpr const char* content =
        "\n \r\t \f     hello\tworld!\v\r\t\f    \n\t "
        ;
    static constexpr const char* expected =
        "hello\tworld!";

    std::string actual(content);
    status_t status;
    tokenize_text(actual, status);
    tokenize_text_check(actual, status.tokens[0], expected);
}

// this tests whether text is left in a valid state for the next processing
TEST_F(lexer_routines_fixture, tokenize_text_twice) 
{
    static constexpr const char* content_1 =
        "\n \r\t \f     hello\tworld!\v\r\t\f    \n\t "
        ;
    static constexpr const char* expected_1 =
        "hello\tworld!";

    static constexpr const char* content_2 =
        "\n this is docgen!\v\f    \n\t "
        ;
    static constexpr const char* expected_2 =
        "this is docgen!";

    std::string actual(content_1);
    status_t status;
    tokenize_text(actual, status);  // actual cleared, status.tokens updated

    // check first token
    tokenize_text_check(actual, status.tokens[0], expected_1);

    // push back content of content_2
    for (size_t i = 0; i < strlen(content_2); ++i) {
        actual.push_back(content_2[i]);
    }

    tokenize_text(actual, status);

    // only 2 tokens
    EXPECT_EQ(status.tokens.size(), static_cast<size_t>(2));

    // check second token
    tokenize_text_check(actual, status.tokens[1], expected_2);
    // check content of first token to test if moving worked correctly
    check_token(status.tokens[0].name, symbol_t::TEXT,
                status.tokens[0].content, expected_1);
}

////////////////////////////////////////////////////////////////////////
// process_char TESTS
////////////////////////////////////////////////////////////////////////

TEST_F(lexer_routines_fixture, process_char_newline)
{
    static constexpr const char* content =
        "\t some text  "
        ;
    static constexpr const char* expected =
        "some text";

    std::string actual(content);
    status_t status; 
    bool res = process_char('\n', actual, status);
    process_char_check(res, status, actual, expected, symbol_t::NEWLINE);
}

TEST_F(lexer_routines_fixture, process_char_semicolon)
{
    static constexpr const char* content =
        "\v\t some text  \r\v\f \v"
        ;
    static constexpr const char* expected =
        "some text";

    std::string actual(content);
    status_t status; 
    bool res = process_char(';', actual, status);
    process_char_check(res, status, actual, expected, symbol_t::SEMICOLON);
}

TEST_F(lexer_routines_fixture, process_char_open_brace)
{
    static constexpr const char* content =
        " \v  some text  \v"
        ;
    static constexpr const char* expected =
        "some text";

    std::string actual(content);
    status_t status; 
    bool res = process_char('{', actual, status);
    process_char_check(res, status, actual, expected, symbol_t::OPEN_BRACE);
}

TEST_F(lexer_routines_fixture, process_char_close_brace)
{
    static constexpr const char* content =
        " \v  some text  \v"
        ;
    static constexpr const char* expected =
        "some text";

    std::string actual(content);
    status_t status; 
    bool res = process_char('}', actual, status);
    process_char_check(res, status, actual, expected, symbol_t::CLOSE_BRACE);
}

TEST_F(lexer_routines_fixture, process_char_default)
{
    static constexpr const char* content =
        " \v  some text  \v"
        ;
    static constexpr const char* expected = content;

    std::string actual(content);
    status_t status; 
    bool res = process_char('a', actual, status);

    EXPECT_FALSE(res);
    EXPECT_EQ(actual, expected);
    EXPECT_EQ(status.tokens.size(), static_cast<size_t>(0));
}

////////////////////////////////////////////////////////////////////////
// tokenize_tag_name TESTS
////////////////////////////////////////////////////////////////////////

TEST_F(lexer_routines_fixture, tokenize_tag_name_sdesc)
{
    static constexpr const char* content =
        "sdesc\t "
        ;
    write_file(content);
    file_reader reader(filename);
    status_t status;    // context is none
    tokenize_tag_name(reader, status);

    EXPECT_EQ(status.tokens.size(), static_cast<size_t>(1));
    check_token(status.tokens[0].name, symbol_t::TAGNAME,
                status.tokens[0].content, "sdesc");
    EXPECT_EQ(reader.peek(), '\t');
}

TEST_F(lexer_routines_fixture, tokenize_tag_name_param)
{
    static constexpr const char* content =
        "param\n \t "
        ;
    write_file(content);
    file_reader reader(filename);
    status_t status;    // context is none
    tokenize_tag_name(reader, status);

    EXPECT_EQ(status.tokens.size(), static_cast<size_t>(1));
    check_token(status.tokens[0].name, symbol_t::TAGNAME,
                status.tokens[0].content, "param");
    EXPECT_EQ(reader.peek(), '\n');
}

TEST_F(lexer_routines_fixture, tokenize_tag_name_tparam)
{
    static constexpr const char* content =
        "tparam\n \t "
        ;
    write_file(content);
    file_reader reader(filename);
    status_t status;    // context is none
    tokenize_tag_name(reader, status);

    EXPECT_EQ(status.tokens.size(), static_cast<size_t>(1));
    check_token(status.tokens[0].name, symbol_t::TAGNAME,
                status.tokens[0].content, "tparam");
    EXPECT_EQ(reader.peek(), '\n');
}

TEST_F(lexer_routines_fixture, tokenize_tag_name_invalid)
{
    static constexpr const char* content =
        "tparram\n \t "
        ;
    write_file(content);
    file_reader reader(filename);
    status_t status;    // context is none
    tokenize_tag_name(reader, status);

    EXPECT_EQ(status.tokens.size(), static_cast<size_t>(2));
    check_token(status.tokens[0].name, symbol_t::TEXT,
                status.tokens[0].content, "@");
    check_token(status.tokens[1].name, symbol_t::TEXT,
                status.tokens[1].content, "tparram");
    EXPECT_EQ(reader.peek(), '\n');
}

TEST_F(lexer_routines_fixture, tokenize_tag_name_eof)
{
    static constexpr const char* content =
        "tparam"
        ;
    write_file(content);
    file_reader reader(filename);
    status_t status;    // context is none
    tokenize_tag_name(reader, status);

    EXPECT_EQ(status.tokens.size(), static_cast<size_t>(1));
    check_token(status.tokens[0].name, symbol_t::TAGNAME,
                status.tokens[0].content, "tparam");
    EXPECT_EQ(reader.peek(), static_cast<int>(file_reader::termination));
}

////////////////////////////////////////////////////////////////////////
// process_tag_name TESTS
////////////////////////////////////////////////////////////////////////

TEST_F(lexer_routines_fixture, process_tag_name_valid)
{
    static constexpr const char* content =
        "param x\tsome int\n"
        ;
    static constexpr const char* text_content =
        "  some existing text... \n";
    static constexpr const char* expected_text =
        "some existing text...";

    write_file(content);
    file_reader reader(filename);
    status_t status;
    std::string text(text_content);
    bool res = process_tag_name('@', text, reader, status);

    EXPECT_TRUE(res);
    EXPECT_EQ(status.tokens.size(), static_cast<size_t>(2));
    tokenize_text_check(text, status.tokens[0], expected_text);
    check_token(status.tokens[1].name, symbol_t::TAGNAME,
                status.tokens[1].content, "param");
}

TEST_F(lexer_routines_fixture, process_tag_name_invalid)
{
    static constexpr const char* content =
        "xparam x\tsome int\n"
        ;
    static constexpr const char* text_content =
        "  some existing text... \n";

    write_file(content);
    file_reader reader(filename);
    status_t status;
    std::string text(text_content);
    bool res = process_tag_name('m', text, reader, status);

    EXPECT_FALSE(res);
    EXPECT_EQ(status.tokens.size(), static_cast<size_t>(0));
}

////////////////////////////////////////////////////////////////////////
// process_line_comment TESTS
////////////////////////////////////////////////////////////////////////

TEST_F(lexer_routines_fixture, process_line_comment_valid)
{
    static constexpr const char* content =
        "/ some special content...\n"
        ;
    static constexpr const char* text_content =
        "\n  some text...   \t";
    static constexpr const char* expected_text =
        "some text...";

    write_file(content);
    file_reader reader(filename);
    status_t status;
    std::string text(text_content);
    process_line_comment(text, reader, status);

    EXPECT_EQ(status.tokens.size(), static_cast<size_t>(2));
    tokenize_text_check(text, status.tokens[0], expected_text);
    check_token(status.tokens[1].name, symbol_t::BEGIN_LINE_COMMENT,
                status.tokens[1].content, "");
}

TEST_F(lexer_routines_fixture, process_line_comment_invalid_nospace)
{
    static constexpr const char* content =
        "/some special content...\n"
        ;
    static constexpr const char* text_content =
        "\n  some text...   \t";

    write_file(content);
    file_reader reader(filename);
    status_t status;
    std::string text(text_content);
    process_line_comment(text, reader, status);

    EXPECT_EQ(status.tokens.size(), static_cast<size_t>(0));
    EXPECT_EQ(text, text_content);  // text unchanged
}

TEST_F(lexer_routines_fixture, process_line_comment_invalid_noslash)
{
    static constexpr const char* content =
        " some special content...\n"
        ;
    static constexpr const char* text_content =
        "\n  some text...   \t";

    write_file(content);
    file_reader reader(filename);
    status_t status;
    std::string text(text_content);
    process_line_comment(text, reader, status);

    EXPECT_EQ(status.tokens.size(), static_cast<size_t>(0));
    EXPECT_EQ(text, text_content);  // text unchanged
}

////////////////////////////////////////////////////////////////////////
// process_block_comment TESTS
////////////////////////////////////////////////////////////////////////

TEST_F(lexer_routines_fixture, process_block_comment_valid)
{
    static constexpr const char* content =
        "! some special content...\n"
        ;
    static constexpr const char* text_content =
        "\n  some text...   \t";
    static constexpr const char* expected_text =
        "some text...";

    write_file(content);
    file_reader reader(filename);
    status_t status;
    std::string text(text_content);
    process_block_comment(text, reader, status);

    EXPECT_EQ(status.tokens.size(), static_cast<size_t>(2));
    tokenize_text_check(text, status.tokens[0], expected_text);
    check_token(status.tokens[1].name, symbol_t::BEGIN_BLOCK_COMMENT,
                status.tokens[1].content, "");
}

TEST_F(lexer_routines_fixture, process_block_comment_invalid_nospace)
{
    static constexpr const char* content =
        "!some special content...\n"
        ;
    static constexpr const char* text_content =
        "\n  some text...   \t";

    write_file(content);
    file_reader reader(filename);
    status_t status;
    std::string text(text_content);
    process_block_comment(text, reader, status);

    EXPECT_EQ(status.tokens.size(), static_cast<size_t>(0));
    EXPECT_EQ(text, text_content);  // text unchanged
}

TEST_F(lexer_routines_fixture, process_block_comment_invalid_noexclam)
{
    static constexpr const char* content =
        " some special content...\n"
        ;
    static constexpr const char* text_content =
        "\n  some text...   \t";

    write_file(content);
    file_reader reader(filename);
    status_t status;
    std::string text(text_content);
    process_block_comment(text, reader, status);

    EXPECT_EQ(status.tokens.size(), static_cast<size_t>(0));
    EXPECT_EQ(text, text_content);  // text unchanged
}

////////////////////////////////////////////////////////////////////////
// process_tags TESTS
////////////////////////////////////////////////////////////////////////

TEST_F(lexer_routines_fixture, process_string_invalid_comment)
{
    static constexpr const char* content =
        "some content...\n "
        ;
    static constexpr const char* text_content = 
        " some text...     ";

    write_file(content);
    file_reader reader(filename);
    status_t status;    // context is none
    std::string text(text_content);
    bool res = process_string('/', text, reader, status);

    EXPECT_TRUE(res);
    EXPECT_EQ(status.tokens.size(), static_cast<size_t>(0));
    EXPECT_EQ(text, std::string(text_content) + "/s");
}

TEST_F(lexer_routines_fixture, process_string_invalid_slash)
{
    static constexpr const char* content =
        "some content...\n "
        ;
    static constexpr const char* text_content = 
        " some text...     ";

    write_file(content);
    file_reader reader(filename);
    status_t status;    // context is none
    std::string text(text_content);
    bool res = process_string('x', text, reader, status);

    EXPECT_FALSE(res);
    EXPECT_EQ(status.tokens.size(), static_cast<size_t>(0));
    EXPECT_EQ(text, text_content);
}

////////////////////////////////////////////////////////////////////////
// process TESTS
////////////////////////////////////////////////////////////////////////

TEST_F(lexer_routines_fixture, process)
{
    static constexpr const char* content =
        "#include <nlohmann/json.hpp> // for json\n"
        "// this is some comment to ignore\n"
        "/* this is another comment to ignore \n*/"
        "\n"
        "   /// description...  @sdesc  some short description\n"
        " /*!  @param x some int\n"
        "   * that we care about\n"
        "   */"
        "inline f(int x);"
        "struct A {const char* p = \"@param\"};"
        ;

    write_file(content);
    file_reader reader(filename);
    status_t status;    // context is none
    process(reader, status);

    EXPECT_EQ(status.tokens.size(), static_cast<size_t>(32));
    check_token(status.tokens[0].name, symbol_t::TEXT,
                status.tokens[0].content, "#include <nlohmann/json.hpp>");
    check_token(status.tokens[1].name, symbol_t::NEWLINE,
                status.tokens[1].content, "");
    check_token(status.tokens[2].name, symbol_t::TEXT,
                status.tokens[2].content, "");
    EXPECT_EQ(status.tokens[2].leading_ws_count, static_cast<uint32_t>(3));

    check_token(status.tokens[3].name, symbol_t::BEGIN_LINE_COMMENT,
                status.tokens[3].content, "");
    check_token(status.tokens[4].name, symbol_t::TEXT,
                status.tokens[4].content, "description...");
    EXPECT_EQ(status.tokens[4].leading_ws_count, static_cast<uint32_t>(1));

    check_token(status.tokens[5].name, symbol_t::TAGNAME,
                status.tokens[5].content, "sdesc");
    check_token(status.tokens[6].name, symbol_t::TEXT,
                status.tokens[6].content, "some short description");
    EXPECT_EQ(status.tokens[6].leading_ws_count, static_cast<uint32_t>(2));
    check_token(status.tokens[7].name, symbol_t::NEWLINE,
                status.tokens[7].content, "");

    check_token(status.tokens[8].name, symbol_t::TEXT,
                status.tokens[8].content, "");
    EXPECT_EQ(status.tokens[8].leading_ws_count, static_cast<uint32_t>(1));
    check_token(status.tokens[9].name, symbol_t::BEGIN_BLOCK_COMMENT,
                status.tokens[9].content, "");
    check_token(status.tokens[10].name, symbol_t::TEXT,
                status.tokens[10].content, "");
    EXPECT_EQ(status.tokens[10].leading_ws_count, static_cast<uint32_t>(2));

    check_token(status.tokens[11].name, symbol_t::TAGNAME,
                status.tokens[11].content, "param");
    check_token(status.tokens[12].name, symbol_t::TEXT,
                status.tokens[12].content, "x some int");
    EXPECT_EQ(status.tokens[12].leading_ws_count, static_cast<uint32_t>(1));

    check_token(status.tokens[13].name, symbol_t::NEWLINE,
                status.tokens[13].content, "");
    check_token(status.tokens[14].name, symbol_t::TEXT,
                status.tokens[14].content, "");
    EXPECT_EQ(status.tokens[14].leading_ws_count, static_cast<uint32_t>(3));
    check_token(status.tokens[15].name, symbol_t::STAR,
                status.tokens[15].content, "");

    check_token(status.tokens[16].name, symbol_t::TEXT,
                status.tokens[16].content, "that we care about");
    EXPECT_EQ(status.tokens[16].leading_ws_count, static_cast<uint32_t>(1));
    check_token(status.tokens[17].name, symbol_t::NEWLINE,
                status.tokens[17].content, "");

    check_token(status.tokens[18].name, symbol_t::TEXT,
                status.tokens[18].content, "");
    EXPECT_EQ(status.tokens[18].leading_ws_count, static_cast<uint32_t>(3));
    check_token(status.tokens[19].name, symbol_t::END_BLOCK_COMMENT,
                status.tokens[19].content, "");

    check_token(status.tokens[20].name, symbol_t::TEXT,
                status.tokens[20].content, "inline f(int x)");
    check_token(status.tokens[21].name, symbol_t::SEMICOLON,
                status.tokens[21].content, "");
    check_token(status.tokens[22].name, symbol_t::TEXT,
                status.tokens[22].content, "struct A");
    check_token(status.tokens[23].name, symbol_t::OPEN_BRACE,
                status.tokens[23].content, "");
    check_token(status.tokens[24].name, symbol_t::TEXT,
                status.tokens[24].content, "const char");
    check_token(status.tokens[25].name, symbol_t::STAR,
                status.tokens[25].content, "");
    check_token(status.tokens[26].name, symbol_t::TEXT,
                status.tokens[26].content, "p = \"");
    check_token(status.tokens[27].name, symbol_t::TAGNAME,
                status.tokens[27].content, "param");
    check_token(status.tokens[28].name, symbol_t::TEXT,
                status.tokens[28].content, "\"");
    check_token(status.tokens[29].name, symbol_t::CLOSE_BRACE,
                status.tokens[29].content, "");
    check_token(status.tokens[30].name, symbol_t::SEMICOLON,
                status.tokens[30].content, "");
    check_token(status.tokens[31].name, symbol_t::END_OF_FILE,
                status.tokens[31].content, "");
}

} // namespace core
} // namespace docgen
