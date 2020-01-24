#include <core/lex/legacy/lexer_routines.hpp>

namespace docgen {
namespace core {
namespace lex {
namespace legacy {

// It is expected that caller has read the string "//" immediately before calling.
void process_line_comment(std::string& text, file_reader& reader, status_t& status)
{
    static constexpr const auto is_not_newline =
        [](char x) {return x != '\n';};

    int c = reader.read();

    if (c == '/') {
        c = reader.read();
        // valid triple-slash comment
        if (isspace(c)) {
            tokenize_text(text, status);
            status.tokens.emplace_back(symbol_t::BEGIN_LINE_COMMENT);
            reader.back(c); // in case it's a single-char token
        }
        // invalid triple-slash comment
        else {
            // no need to read back since c cannot be a whitespace and we ignore anyway
            ignore_until(reader, is_not_newline);
        }
    }

    // invalid triple-slash comment
    else {
        reader.back(c); // the character just read may be '\n'
        ignore_until(reader, is_not_newline);
    }
}

// It is expected that caller has read the string "/*" immediately before calling.
void process_block_comment(std::string& text, file_reader& reader, status_t& status)
{
    const auto is_not_end_block =
        [&](char x) {return (x != '*') || (reader.peek() != '/');};

    int c = reader.read();

    if (c == '!') {
        c = reader.read();
        // valid block comment: tokenize text then begin block comment symbol
        if (isspace(c)) {
            tokenize_text(text, status);
            status.tokens.emplace_back(symbol_t::BEGIN_BLOCK_COMMENT);
            reader.back(c); // may be special single-char token
        }
        // regular block comment: ignore text until end and stop tokenizing
        else {
            ignore_until(reader, is_not_end_block);
            reader.read(); // read the '/'
        }
    }

    // regular block comment
    else {
        ignore_until(reader, is_not_end_block); // stops after reading '*' in "*/"
        reader.read(); // read the '/' after 
    }
}

// If c is not '/' or '*', then no operation done and returns false.
// If c is '/', and if it's a possible line comment ("//") then same as process_line_comment;
// if it's a possible block comment ("/*") then same as process_block_comment;
// otherwise, text is updated to include all characters read.
//
// If c is '*', and if it is the ending of a block comment ("*/"), text tokenized then END_BLOCK_COMMENT;
// otherwise, text tokenized then STAR.
//
// In any case, returns true if first char has been processed.
bool process_string(int c, std::string& text,
                           file_reader& reader, status_t& status)
{
    // possibly beginning of line or block comment
    if (c == '/') {
        c = reader.read();
        if (c == '/') {
            process_line_comment(text, reader, status);
        }
        else if (c == '*') {
            process_block_comment(text, reader, status);
        }
        else {
            text.push_back('/');
            text.push_back(c);
        }
        return true;
    }

    // possibly ending block comment or a star that can be ignored in the middle of a block comment
    else if (c == '*') {
        c = reader.read();
        if (c == '/') {
            tokenize_text(text, status);
            status.tokens.emplace_back(symbol_t::END_BLOCK_COMMENT);
        }
        else {
            tokenize_text(text, status);
            status.tokens.emplace_back(symbol_t::STAR);
            reader.back(c);
        }
        return true;
    }

    return false;
}

void process(file_reader& reader, status_t& status)
{
    std::string text;
    text.reserve(DEFAULT_STRING_RESERVE_SIZE);
    int c = 0;
    bool processed = false;

    while ((c = reader.read()) != file_reader::termination) {

        // process special single-char
        processed = process_char(c, text, status);
        if (processed) {
            continue;
        }

        // process tag name
        processed = process_tag_name(c, text, reader, status);
        if (processed) {
            continue;
        }

        // process string tokens
        processed = process_string(c, text, reader, status);
        if (processed) {
            continue;
        }

        // otherwise, no special symbol -> push to text
        text.push_back(c);
    }

    // tokenize last text then EOF
    tokenize_text(text, status);
    status.tokens.emplace_back(token_t::symbol_t::END_OF_FILE);
}

} // namespace legacy
} // namespace lex
} // namespace core
} // namespace docgen
