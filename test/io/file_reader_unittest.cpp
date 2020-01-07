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
        // actual
        file_reader reader(filepath);
        std::string actual;
        while (reader.peek() != EOF) {
            actual.push_back(reader.read());
        }

        // expected 
        FILE* fp = fopen(filepath, "r");
        fseek(fp, 0L, SEEK_END);
        std::string expected(ftell(fp), 0);
        rewind(fp);
        fread(&expected[0], sizeof(char), expected.size(), fp);
        fclose(fp);

        EXPECT_EQ(actual, expected);
    }
};

TEST_F(file_reader_fixture, ctor)
{
    file_reader reader(empty_filepath);
}

TEST_F(file_reader_fixture, peek_empty)
{
    file_reader reader(empty_filepath);
    EXPECT_EQ(reader.peek(), EOF);
}

TEST_F(file_reader_fixture, read_empty_multiple)
{
    file_reader reader(empty_filepath);
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
