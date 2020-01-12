#include <nlohmann/json.hpp>
#include "parse_feeder.hpp"

namespace docgen {
namespace core {

nlohmann::json ParseFeeder::parsed_, ParseFeeder::parsing_;
std::string ParseFeeder::key_;
bool ParseFeeder::writing_ = false,
     ParseFeeder::to_skip_ = false,
     ParseFeeder::just_written_ = false;

} // namespace core
} // namespace docgen
