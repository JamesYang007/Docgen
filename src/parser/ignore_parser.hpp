#pragma once

#include "parser.hpp"

struct IgnoreParser_routines {
	protected:
		static const Parser::routine_t on_open_r_, on_close_r_;
};

const Parser::routine_t IgnoreParser_routines::on_open_r_ = [](Parser *p) {
	if (Parser::on() && !Parser::fresh()) {
		Parser::chop(p->just_matched().length());
		Parser::trim();
	}
	Parser::stop();
};

const Parser::routine_t IgnoreParser_routines::on_close_r_ = [](Parser *p) {
	Parser::go();
	if (Parser::fed()) {
		Parser::force_feed(' ');
	}
	Parser::force_feed(p->just_matched());
};

class IgnoreParser : public Parser, private IgnoreParser_routines {
	public:
		IgnoreParser(const char *from, const char *to)
			: Parser(from, to, on_open_r_, nullptr, on_close_r_)
	{}
};
