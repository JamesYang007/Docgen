#include "io/file_reader.hpp"
#include <gtest/gtest.h>
#include <unistd.h>

namespace docgen {
namespace io {

struct file_reader_fixture : ::testing::Test
{
protected:
    static constexpr const char* empty_filepath = "io_data/empty.txt";
    static constexpr const char* one_line_filepath = "io_data/one_line.txt";
    static constexpr const char* paragraph_filepath = "io_data/paragraph.txt";

    void read_test(const char* filepath)
    {
        FILE* file = fopen(filepath, "r+");

        // actual
        file_reader reader(file);
        std::string actual;
        while (reader.peek() != EOF) {
            actual.push_back(reader.read());
        }

        // expected 
        fseek(file, 0L, SEEK_END);
        std::string expected(ftell(file), 0);
        rewind(file);
        fread(&expected[0], sizeof(char), expected.size(), file);

        fclose(file);

        EXPECT_EQ(actual, expected);
    }
};

TEST_F(file_reader_fixture, ctor)
{
    FILE* file = fopen(empty_filepath, "r");
    file_reader reader(file);
    fclose(file);
}

TEST_F(file_reader_fixture, peek_empty)
{
    FILE* file = fopen(empty_filepath, "r");
    file_reader reader(file);
    fclose(file);
    EXPECT_EQ(reader.peek(), EOF);
}

TEST_F(file_reader_fixture, read_empty_multiple)
{
    FILE* file = fopen(empty_filepath, "r");
    file_reader reader(file);
    fclose(file);
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(reader.read(), EOF);
    }
}

TEST_F(file_reader_fixture, read_empty)
{
    read_test(empty_filepath);
}

TEST_F(file_reader_fixture, read_one_line)
{
    read_test(one_line_filepath);
}

TEST_F(file_reader_fixture, read_paragraph)
{
    read_test(paragraph_filepath);
}

} // namespace io
} // namespace docgen
