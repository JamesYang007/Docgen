#include <exceptions/exceptions.hpp>
#include <gtest/gtest.h>

namespace docgen {
namespace exceptions {

struct exceptions_fixture : ::testing::Test
{
protected:
};

TEST_F(exceptions_fixture, exception_base)
{
    exception error("some error");
    std::string expected = "Docgen encountered a problem: some error";
    EXPECT_EQ(std::string(error.what()), expected);
}

TEST_F(exceptions_fixture, exception_system_error)
{
    system_error error("some error");
    std::string expected = "Docgen encountered a problem: some error: ";
    expected += std::strerror(errno);
    EXPECT_EQ(std::string(error.what()), expected);
}

TEST_F(exceptions_fixture, exception_control_flow_error)
{
    control_flow_error error("some error");
    std::string expected = "Docgen encountered a problem: control flow error: some error";
    EXPECT_EQ(std::string(error.what()), expected);
}

TEST_F(exceptions_fixture, exception_bad_flags)
{
    bad_flags error;
    std::string expected = "Docgen encountered a problem: bad flags";
    EXPECT_EQ(std::string(error.what()), expected);
}

} // namespace exceptions
} // namespace docgen
