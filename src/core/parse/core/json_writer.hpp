#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace docgen {
namespace core {
namespace parse {
namespace core {

/**
 * A JSONWriter object holds a stored_ JSON to be populated throughout its lifetime.
 * The job of this writer is to carefully regulate what is written to this JSON and how
 * it is formatted, as strings are fed in and state is manipulated by public helpers.
 *
 * A "continuous write session" is said to be when writes have been uninterrupted by
 * a stop or a change to the currently handled key. The JSONWriter will not make any
 * attempt to regulate spacing when in this state.
 **/
class JSONWriter {
	public:
		/*
		 * Writes a string/character to the JSON value currently being handled.
		 * Boolean may be passed to indicate if this is a substantive write
		 * to be registered accordingly with state elements
		 * (usually only the case if non-whitespace).
		 */
		template <class StringType>
		void write(const StringType& s, bool counts=true);
		void write(char c, bool counts=false);

		/*
		 * Feeds a passed string to be written to the JSON if the current state allows for it;
		 * if string content is (or begins with) whitespace, if/how it is written will depend
		 * on other state factors, such as if anything has been written to the presently handled
		 * value and if the writer is currently in a "continuous write session".
		 */
		void feed(const std::string& s);

		/*
		 * Trims the value currently being written of any trailing whitespace,
		 * and erases the key if value is ultimately empty.
		 */
		void trim_written();

		/*
		 * Store the current active_ JSON by moving it into destination JSON array at the
		 * passed parent_key; if active_ is empty or value at parent_key is set to anything
		 * other than an array, nothing will be done.
		 * active_ and all state elements are reset on success.
		 */
		void store(const char *parent_key, nlohmann::json& dest);

		/*
		 * Stow the active_ JSON to be held in stowed_. If anything is currently stowed,
		 * it is first unstowed. Anything subsequently written to active_ will ultimately
		 * be appended at the passed unstow_key when content is unstowed, s.t. content is
		 * stowed for writing sub-content at this key (hence the name "stow_for()".
		 * active_ and all state elements are reset.
		 */
		void stow_for(const char *unstow_key);

		/*
		 * If anything is stowed, move stowed_ back as active_ JSON; anything in active_ will
		 * be appended at the key set by call to stow_for().
		 * unstow_key_ is reset, and on success, all state elements are reset.
		 */
		void unstow();

		/*
		 * Clear current active_ JSON and reset all state elements.
		 */
		void reset_active();

		/*
		 * Clear stored_ JSON and reset_active().
		 */
		void reset();

		/*** STATE SETTERS ***/
		void start_writing() { writing_ = true; just_written_ = false; } // set fed strings to be written to JSON
		void stop_writing() { writing_ = false; trim_written(); } // set fed strings to be disregarded
		void skip_write() { to_skip_ = true; } // set for the next fed string to be skipped/disregarded
		void set_key_active(const char *key) { trim_written(); key_.assign(key); just_written_ = false; } // set the JSON key to be currently handled
		void set_key(const char *key) { unstow(); set_key_active(key); } // set the JSON key for main JSON (by first unstowing in case stowed)
		void clear_key() { set_key(""); } // clear the currently handled key for main JSON
		void store(const char *parent_key) { unstow(); store(parent_key, stored_); } // store main JSON to stored_ (by first unstowing in case stowed)

		/*** STATE GETTERS ***/
		bool key_set() const { return !key_.empty(); } // check if a JSON key is set
		bool writing() const { return writing_ && key_set(); } // check if the JSON is being written to
		bool skipping() const { return to_skip_; } // check if set to skip the next fed string
		bool written() { return key_set() && !val_()->empty(); } // check if anything has been written to the currently handled value
		bool just_written() { return written() && just_written_; } // check if anything has been written since last "write session"
		bool anything_written() const { return !active_.empty(); } // check if anything has been written to active_
		bool anything_stowed() const { return !stowed_.empty(); }
		nlohmann::json& stored() { return stored_; } // returns a reference to the internal stored_ JSON

	private:
		/*** STATE ***/
		nlohmann::json stored_, // JSON for what is stored throughout the lifetime of this writer
		               stowed_, // JSON for actively written JSON to be temporarily stowed when necessary
		               active_; // JSON that is currently being populated
		std::string key_, // the key whose value is currently being written	
		            unstow_key_; // the key at which active_ content will be appended when unstowing
		bool writing_ = false, // dictates where fed strings will be written to JSON
		     just_written_ = false, // tracks whether or not anything has been written since "write session"
		     to_skip_ = false; // may be set to skip the next fed string

		/*
		 * Returns a pointer to the active_ JSON's currently handled string value,
		 * which is determined by key_
		 */
		std::string *val_()
		{
			if (active_[key_].is_null()) {
				active_[key_] = ""; // this is necessary for the JSON to register the value as a string before appending
			}
			return active_[key_].template get_ptr<std::string *>();
		}
};

template <class StringType>
inline void JSONWriter::write(const StringType& s, bool counts)
{
	if (key_set()) {
		val_()->append(s);
		if (counts) { // set write-tracking state
			just_written_ = true;
		}
	}
}

inline void JSONWriter::write(char c, bool counts)
{
	if (key_set()) {
		val_()->push_back(c);
		if (counts) { // set write-tracking state
			just_written_ = true;
		}
	}
}

inline void JSONWriter::feed(const std::string& s)
{
	if (writing()){
		if (!skipping()) {
			size_t start_pos;
			if (just_written()) { // if in a "continuous write session", write regardless
				write(s);
			}
			else if ((start_pos = s.find_first_not_of(' ')) != std::string::npos) { // otherwise write from non-whitespace
				if (written()) { // if something has already been written to the current value, separate with a space
					write(' ');
				}
				write(s.c_str() + start_pos);
			}
		}
		to_skip_ = false;
	}
}

inline void JSONWriter::trim_written()
{
	if (key_set()) {
		// trim any trailing whitespace
		while (!val_()->empty() && std::isspace(val_()->back())) {
			val_()->pop_back();
		}
		// erase currently handled key from active_ JSON if value is empty
		if (val_()->empty()) {
			active_.erase(key_);
		}
	}
}

inline void JSONWriter::store(const char *parent_key, nlohmann::json& dest)
{
	nlohmann::json& dest_arr = dest[parent_key];
	if (anything_written() && (dest_arr.is_null() || dest_arr.is_array())) {
		trim_written();
		dest_arr.push_back(std::move(active_));
		reset_active();
	}
}

inline void JSONWriter::stow_for(const char *unstow_key)
{
	if (anything_stowed()) {
		unstow();
	}
	unstow_key_.assign(unstow_key);
	stowed_ = std::move(active_);
	reset_active();
}

inline void JSONWriter::unstow()
{
	if (anything_stowed()) {
		store(unstow_key_.c_str(), stowed_);
		active_ = std::move(stowed_);
	}
	unstow_key_.clear();
}

inline void JSONWriter::reset_active()
{
	active_.clear();
	key_.clear();
	writing_ = false;
	just_written_ = false;
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
