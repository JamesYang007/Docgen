#pragma once

#include <string>
#include <vector>
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
 * a stop, a change to the currently handled key, or any storing/stowing of the current
 * JSON being actively written to. The JSONWriter will not make an attempt to regulate
 * spacing when in this state.
 **/
class JSONWriter {
	public:
		JSONWriter()
		{
			stowed_.reserve(10); // this really only needs to be as big as the JSON depth might go from active_ (so usually only 2-3 in actuality)
		}

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
		 * Signifies end of "write session" for cleaning up and executing whatever
		 * is necessary.
		 */
		void cleanup_written();
		
		/*
		 * Sets the JSON currently being handled (written to).
		 */
		void set_key(const char *key);

		/*
		 * Store the current active_ JSON by moving it into destination JSON at the
		 * passed at_key; if this key's value is unassigned, to_arr dictates whether
		 * to push content on as array at key or to simply assign at key. If active_
		 * is empty, nothing will be done.
		 * active_ and all state elements are reset on success.
		 */
		void store_to(nlohmann::json& dest_container, const char *at_key, bool to_arr=false);

		/*
		 * Stow the active_ JSON to be held in stowed_. If anything is currently stowed,
		 * it is first unstowed. Anything subsequently written to active_ will ultimately
		 * be assigned (or appended if push_back) at the passed unstow_key when content
		 * is unstowed, s.t. content is stowed for writing sub-content at this key. This
		 * has the effect of "going into" the JSON element.
		 * active_ and all state elements are reset.
		 */
		void go_into(const char *unstow_key, bool push_back=false);

		/*
		 * If anything is stowed, move top stowed_ back as active_ JSON; anything
		 * in active_ will be appended at the key set by call to stow_for().
		 * unstow elements are reset, and on success, all state elements are reset.
		 */
		void back_out();

		/*
		 * Clear current active_ JSON and reset pertinent state elements.
		 */
		void reset_active();

		/*
		 * Clear current active_ and stowed_ JSONs, and reset all state elements.
		 */
		void reset_state();

		/*
		 * Clear stored_ JSON and reset state.
		 */
		void reset();

		/*** STATE SETTERS ***/
		void start_writing() { cleanup_written(); writing_ = true; } // set fed strings to be written to JSON
		void stop_writing() { writing_ = false; } // set fed strings to be disregarded
		void skip_write() { to_skip_ = true; } // set for the next fed string to be skipped/disregarded
		void pushback_write() { to_pushback_ = true; } // set to begin writing value on back of current key's array; if current key is not array, no effect
		void clear_key() { set_key(""); } // clear the currently handled key
		void next_back_out() { pending_back_out_ = true; } // set to back out at next key change or store/depth action

		/*** STATE GETTERS ***/
		bool key_set() const { return !key_.empty(); } // check if a JSON key is set
		bool writing() const { return writing_ && key_set(); } // check if the JSON is being written to
		bool skipping() const { return to_skip_; } // check if set to skip the next fed string
		bool written() { return key_set() && !val_()->empty(); } // check if anything has been written to the currently handled value
		bool just_written() { return written() && just_written_; } // check if anything has been written since last "write session"
		bool anything_written() const { return !active_.empty(); } // check if anything has been written to active_
		bool at_top() const { return stowed_.empty(); } // checks if anything is currently stowed
		nlohmann::json& stored() { return stored_; } // returns a reference to the internal stored_ JSON
		const nlohmann::json& stored() const { return stored_; } // returns a const reference to the internal stored_ JSON

		/*** HELPERS ***/
		void store_to(const char *at_key, bool pushback=false) { store_to(store_dest_(), at_key, pushback); } // store active_ to current store destination
		void store_to_pushback(const char *at_key) { store_to(at_key, true); } // pushback JSON to store rather than assigning
		void go_into_this() { go_into(nullptr); } // stow the current active_ JSON such that it can be stored to; no key is specified for returning to
		void go_into_pushback(const char *unstow_key) { go_into(unstow_key, true); } // stow for key to be appended to (as array) rather than assigned
		void handle_pending_backout() { if (pending_back_out_) { pending_back_out_ = false; back_out(); } }

