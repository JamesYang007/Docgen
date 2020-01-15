#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace docgen {
namespace core {

/*
 * TokenType must have a member function .c_str() which returns a const char * string representation.
 */
template <class TokenType>
class JSONWriter {
	public:
		using token_t = TokenType;

		template <class StringType>
		void write(const StringType& s);
		void write(char c);
		void feed(const token_t& t);
		void trim_written();
		void store(const char *parent_key);
		void reset();

		void start_writing() { writing_ = true; just_written_ = false; }
		void stop_writing() { writing_ = false; }
		void skip_write() { to_skip_ = true; }
		void set_key(std::string key) { trim_written(); key_.assign(std::move(key)); just_written_ = false; }
		void clear_key() { set_key(std::string()); }
		bool key_set() { return !key_.empty(); }
		bool writing() { return writing_ && key_set(); }
		bool skipping() { return to_skip_; }
		bool written() { return key_set() && !val_()->empty(); }
		bool just_written() { return !just_written_; }
		const nlohmann::json& stored() { return stored_; }

	private:
		nlohmann::json stored_, active_;
		std::string key_;
		bool writing_ = false,
		     just_written_ = false,
		     to_skip_ = false;

		std::string *val_()
		{
			if (active_[key_].is_null()) {
				active_[key_] = "";
			}
			return active_[key_].template get_ptr<std::string *>();
		}
};


template <class TokenType>
template <class StringType>
inline void JSONWriter<TokenType>::write(const StringType& s)
{
	if (key_set()) {
		val_()->append(s);
	}
}

template <class TokenType>
inline void JSONWriter<TokenType>::write(char c)
{
	if (key_set()) {
		val_()->push_back(c);
	}
}

template <class TokenType>
inline void JSONWriter<TokenType>::feed(const JSONWriter::token_t& t)
{
	if (writing()){
		if (!skipping()) {
			if (written()) {
				if (just_written()) {
					write(' ');
				}
				else if (t.leading_ws_count) {
					write(std::string(t.leading_ws_count, ' '));
				}
			}
			write(t.c_str());
			just_written_ = true;
		}
		to_skip_ = false;
	}
}

template <class TokenType>
inline void JSONWriter<TokenType>::trim_written()
{
	if (key_set()) {
		while (!val_()->empty() && std::isspace(val_()->back())) {
			val_()->pop_back();
		}
		if (val_()->empty()) {
			active_.erase(key_);
		}
	}
}

template <class TokenType>
inline void JSONWriter<TokenType>::store(const char *parent_key)
{
	if (!active_.is_null()) {
		trim_written();
		stored_[parent_key].push_back(std::move(active_));
		active_.clear();
	}
}

template <class TokenType>
inline void JSONWriter<TokenType>::reset()
{
	stored_.clear();
	active_.clear();
	key_.clear();
	writing_ = false;
	just_written_ = false;
	to_skip_ = false;
}

} // core
} // docgen
