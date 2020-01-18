#pragma once 
#include <core/trie.hpp>
#include <core/symbol.hpp>
#include <core/status.hpp>
#include <core/token.hpp>

namespace docgen {
namespace core {

struct Lexer
{
    using symbol_t = Symbol;
    using token_t = Token<symbol_t>;
    using status_t = Status<token_t>;

    Lexer();

    void process(char c);
    std::optional<Lexer::token_t> next_token();

private:

    bool is_backtracking() const;
    void set_backtracking();
    void reset_backtracking();
    void backtrack(char c);

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

///////////////////////////////////
// Lexer Implementation
///////////////////////////////////

Lexer::Lexer()
    : trie_({
            {"\n", Symbol::NEWLINE},
            {";", Symbol::SEMICOLON},
            {" ", Symbol::WHITESPACE},
            {"\t", Symbol::WHITESPACE},
            {"\v", Symbol::WHITESPACE},
            {"\r", Symbol::WHITESPACE},
            {"\f", Symbol::WHITESPACE},
            {"*", Symbol::STAR},
            {"{", Symbol::OPEN_BRACE},
            {"}", Symbol::CLOSE_BRACE},
            {"///", Symbol::BEGIN_SLINE_COMMENT},
            {"/*!", Symbol::BEGIN_SBLOCK_COMMENT},
            {"//", Symbol::BEGIN_NLINE_COMMENT},
            {"/*", Symbol::BEGIN_NBLOCK_COMMENT},
            {"*/", Symbol::END_BLOCK_COMMENT},
            {"@sdesc", Symbol::SDESC}
            })
{
    // TODO: reserve space for status_.tokens?
}

inline void Lexer::process(char c)
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
        text_.append(buf_);
        text_.push_back(c);
        buf_.clear();
        trie_.reset();
        return;
    }
    
    // otherwise, currently backtracking
    this->backtrack(c); 
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

inline void Lexer::backtrack(char c)
{
    // reset to non-backtracking
    this->reset_backtracking();

    // tokenize and clear text
    if (!text_.empty()) {
        status_.tokens.emplace(symbol_t::TEXT, std::move(text_));
        text_.clear();
    }
    
    // tokenize symbol
    for (uint32_t i = 0; i < buf_.size(); ++i) {
        trie_.back_transition();
    }
    assert(trie_.is_accept());
    auto opt_symbol = trie_.get_symbol();
    assert(static_cast<bool>(opt_symbol));
    status_.tokens.emplace(*opt_symbol);

    // move and clear buf_ to temp
    std::string reprocess_str(std::move(buf_));
    buf_.clear();
    reprocess_str.push_back(c);

    // reset trie
    trie_.reset();
    
    // reprocess the rest
    for (char c : reprocess_str) {
        this->process(c);
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

} // namespace core
} // namespace docgen
