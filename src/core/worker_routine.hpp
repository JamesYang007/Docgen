#pragma once

#include "parse_feeder.hpp"

namespace docgen {
namespace core {

struct WorkerRoutine
{
	using routine_t = void (*)(const ParseFeeder::token_t&, ParseFeeder&);
};

} // namespace core
} // namespace docgen
