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

/////////////////////////////////////////////////////////////////
// Lexer type setup
/////////////////////////////////////////////////////////////////

namespace details {

template <class FirstType, class SecondType, size_t N, size_t... I>
static constexpr auto make_pair_array_helper(
        const std::pair<FirstType, SecondType> (&list)[N],
        std::index_sequence<I...>
        )
{
    using pair_t = std::pair<FirstType, SecondType>;
    return std::array<pair_t, N>{{list[I]...}};
}

// Variadic number of pairs to array of pairs
template <class FirstType, class SecondType, size_t N>
static constexpr auto make_pair_array(
        const std::pair<FirstType, SecondType> (&list)[N]
        )
{
    return make_pair_array_helper(list, std::make_index_sequence<N>());
}

template <const auto& arr, size_t I>
struct get_array_element
{
    using value_type = std::decay_t<typename std::decay_t<decltype(arr)>::value_type>;
    static constexpr value_type value = arr[I];
};

template <const auto& arr, size_t I>
inline constexpr auto get_array_element_v = get_array_element<arr, I>::value;

template <const auto& pair, size_t I>
struct get_pair_element
{
    using value_type = std::conditional_t<
        I == 0,
        std::decay_t<typename std::decay_t<decltype(pair)>::first_type>,
        std::decay_t<typename std::decay_t<decltype(pair)>::second_type>
    >;
    static constexpr value_type value = std::get<I>(pair);
};

template <const auto& pair, size_t I>
inline constexpr auto get_pair_element_v = get_pair_element<pair, I>::value;

template <const auto& string_symbol_arr, size_t... I>
static constexpr auto make_trie_params_input(std::index_sequence<I...>)
{
    return utils::typelist<
        utils::typelist<
            utils::sv_to_valuelist_t<
                get_pair_element_v<
                    get_array_element_v<string_symbol_arr, I>, 
                    0
                >
            >,
            std::integral_constant<
                Symbol, 
                get_pair_element_v<
                    get_array_element_v<string_symbol_arr, I>, 
                    1 
                >
            >
        >...
    >();
}

template <const auto& string_symbol_arr>
static constexpr auto make_trie_params_input()
{
    return make_trie_params_input<string_symbol_arr>(
        std::make_index_sequence<string_symbol_arr.size()>()
    );
}

struct LexTrieParamsGenerator 
{
private:
    static constexpr auto string_symbol_arr = 
        make_pair_array<std::string_view, Symbol>({
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
        });

public:
    using type = utils::trie_params_t<
        std::decay_t<decltype(make_trie_params_input<string_symbol_arr>())>
    >;
};

} // namespace details

using Lexer = LexerGeneric<
    utils::LexTrie<
        typename details::LexTrieParamsGenerator::type
    >
>;

} // namespace lex
} // namespace core
} // namespace docgen
