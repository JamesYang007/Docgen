#include <stdio.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <type_traits>
#include <vector>
#include <boost/regex.hpp>
#include "parser.hpp"
#include "file_parser.hpp"
#include "comments_parser.hpp"
#include "tags_parser.hpp"
#include "ignore_parser.hpp"

void Parser::proc(char c)
{
	partial_.push_back(c);

	if (betwixt_()) {
		handle_children_(c);
		if (while_betwixt_) {
			while_betwixt_(this);
		}
	}

	if (!boost::regex_match(partial_.data(), matcher_, *cur_, boost::match_default | boost::match_partial)) {
		partial_.clear();
		return;
	}
	if (matcher_[0].matched) { // on expression match
		if (!betwixt_()) { // on opener
			cur_ = &closer_;
			if (on_open_) {
				on_open_(this);
			}

			// handle for when just opened
			for (char o : partial_) {
				handle_children_(o);	
			}
		}
		else { // on closer
			cur_ = &opener_;
			if (on_close_) {
				on_close_(this);
			}

			for (Parser& p : children_) {
				p.reset();
			}
		}
		partial_.clear();
	}
	return;
}

void Parser::handle_children_(char c)
{
	if (child_betwixt_) {
		child_betwixt_->proc(c);
		if (!child_betwixt_->betwixt_()) {
			child_betwixt_ = nullptr;
			for (Parser& p : children_) {
				p.reset();
			}
		}
	}
	if (!child_betwixt_) {
		for (Parser& p : children_) {
			p.proc(c);
			if (p.betwixt_()) {
				child_betwixt_ = &p;
				break;
			}
		}
	}
}

void Parser::reset()
{
	cur_ = &opener_;	
	partial_.clear();
}

int main()
{

	FileParser(stdin, {
		CommentParser("/\\*", "\\*/", { TagParser('[', ']'), IgnoreParser("\\n", "[^[:space:]\\*]") }),
		CommentParser("//", "\\n", { TagParser('[', ']') })
	}).go();

	Parser::dump();
	return 0;
}
