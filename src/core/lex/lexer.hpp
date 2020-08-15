#pragma once 
#include <string>
#include <string_view>
#include <cassert>
#include <core/lex/lextrie.hpp>
#include <core/lex/status.hpp>
#include <core/symbol.hpp>
#include <core/token.hpp>

namespace docgen {
namespace core {
namespace lex {

template <class LexTrieType>
struct LexerGeneric
{
    using lexer_trie_t = LexTrieType;
    using symbol_t = typename lexer_trie_t::symbol_t;
    using token_t = Token<symbol_t>;
    using status_t = Status<token_t>;

    void process(char c);
    void flush();
    std::optional<token_t> next_token();

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
    
    lexer_trie_t trie_;
    std::string text_;
    std::string buf_;
    State state_ = State::non_backtrack;
    status_t status_;
};

template <class LexTrieType>
inline void LexerGeneric<LexTrieType>::tokenize_text()
{
    if (!text_.empty()) {
        status_.tokens.emplace(symbol_t::TEXT, std::move(text_));
    }
}

template <class LexTrieType>
inline bool LexerGeneric<LexTrieType>::is_backtracking() const
{
    return state_ == State::backtrack;
}

template <class LexTrieType>
inline void LexerGeneric<LexTrieType>::set_backtracking()
{
    state_ = State::backtrack;
}

template <class LexTrieType>
inline void LexerGeneric<LexTrieType>::reset_backtracking()
{
    state_ = State::non_backtrack;
}

template <class LexTrieType>
inline void LexerGeneric<LexTrieType>::update_state()
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

template <class LexTrieType>
inline std::optional<typename LexerGeneric<LexTrieType>::token_t> 
LexerGeneric<LexTrieType>::next_token() 
{
    if (!status_.tokens.empty()) {
        token_t token = std::move(status_.tokens.front());
        status_.tokens.pop();
        return token;
    }
    return {};
}

template <class LexTrieType>
inline void LexerGeneric<LexTrieType>::reset()
{
    text_.clear();
    buf_.clear();
    trie_.reset();
    reset_backtracking();
}

template <class LexTrieType>
inline void LexerGeneric<LexTrieType>::process(char c)
{
    this->update_state();

    bool transitioned = trie_.transition(c, 
            [this, c]() {
                this->buf_.push_back(c);
            }
        );

    // if transition exists
    if (transitioned) {
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

template <class LexTrieType>
inline void LexerGeneric<LexTrieType>::backtrack(char c)
{
    // tokenize text
    this->tokenize_text(); 

    // tokenize symbol
    trie_.back_transition(buf_.size());
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

template <class LexTrieType>
inline void LexerGeneric<LexTrieType>::flush()
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

using Lexer = LexerGeneric<lextrie_t>;

} // namespace lex
} // namespace core
} // namespace docgen
