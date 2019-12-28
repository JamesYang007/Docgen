#pragma once

#include <cstdio>
#include <cerrno>
#include <string>
#include <exception>

namespace docgen
{
	class exception : public std::exception
	{
		public:
			explicit exception(const std::string& msg)
				: msg_(prefix_ + msg)
			{}

			virtual const char *what() const throw()
			{
				return msg_.c_str();
			}

		private:
			static constexpr const char * prefix_ = "Docgen encountered a problem: ";
			std::string msg_;
	};

	class system_error : public exception
	{
		public:
			explicit system_error(const std::string& msg)
				: exception(msg + ": " + strerror(errno))
			{}
	};

	class bad_flags : public exception
	{
		public:
			bad_flags()
				: exception("bad flags")
			{}
	};
}
