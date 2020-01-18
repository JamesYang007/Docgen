#include <core/lexer/lexer.hpp>

namespace docgen {
namespace core {
namespace lexer {

///////////////////////////////////
// Lexer Implementation
///////////////////////////////////

Lexer::Lexer()
    : trie_({
            {"\n", Symbol::NEWLINE},
            {" ", Symbol::WHITESPACE},
            {"\t", Symbol::WHITESPACE},
            {"\v", Symbol::WHITESPACE},
            {"\r", Symbol::WHITESPACE},
            {"\f", Symbol::WHITESPACE},
            {";", Symbol::SEMICOLON},
            {"#", Symbol::HASHTAG},
            {"*", Symbol::STAR},
            {"{", Symbol::OPEN_BRACE},
            {"}", Symbol::CLOSE_BRACE},
            {"///", Symbol::BEGIN_SLINE_COMMENT},
            {"/*!", Symbol::BEGIN_SBLOCK_COMMENT},
            {"//", Symbol::BEGIN_NLINE_COMMENT},
            {"/*", Symbol::BEGIN_NBLOCK_COMMENT},
            {"*/", Symbol::END_BLOCK_COMMENT},
            {"@sdesc", Symbol::SDESC},
            {"@tparam", Symbol::TPARAM},
            {"@param", Symbol::PARAM},
            {"@return", Symbol::RETURN}
            })
{}

void Lexer::process(char c)
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
        text_.append(buf_);
        text_.push_back(c);
        buf_.clear();
        trie_.reset();
        return;
    }
    
    // otherwise, currently backtracking
    this->backtrack(c); 
}

void Lexer::backtrack(char c)
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

} // namespace lexer
} // namespace core
} // namespace docgen
