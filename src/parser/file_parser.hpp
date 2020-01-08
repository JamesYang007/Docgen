#pragma once

#include <stdio.h>
#include <nlohmann/json.hpp>
#include "../exceptions/exceptions.hpp"
#include "parser.hpp"

class FileParser : private Parser
{
	public:
		FileParser(const char *path, std::vector<Parser> workers, size_t buf_size=4096)
			: Parser(".", "\\0", nullptr, nullptr, nullptr, workers), buf_size_(buf_size)
		{
			if ((fp_ = std::fopen(path, "rb")) == nullptr) {
				throw docgen::exceptions::system_error("fopen() failed");
			}
			to_close = true;
		}
		FileParser(FILE *src, std::vector<Parser> workers, size_t buf_size=4096)
			: fp_(src), Parser(".", "\\0", nullptr, nullptr, nullptr, workers), buf_size_(buf_size)
		{}
		~FileParser()
		{
			if (to_close) {
				std::fclose(fp_);
			}
		}

		void go()
		{
			Parser::reset();
			std::rewind(fp_);
			
			char buf[buf_size_];
			size_t r, i;
			while ((r = std::fread(buf, 1, sizeof(buf), fp_)) > 0) {
				for (i = 0; i < r; i++) {
					Parser::feed(buf[i]);
					proc(buf[i]);
				}
			}
			if (ferror(fp_)) {
				throw docgen::exceptions::system_error("fread() failed");
			}
		}

	private:
		FILE *fp_;
		bool to_close = false;
		size_t buf_size_;
};
