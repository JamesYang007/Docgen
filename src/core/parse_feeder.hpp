#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "token.hpp"
#include "symbol.hpp"

namespace docgen {
namespace core {

class ParseFeeder {
	public:
		using token_t = Token<Symbol>;

		template <class StringType>
		void force_feed(const StringType& s);
		void force_feed(char c);
		bool feed(const token_t& t);
		void trim();
		void reset();
		const nlohmann::json& parsed();

		void go() { writing_ = true; just_written_ = false; }
		void stop() { writing_ = false; }
		void skip() { to_skip_ = true; }
		void at(const std::string& key) { trim(); key_ = key; just_written_ = false; }
		bool at() { return !key_.empty(); }
		bool on() { return writing_ && at(); }
		bool open() { return on() && !to_skip_; }
		bool fed() { return at() && !val_()->empty(); }
		bool fresh() { return !just_written_; }

	private:
		nlohmann::json parsed_, parsing_;
		std::string key_;
		bool writing_ = false,
		     just_written_ = false,
		     to_skip_ = false;

		std::string *val_()
		{
			if (parsing_[key_].is_null()) {
				parsing_[key_] = "";
			}
			return parsing_[key_].get_ptr<std::string *>();
		}
};


template <class StringType>
inline void ParseFeeder::force_feed(const StringType& s)
{
	if (at()) {
		val_()->append(s);
	}
}

inline void ParseFeeder::force_feed(char c)
{
	if (at()) {
		val_()->push_back(c);
	}
}

inline bool ParseFeeder::feed(const ParseFeeder::token_t& t)
{
	if (open()) {
		if (fed()) {
			if (fresh()) {
				force_feed(' ');
			}
			else if (t.leading_ws_count) {
				force_feed(std::string(t.leading_ws_count, ' '));
			}
		}
		force_feed(t.c_str());
		just_written_ = true;
	}
	to_skip_ = false;
	return fed();
}

inline void ParseFeeder::trim()
{
	if (at()) {
		while (!val_()->empty() && std::isspace(val_()->back())) {
			val_()->pop_back();
		}
		if (val_()->empty()) {
			parsing_.erase(key_);
		}
	}
}

inline void ParseFeeder::reset()
{
	parsed_.clear();
	parsing_.clear();
	key_.clear();
	writing_ = false;
	just_written_ = false;
	to_skip_ = false;
}

inline const nlohmann::json& ParseFeeder::parsed()
{
	trim();
	if (!parsing_.is_null()) {
		parsed_["functions"].push_back(std::move(parsing_));
		parsing_.clear();
	}
	return parsed_;
}

} // core
} // docgen
