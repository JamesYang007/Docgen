#pragma once

#include "parser.hpp"

struct CommentParser_routines
{
	protected:
		static const Parser::routine_t on_open_r_, on_close_r_;
};

const Parser::routine_t CommentParser_routines::on_open_r_ = [](Parser *) {
	if (Parser::at()) {
		Parser::go();
	}
};

const Parser::routine_t CommentParser_routines::on_close_r_ = [](Parser *p) {
	Parser::chop(p->just_matched().length());
	Parser::trim();
	Parser::stop();
};

class CommentParser : public Parser, private CommentParser_routines
{
	public:
		CommentParser(const char *start, const char *term, std::vector<Parser> workers)
			: Parser(start, term, on_open_r_, nullptr, on_close_r_, workers)
		{}
};

