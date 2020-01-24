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

} // namespace lex
} // namespace core
} // namespace docgen
