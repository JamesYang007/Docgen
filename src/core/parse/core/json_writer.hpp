#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace docgen {
namespace core {
namespace parse {
namespace core {

class JSONWriter {
	public:
		void write(char c, bool counts=false);
		void write(const std::string& s, bool counts=true);
		void feed(const std::string& s);
		void trim_written();
		void store(const char *parent_key);
		void reset_active();
		void reset();

		void start_writing() { writing_ = true; just_written_ = false; }
		void stop_writing() { writing_ = false; trim_written(); }
		void skip_write() { to_skip_ = true; }
		void set_key(const char *key) { trim_written(); key_.assign(key); just_written_ = false; }
		void clear_key() { set_key(""); }
		bool key_set() const { return !key_.empty(); }
		bool writing() const { return writing_ && key_set(); }
		bool skipping() const { return to_skip_; }
		bool written() { return key_set() && !val_()->empty(); }
		bool just_written() { return written() && just_written_; }
		bool anything_written() const { return anything_written_; }
		nlohmann::json& stored() { return stored_; }

	private:
		nlohmann::json stored_, active_;
		std::string key_;
		bool writing_ = false,
		     just_written_ = false,
		     anything_written_ = false,
		     to_skip_ = false;

		std::string *val_()
		{
			if (active_[key_].is_null()) {
				active_[key_] = "";
			}
			return active_[key_].template get_ptr<std::string *>();
		}
};

inline void JSONWriter::write(char c, bool counts)
{
	if (key_set()) {
		val_()->push_back(c);
		if (counts) {
			just_written_ = true;
			anything_written_ = true;
		}
	}
}

inline void JSONWriter::write(const std::string& s, bool counts)
{
	if (key_set()) {
		val_()->append(s);
		if (counts) {
			just_written_ = true;
			anything_written_ = true;
		}
	}
}

/*
 * Feeds a passed string to be written to the JSON if the current status allows for it;
 * suggested_prepend is the count of whitespaces to be prepended if formatting conditions
 * are met (i.e. if anything has been written since last stop or key change).
 */
inline void JSONWriter::feed(const std::string& s)
{
	if (writing()){
		if (!skipping()) {
			if (just_written()) {
				write(s);
			}
			else if (s.find_first_not_of(' ') != std::string::npos) {
				if (written()) {
					write(' ');
				}
				write(s);
			}
		}
		to_skip_ = false;
	}
}

inline void JSONWriter::trim_written()
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

inline void JSONWriter::store(const char *parent_key)
{
	if (!active_.is_null()) {
		trim_written();
		stored_[parent_key].push_back(std::move(active_));
	}
	reset_active();
}

inline void JSONWriter::reset_active()
{
	active_.clear();
	key_.clear();
	writing_ = false;
	just_written_ = false;
	anything_written_ = false;
	to_skip_ = false;
}

inline void JSONWriter::reset()
{
	stored_.clear();
	reset_active();
}

} // core
} // parse
} // core
} // docgen
