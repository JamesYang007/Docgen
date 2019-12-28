#pragma once

#include "docgen.hpp"

static inline void to_parsed_global()
{
	if (parsed_src) {
		*parsed_src >> parsed;	
		if (parsed_src->fail()) {
			throw docgen::system_error("failed to read from istream");
		}
		return;
	}

	// parse files from global NULL-terminated list files_src_paths and write information to json parsed
	// set global status on failure
}
