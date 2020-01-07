#include "io/string_reader.hpp"
#include <gtest/gtest.h>

namespace docgen {
namespace io {

struct string_reader_fixture : ::testing::Test
{
};

TEST_F(string_reader_fixture, read) 
{
    static constexpr const char* content =
        "hello world!"
        ;

    string_reader reader(content);
    std::string actual;
    int c = 0;
    while ((c = reader.read()) != string_reader::termination) {
        actual.push_back(c);
    }
    EXPECT_EQ(actual, content);
}

TEST_F(string_reader_fixture, read_past_string) 
{
    static constexpr const char* content =
        "hello world!"
        ;

    string_reader reader(content);
    std::string actual;
    int c = 0;
    while ((c = reader.read()) != string_reader::termination) {
        actual.push_back(c);
    }
    // unnecessary read should still return 0
    EXPECT_EQ(reader.read(), 0);
}

TEST_F(string_reader_fixture, back)
{
    static constexpr const char* content =
        "hello world!"
        ;

    string_reader reader(content);
    std::string actual;
    int c = 0;
    while ((c = reader.read()) != string_reader::termination) {
        actual.push_back(c);
    }
    reader.back(c);
    actual.push_back(reader.read());
    std::string expected = std::string(content) + "!";
    EXPECT_EQ(actual, expected);
}

TEST_F(string_reader_fixture, peek)
{
    static constexpr const char* content =
        "hello world!"
        ;

    string_reader reader(content);

    EXPECT_EQ(reader.peek(), 'h');

    std::string actual;
    int c = 0;

    // peeking should not affect cursor
    while ((c = reader.read()) != string_reader::termination) {
        actual.push_back(c);
    }
    EXPECT_EQ(actual, content);
}

} // namespace io
} // namespace docgen
