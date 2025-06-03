#pragma once

#include <string>
#include <vector>

//------------------------------------------------------------------------------
// Token Types
//------------------------------------------------------------------------------
enum class TokenType {
    // Keywords
    TOKEN_PRINT,        // "PRINT"

    // Literals
    TOKEN_NUMBER,       // 123, 42.0

    // Operators
    TOKEN_PLUS,         // +
    TOKEN_MINUS,        // -
    TOKEN_STAR,         // *
    TOKEN_SLASH,        // /
    TOKEN_LPAREN,       // (
    TOKEN_RPAREN,       // )

    // Punctuation
    TOKEN_SEMICOLON,    // ;

    // Special Tokens
    TOKEN_EOF,          // End of File
    TOKEN_ERROR,        // Lexical error / unrecognized token
    TOKEN_IDENTIFIER    // For future use (variables, functions) - not used in v0.1
};

//------------------------------------------------------------------------------
// Token Structure
//------------------------------------------------------------------------------
struct Token {
    TokenType type;
    std::string lexeme; // The actual string sequence for the token (e.g. "123")
    double value;       // Numeric value if TOKEN_NUMBER (e.g. 123)
    int line;           // Line number where the token starts
    int column;         // Column number where the token starts

    Token(TokenType t, std::string lex = "", double val = 0.0, int l = 0, int col = 0)
        : type(t), lexeme(std::move(lex)), value(val), line(l), column(col) {}

    // Helper to convert token type to string for debugging
    std::string typeToString() const;
};

//------------------------------------------------------------------------------
// Lexer Class
//------------------------------------------------------------------------------
class Lexer {
public:
    // Constructor: takes the source code as a string
    Lexer(const std::string& source);

    // Returns the next token from the source code
    Token getNextToken();

private:
    std::string source_code;
    size_t current_pos; // Current position in the source_code string
    int current_line;
    int current_column_start_of_line; // Position of the start of the current line, for column calculation

    // Helper methods
    char peek() const;        // Look at the current character without consuming
    char advance();           // Consume the current character and advance
    bool isAtEnd() const;
    void skipWhitespaceAndComments(); // Skips spaces, tabs, newlines, and comments

    Token makeToken(TokenType type, const std::string& lexeme = "") const;
    Token errorToken(const std::string& message) const;
    Token number();
    Token identifierOrKeyword(); // For PRINT and future identifiers
};
