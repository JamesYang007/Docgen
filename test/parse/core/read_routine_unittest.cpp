#include "routine_fixture.hpp"

namespace docgen {
namespace parse {
namespace core {

/////////////////////////////////////////////////////////////////
// process_slash TEST
/////////////////////////////////////////////////////////////////

TEST_F(routine_fixture, slash_end_batch)
{
    process_slash_setup();
    const char* const text = "";
    text_setup(text);
    next_routine = details::process_slash(status, begin, end); 
    check_routine(Routine::READ, State::DEFAULT, "/", text);
}

TEST_F(routine_fixture, slash_single_line)
{
    process_slash_setup();
    const char* const text = "/ comment";
    text_setup(text);
    next_routine = details::process_slash(status, begin, end); 
    check_routine(Routine::IGNORE_WS, State::SINGLE_LINE, "", text);
}

TEST_F(routine_fixture, slash_block)
{
    process_slash_setup();
    const char* const text = "* comment";
    text_setup(text);
    next_routine = details::process_slash(status, begin, end); 
    check_routine(Routine::IGNORE_WS, State::BLOCK, "", text);
}

TEST_F(routine_fixture, slash_default)
{
    process_slash_setup();
    const char* const text = "a comment";
    text_setup(text);
    next_routine = details::process_slash(status, begin, end); 
    check_routine(Routine::READ, State::DEFAULT, "", text);
}

/////////////////////////////////////////////////////////////////
// routine<Routine::READ> TEST
/////////////////////////////////////////////////////////////////

// Slash-match is when symbol is "/".

// Test case slash-match: 
TEST_F(routine_fixture, read_slash_match_end_batch)
{
    process_slash_setup();
    const char* const text = "";
    text_setup(text);
    next_routine = routine<Routine::READ>::run(status, begin, end); 
    check_routine(Routine::READ, State::DEFAULT, "/", text);
}

TEST_F(routine_fixture, read_slash_match_single_line)
{
    process_slash_setup();
    const char* const text = "/ comment";
    text_setup(text);
    next_routine = routine<Routine::READ>::run(status, begin, end); 
    check_routine(Routine::IGNORE_WS, State::SINGLE_LINE, "", text + 1);
}

TEST_F(routine_fixture, read_slash_match_block)
{
    process_slash_setup();
    const char* const text = "* comment";
    text_setup(text);
    next_routine = routine<Routine::READ>::run(status, begin, end); 
    check_routine(Routine::IGNORE_WS, State::BLOCK, "", text + 1);
}

TEST_F(routine_fixture, read_slash_match_default)
{
    process_slash_setup();
    const char* const text = "a comment";
    text_setup(text);
    next_routine = routine<Routine::READ>::run(status, begin, end); 
    check_routine(Routine::READ, State::DEFAULT, "", text + strlen(text));
}

// Test case no slash-match:
TEST_F(routine_fixture, read_no_slash_match_begin_eq_end)
{
    const char* const text = "";
    text_setup(text);
    next_routine = routine<Routine::READ>::run(status, begin, end);
    check_routine(Routine::READ, State::DEFAULT, "", text);
}

TEST_F(routine_fixture, read_no_slash_match_single_line)
{
    const char* const text = "#include <nlohmann/json.hpp> // json";
    text_setup(text);
    next_routine = routine<Routine::READ>::run(status, begin, end);
    const char* new_begin = strrchr(text, ' ');
    check_routine(Routine::IGNORE_WS, State::SINGLE_LINE, "", new_begin);
}

TEST_F(routine_fixture, read_no_slash_match_block)
{
    const char* const text = "#include <nlohmann/json.hpp> /* json";
    text_setup(text);
    next_routine = routine<Routine::READ>::run(status, begin, end);
    const char* new_begin = strrchr(text, ' ');
    check_routine(Routine::IGNORE_WS, State::BLOCK, "", new_begin);
}

TEST_F(routine_fixture, read_no_slash_match_no_comment)
{
    const char* const text = "#include <nlohmann/json.hpp> / json ";
    text_setup(text);
    next_routine = routine<Routine::READ>::run(status, begin, end);
    check_routine(Routine::READ, State::DEFAULT, "", text + strlen(text));
}

TEST_F(routine_fixture, read_no_slash_match_no_comment_end_slash)
{
    const char* const text = "#include <nlohmann/json.hpp> / json /";
    text_setup(text);
    next_routine = routine<Routine::READ>::run(status, begin, end);
    check_routine(Routine::READ, State::DEFAULT, "/", text + strlen(text));
}

} // namespace core
} // namespace parse
} // namespace docgen
