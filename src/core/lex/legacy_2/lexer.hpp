#pragma once 
#include <core/lex/legacy_2/trie.hpp>
#include <core/lex/status.hpp>
#include <core/symbol.hpp>
#include <core/token.hpp>

namespace docgen {
namespace core {
namespace lex {

struct Lexer
{
    using symbol_t = Symbol;
    using token_t = Token<symbol_t>;
    using status_t = Status<token_t>;

    Lexer();

    void process(char c);
    void flush();
    std::optional<Lexer::token_t> next_token();

private:

    void tokenize_text();
    bool is_backtracking() const;
    void set_backtracking();
    void reset_backtracking();
    void backtrack(char c);
    void update_state();
    void reset();

    enum class State : bool {
        backtrack,
        non_backtrack
    };
    
    Trie<symbol_t> trie_;
    std::string text_;
    std::string buf_;
    State state_ = State::non_backtrack;
    status_t status_;
};

inline void Lexer::tokenize_text()
{
    if (!text_.empty()) {
        status_.tokens.emplace(symbol_t::TEXT, std::move(text_));
    }
}

inline bool Lexer::is_backtracking() const
{
    return state_ == State::backtrack;
}

inline void Lexer::set_backtracking()
{
    state_ = State::backtrack;
}

inline void Lexer::reset_backtracking()
{
    state_ = State::non_backtrack;
}

inline void Lexer::update_state()
{
    // if current state is accepting
    if (trie_.is_accept()) {
        if (!this->is_backtracking()) {
            this->set_backtracking();
        }
        // ignore contents in buffer up until now
        // this optimization can be done because we look for longest match
        buf_.clear();
    }
}

inline std::optional<Lexer::token_t> Lexer::next_token() 
{
    if (!status_.tokens.empty()) {
        token_t token = std::move(status_.tokens.front());
        status_.tokens.pop();
        return token;
    }
    return {};
}

inline void Lexer::reset()
{
    text_.clear();
    buf_.clear();
    trie_.reset();
    reset_backtracking();
}

///////////////////////////////////
// Lexer Implementation
///////////////////////////////////

inline Lexer::Lexer()
    : trie_({
            {"\n", Symbol::NEWLINE},
            {";", Symbol::SEMICOLON},
            {"*", Symbol::STAR},
            {"{", Symbol::OPEN_BRACE},
            {"}", Symbol::CLOSE_BRACE},
            {"///", Symbol::BEGIN_SLINE_COMMENT},
            {"/*!", Symbol::BEGIN_SBLOCK_COMMENT},
            {"*/", Symbol::END_BLOCK_COMMENT},
            {"@tparam", Symbol::TPARAM},
            {"@param", Symbol::PARAM},
            })
{}

inline void Lexer::process(char c)
{
    this->update_state();

    auto it = trie_.get_children().find(c);

    // if transition exists
    if (it != trie_.get_children().end()) {
        buf_.push_back(c);
        trie_.transition(c);
        return;
    }

    // otherwise, no transition exists
    
    // if not backtracking
    if (!this->is_backtracking()) {
        // if trie at root
        if (trie_.is_reset()) {
            text_.push_back(c);
            return;
        }
        text_.append(buf_);
        buf_.clear();
        trie_.reset();
        return this->process(c);
    }
    
    // otherwise, currently backtracking
    this->backtrack(c); 
}

inline void Lexer::backtrack(char c)
{
    // tokenize text
    this->tokenize_text(); 

    // tokenize symbol
    for (uint32_t i = 0; i < buf_.size(); ++i) {
        trie_.back_transition();
    }
    assert(trie_.is_accept());
    auto opt_symbol = trie_.get_symbol();
    assert(static_cast<bool>(opt_symbol));
    status_.tokens.emplace(*opt_symbol);

    // move and clear buf_ to temp string for reprocessing
    std::string reprocess_str(std::move(buf_));
    reprocess_str.push_back(c);

    // reset 
    this->reset();
    
    // reprocess the rest
    for (char c : reprocess_str) {
        this->process(c);
    }
}

inline void Lexer::flush()
{
    this->update_state();

    if (this->is_backtracking()) {
        return this->backtrack(0);
    }

    // non-backtracking: no parent is an accepting node
    // append buf_ to text_ and tokenize text_
    // reset all other fields
    text_.append(buf_);
    this->tokenize_text();
    this->reset();
}

} // namespace lex
} // namespace core
} // namespace docgen
