#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "token.hpp"
#include "symbol.hpp"

namespace docgen {
namespace core {

struct ParseFeeder {
	private:
		static nlohmann::json parsed_, parsing_;
		static std::string key_;
		static bool writing_;
		static bool to_skip_;
		static std::string *val_()
		{
			if (parsing_[key_].is_null()) {
				parsing_[key_] = "";
			}
			return parsing_[key_].get_ptr<std::string *>();
		}
		static void short_reset()
		{
			parsing_.clear();
			key_.clear();
			writing_ = false;
			to_skip_ = false;
		}

	public:
		static void go() { writing_ = true; }
		static void stop() { writing_ = false; }
		static void skip() { to_skip_ = true; }
		static void at(const std::string &key) { trim(); key_ = key; }
		static bool at() { return !key_.empty(); }
		static bool on() { return writing_ && at(); }
		static bool open() { return on() && !to_skip_; }
		static bool fed() { return at() && !val_()->empty(); }

		template <class StringType>
		static void force_feed(StringType s)
		{
			if (at()) {
				val_()->append(s);
			}
		}
		static void force_feed(char c)
		{
			if (at()) {
				val_()->push_back(c);
			}
		}

		using token_t = Token<Symbol>;

		static bool feed(const token_t& t)
		{
			if (open()) {
				if (fed() && t.leading_ws_count) {
					force_feed<std::string>(std::string(t.leading_ws_count, ' '));
				}
				force_feed<const char *>(t.c_str());
			}
			to_skip_ = false;
			return fed();
		}

		static void trim()
		{
			if (at()) {
				while (std::isspace(val_()->back())) {
					val_()->pop_back();
				}
				if (val_()->empty()) {
					parsing_.erase(key_);
				}
			}
		}

		static void reset()
		{
			parsed_.clear();
			short_reset();
		}

		static nlohmann::json&& move() {
			trim();
			parsed_["functions"].push_back(std::move(parsing_));
			short_reset();
			return std::move(parsed_);
		}
};

nlohmann::json ParseFeeder::parsed_, ParseFeeder::parsing_;
std::string ParseFeeder::key_;
bool ParseFeeder::writing_ = false, ParseFeeder::to_skip_ = false;

} // core
} // docgen
