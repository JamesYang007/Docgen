#pragma once

#include "parser.hpp"

struct TagParser_routines
{
	protected:
		static const Parser::routine_t on_open_r_, on_close_r_;
};

const Parser::routine_t TagParser_routines::on_open_r_ = [](Parser *p) {
	if (Parser::on() && !Parser::fresh()) {
		Parser::chop(p->just_matched().length());
		Parser::trim();
	}
	Parser::at(p->just_matched().substr(1, p->just_matched().length() - 2));
	Parser::stop();
};

const Parser::routine_t TagParser_routines::on_close_r_ = [](Parser *p) {
	Parser::go();
	if (Parser::fed()) {
		Parser::force_feed(' ');
	}
	Parser::force_feed(p->just_matched());
};

class TagParser : public Parser, private TagParser_routines
{
	public:
		TagParser(char open, char close)
			: Parser((std::string("\\") + open + std::string("\\S*") + std::string("\\") + close).c_str(), "[\\S\\n]", on_open_r_, nullptr, on_close_r_)
		{}
};
