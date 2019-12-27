#pragma once

#include "docgen.hpp"

static inline void from_parsed_global()
{
	if (parsed_dst) {
		*parsed_dst << parsed;
		if (parsed_dst->fail()) {
			perror("std::ostream write failed");
			status = DG_SYS_ERR;
		}
		return;
	}

	// generate docs (writing files as necessary) from global json parsed
	// set global status on failure
}
