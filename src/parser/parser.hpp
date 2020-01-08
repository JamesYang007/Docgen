#pragma once

#include <string>
#include <vector>
#include <boost/regex.hpp>
#include <nlohmann/json.hpp>

struct Parser_globals {
	private:
		static nlohmann::json parsed_, funcs_parsed_, classes_parsed_;
		static std::string key_;
		static bool writing_;
		static bool just_written_;
		static std::string *val_()
		{
			if (parsed_[key_].is_null()) {
				parsed_[key_] = "";
			}
			return parsed_[key_].get_ptr<std::string *>();
		}

	public:
		static void go() { writing_ = true; }
		static void stop() { writing_ = false; just_written_ = false; }
		static void at(const std::string &key) { key_ = key; }
		static bool at() { return !key_.empty(); }
		static bool on() { return writing_ && at(); }
		static bool fresh() { return !just_written_; }
		static bool fed() { return at() && !val_()->empty(); }

		static void force_feed(const std::string &s)
		{
			if (at()) {
				val_()->append(s);
				just_written_ = true;
			}
		}
		static void force_feed(char c) {
			if (at()) {
				val_()->push_back(c);
				just_written_ = true;
			}
		}
		static void feed(char c) {
			if (on()) {
				if (fresh()) {
					if (std::isspace(c)) {
						return;
					}
					else if (fed()) {
						force_feed(' ');
					}
				}
				force_feed(c);
			}
		}
		static void chop(size_t n)
		{
			if (at()) {
				val_()->resize(val_()->length() >= n ? val_()->length() - n : 0);
			}
		}
		static void trim()
		{
			if (at()) {
				while (std::isspace(val_()->back())) {
					val_()->pop_back();
				}
			}
		}
		static void trim(const std::string& to_cut)
		{
			if (at()) {
				size_t i = to_cut.length()-1;
				while (i >= 0 && val_()->back() == to_cut[i--]) {
					val_()->pop_back();
				}
			}
		}
		static void reset()
		{
			parsed_.clear();
			key_.clear();
			writing_ = false;
			just_written_ = false;
		}
		static void dump()
		{
			std::cout << parsed_ << std::endl;
		}
};
nlohmann::json Parser_globals::parsed_;
std::string Parser_globals::key_;
bool Parser_globals::writing_ = false;
bool Parser_globals::just_written_ = false;

class Parser : public Parser_globals
{
	public:
		using routine_t = void (*)(Parser *p);

		Parser(const char *opener, const char *closer,
		       routine_t on_open, routine_t while_betwixt, routine_t on_close,
		       std::vector<Parser> children={})
			: opener_(opener), closer_(closer),
			  on_open_(on_open), while_betwixt_(while_betwixt), on_close_(on_close),
			  children_(children)
		{}

		void proc(char c);
		void reset();

		const std::string& just_matched() const { return partial_; }
		char last_proc() const { return partial_.back(); }

	private:
		const boost::regex opener_, closer_;
		const boost::regex *cur_ = &opener_;
		boost::cmatch matcher_;
		std::string partial_;

		routine_t on_open_, while_betwixt_, on_close_;

		std::vector<Parser> children_;
		Parser *child_betwixt_ = nullptr;

		void handle_children_(char c);
		bool betwixt_() { return cur_ == &closer_; }
};
