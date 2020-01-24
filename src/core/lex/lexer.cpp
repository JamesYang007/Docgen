#include <core/lex/lexer.hpp>
#include <cassert>

namespace docgen {
namespace core {
namespace lex {

///////////////////////////////////
// Lexer Implementation
///////////////////////////////////

void Lexer::process(char c)
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

void Lexer::backtrack(char c)
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

void Lexer::flush()
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
