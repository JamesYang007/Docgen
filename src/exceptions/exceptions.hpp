#pragma once

#include <cstdio>
#include <cerrno>
#include <string>
#include <exception>

namespace docgen {
namespace exceptions {

class exception : public std::exception
{
public:
    explicit exception(const std::string& msg) noexcept
        : msg_(prefix_ + msg)
    {}

    virtual const char *what() const noexcept
    {
        return msg_.c_str();
    }

private:
    static constexpr const char *prefix_ = "Docgen encountered a problem: ";
    std::string msg_;
};

class system_error : public exception
{
public:
    explicit system_error(const std::string& msg) noexcept
        : exception(msg + ": " + std::strerror(errno))
    {}
};

class control_flow_error : public exception
{
public:
    explicit control_flow_error(const std::string& msg) noexcept
        : exception("control flow error: " + msg)
    {}
};

class bad_flags : public exception
{
public:
    bad_flags() noexcept
        : exception("bad flags")
    {}
};

} // namespace exceptions
} // namespace docgen
