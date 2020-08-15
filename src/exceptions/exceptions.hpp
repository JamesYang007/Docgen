#pragma once

#include <cstdio>
#include <cerrno>
#include <cstring>
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
		system_error(const std::string& msg, int errnum) noexcept
			: exception(msg + ": " + strerror(errnum))
		{}

		explicit system_error(const std::string& msg) noexcept
			: system_error(msg, errno)
		{}
};

class control_flow_error : public exception
{
	public:
		explicit control_flow_error(const std::string& msg) noexcept
			: exception("control flow error: " + msg)
		{}
};

class file_open_error : public system_error
{
	public:
		file_open_error(const std::string& path, int errnum) noexcept
			: system_error("failed to open file at path \"" + std::string(path) + '\"', errnum)
		{}

		explicit file_open_error(const std::string& path) noexcept
			: file_open_error(path, errno)
		{}
};

class file_exist_error : public file_open_error
{
	public:
		explicit file_exist_error(const std::string& path) noexcept
			: file_open_error(path, ENOENT)
		{}
};

class bad_file : public exception
{
	public:
		explicit bad_file(const std::string& path) noexcept
			: exception('\"' + path + "\" is not a regular file")
		{}
};

class bad_source : public exception
{
	public:
		bad_source() noexcept
			: exception("no pertinent information was parsed; please ensure your files follow Docgen formatting guidelines")
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