	private:
		/*** STATE ***/
		nlohmann::json stored_, // JSON for what is stored throughout the lifetime of this writer
		               active_; // JSON that is currently being populated
		std::string key_; // the key whose value is currently being written	
		bool writing_ = false, // dictates where fed strings will be written to JSON
		     just_written_ = false, // tracks whether or not anything has been written since "write session"
		     to_skip_ = false, // may be set to skip the next fed string
		     to_pushback_ = false, // may be set for writing to back of key's array
		     pending_back_out_ = false; // may be set for backing out at end of current "write session"
		
		/*
		 * Returns a pointer to the active_ JSON's currently handled string value,
		 * which is determined by key_.
		 */
		std::string *val_()
		{
			nlohmann::json& cur = active_[key_];
			std::string *ptr;

			if (cur.is_null()) {
				if (to_pushback_) {
					cur.push_back("");
					ptr = cur.back().get_ptr<std::string *>();
				}
				else {
					cur = "";
					ptr = cur.get_ptr<std::string *>();
				}
			}
			else if (cur.is_array()) {
				if (to_pushback_ || cur.empty()) {
					cur.push_back("");
				}
				ptr = cur.back().get_ptr<std::string *>();
			}
			else {
				ptr = cur.get_ptr<std::string *>();
			}

			to_pushback_ = false;
			return ptr;
		}

		/*
		 * Packs all necessary information/content for JSON data to be stowed.
		 */
		struct StowPack_
		{
			nlohmann::json stowed; // JSON for stowed content
			std::string unstow_key; // the key at which active_ content will be assigned when unstowing
			bool unstow_pushback; // may be set for unstowed content to be pushed back to array rather than assigned

			StowPack_(nlohmann::json&& s, const char *k, bool p)
				: stowed(std::move(s)), unstow_key(k), unstow_pushback(p)
			{}
		};
		std::vector<StowPack_> stowed_; // JSONs for actively written JSON to be temporarily stowed; used to implement go_into() going deeper into the JSON

		/*** HELPERS ***/
		nlohmann::json& store_dest_() { return at_top() ? stored_ : stowed_.back().stowed; } // store destination is top stowed_ json or stored_ if none present
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
				if (!s.empty()) {
					write(s);
				}
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

inline void JSONWriter::cleanup_written()
{
	if (key_set()) {
		// trim any trailing whitespace
		std::string *val = val_();
		while (!val->empty() && std::isspace(val->back())) {
			val->pop_back();
		}
		// erase currently handled key from active_ JSON if value is empty
		if (val->empty()) {
			nlohmann::json& cur = active_[key_];
			if (cur.is_array()) {
				active_.erase(cur.end()-1);
			}
			else {
				active_.erase(key_);
			}
		}
	}
	// cleanup "write session" elements
	just_written_ = false;
	to_skip_ = false;
	to_pushback_ = false;
}

inline void JSONWriter::set_key(const char *key)
{
	cleanup_written();
	handle_pending_backout();
	key_.assign(key);
}

inline void JSONWriter::store_to(nlohmann::json& dest_container, const char *at_key, bool to_arr)
{
	cleanup_written();
	handle_pending_backout();
	if (anything_written()) {
		nlohmann::json& dest = dest_container[at_key];
		if (dest.is_null()) {
			if (to_arr) {
				dest.push_back(std::move(active_));
			}
			else {
				dest = std::move(active_);
			}
		}
		else if (dest.is_array()) {
			dest.push_back(std::move(active_));
		}
		else {
			dest = std::move(active_);
		}
		reset_active();
	}
}

inline void JSONWriter::go_into(const char *unstow_key, bool pushback)
{
	cleanup_written();
	handle_pending_backout();
	stowed_.push_back(StowPack_(std::move(active_), unstow_key ? unstow_key : "", pushback));
	reset_active();
}

inline void JSONWriter::back_out()
{
	if (!at_top()) {
		StowPack_& cur = stowed_.back();
		if (!cur.unstow_key.empty()) {
			store_to(cur.stowed, cur.unstow_key.c_str(), cur.unstow_pushback);
		}
		active_ = std::move(cur.stowed);
		stowed_.pop_back();
	}
}

inline void JSONWriter::reset_active()
{
	active_.clear();
	key_.clear();
	writing_ = false;
	just_written_ = false;
	to_skip_ = false;
	to_pushback_ = false;
}

inline void JSONWriter::reset_state()
{
	stowed_.clear();
	reset_active();
}

inline void JSONWriter::reset()
{
	stored_.clear();
	reset_state();
}

} // core
} // parse
} // core
} // docgen
