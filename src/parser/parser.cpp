#include <stdio.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <type_traits>
#include <vector>
#include <boost/regex.hpp>

struct Parser_globals {
	private:
		static nlohmann::json parsed_;
		static std::string key_;
		static bool writing_;
		static std::string& val_()
		{
			if (parsed_[key_].is_null()) {
				parsed_[key_] = "";
			}
			return parsed_[key_].get_ref<std::string&>();
		}
	public:
		static void at(const char *key)
		{
			key_ = key;
		}
		static void feed(char c)
		{
			if (writing_ && key_.length()) {
				val_().push_back(c);
			}
		}
		static void chop(size_t n)
		{
			if (key_.length()) {
				val_().resize(val_().length() >= n ? val_().length() - n : 0);
			}
		}
		static void trim()
		{
			if (key_.length()) {
				while (std::isspace(val_().back())) {
					val_().pop_back();
				}
			}
		}
		static bool on()
		{
			return writing_;
		}
		static void go()
		{
			writing_ = true;
		}
		static void stop()
		{
			writing_ = false;
		}
		static void dump()
		{
			std::cout << parsed_ << std::endl;
		}
};
nlohmann::json Parser_globals::parsed_;
std::string Parser_globals::key_;
bool Parser_globals::writing_ = false;

class Parser : public Parser_globals
{
	public:
		using on_open_t = void (*)();
		using while_betwixt_t = void (*)(char);
		using on_close_t = void (*)(const char *);

		Parser(const char *opener, const char *closer,
		       on_open_t on_open, while_betwixt_t while_betwixt, on_close_t on_close, bool to_store,
		       std::vector<Parser> parsers={})
			: opener_(opener), closer_(closer), to_store_(to_store),
			  on_open_(on_open), while_betwixt_(while_betwixt), on_close_(on_close),
			  parsers_(parsers)
		{}

		void proc(char c)
		{
			this->proc_(c);
		}

	private:
		const boost::regex opener_, closer_;
		const boost::regex *cur_ = &opener_;
		std::string partial_;
		boost::cmatch matcher_;

		bool to_store_;
		std::string stored_;

		on_open_t on_open_;
		while_betwixt_t while_betwixt_;
		on_close_t on_close_;

		std::vector<Parser> parsers_;

		bool betwixt_() { return cur_ == &closer_; }
		void reset_();
		bool proc_(char c);
};

bool Parser::proc_(char c)
{
	bool no_close = false;
	if (betwixt_()) {
		for (Parser& p : parsers_) {
			if (p.proc_(c)) {
				no_close = true;
			}
		}
		if (to_store_) {
			stored_.push_back(c);	
		}
		if (while_betwixt_) {
			while_betwixt_(c);
		}
	}

	partial_.push_back(c);
	if (!boost::regex_match(partial_.data(), matcher_, *cur_, boost::match_default | boost::match_partial)) {
		partial_.clear();
		return no_close;
	}
	if (matcher_[0].matched) {
		if (!betwixt_()) {
			cur_ = &closer_;
			if (on_open_) {
				on_open_();
			}
		}
		else if (!no_close) {
			if (to_store_) {
				stored_.resize(stored_.length() - partial_.length());
				on_close_(stored_.data());
			}
			else {
				on_close_(&c);
			}
			stored_.clear();
			cur_ = &opener_;
			for (Parser& p : parsers_) {
				p.reset_();
			}
		}
		partial_.clear();
	}
	return no_close;
}

void Parser::reset_()
{
	cur_ = &opener_;	
	partial_.clear();
	stored_.clear();
}

int main()
{
	Parser tag_parser("\\[", "\\] ", nullptr, nullptr, [](const char *s){if (Parser::on()) { Parser::chop(3+std::strlen(s)); Parser::trim(); } Parser::at(s); Parser::go();}, true);
	Parser ignore_parser("\\n", "[^[:space:]\\*]", [](){Parser::stop(); Parser::chop(1);}, nullptr, [](const char *s){Parser::go(); Parser::feed(' '); Parser::feed(*s);}, false);
	Parser block_parser("/\\*", "\\*/", nullptr, nullptr, [](const char *s){if (Parser::on()) { Parser::chop(2); Parser::trim(); } Parser::stop();}, false, {ignore_parser, tag_parser});

	char buf[1000];
	size_t r;
	while ((r = fread(buf, 1, sizeof(buf), stdin)) > 0) {
		for (size_t i = 0; i < r; i++) {
			Parser::feed(buf[i]);
			block_parser.proc(buf[i]);
		}
	}
	Parser::dump();
	return 0;
}
