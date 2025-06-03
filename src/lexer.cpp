#include "lexer.hpp"
#include <cctype>   // For isdigit, isalpha, isspace
#include <iostream> // For error reporting (temporary)
#include <unordered_map>

// Helper to convert TokenType to string
std::string Token::typeToString() const {
    switch (type) {
        case TokenType::TOKEN_PRINT:     return "PRINT";
        case TokenType::TOKEN_NUMBER:    return "NUMBER";
        case TokenType::TOKEN_PLUS:      return "PLUS";
        case TokenType::TOKEN_MINUS:     return "MINUS";
        case TokenType::TOKEN_STAR:      return "STAR";
        case TokenType::TOKEN_SLASH:     return "SLASH";
        case TokenType::TOKEN_LPAREN:    return "LPAREN";
        case TokenType::TOKEN_RPAREN:    return "RPAREN";
        case TokenType::TOKEN_SEMICOLON: return "SEMICOLON";
        case TokenType::TOKEN_EOF:       return "EOF";
        case TokenType::TOKEN_ERROR:     return "ERROR";
        case TokenType::TOKEN_IDENTIFIER:return "IDENTIFIER";
        default:                         return "UNKNOWN";
    }
}


Lexer::Lexer(const std::string& source)
    : source_code(source), current_pos(0), current_line(1), current_column_start_of_line(0) {}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source_code[current_pos];
}

char Lexer::advance() {
    if (isAtEnd()) return '\0';
    char currentChar = source_code[current_pos];
    current_pos++;
    return currentChar;
}

bool Lexer::isAtEnd() const {
    return current_pos >= source_code.length();
}

Token Lexer::makeToken(TokenType type, const std::string& lexeme_val) const {
    // Calculate column based on current_pos and current_column_start_of_line
    // This is a simplified column calculation, assumes lexeme is part of the current advance()
    // For multi-char tokens, the start_pos of the token should be used.
    int col = (current_pos - current_column_start_of_line) - lexeme_val.length() + 1;
     if (!lexeme_val.empty() && type != TokenType::TOKEN_EOF) { // EOF has no lexeme
        col = (current_pos - current_column_start_of_line) - lexeme_val.length() +1;
    } else if (type == TokenType::TOKEN_EOF) {
        col = (current_pos - current_column_start_of_line) + 1;
    } else { // For single char tokens where lexeme is not explicitly passed
        col = (current_pos - current_column_start_of_line);
    }
    return Token(type, lexeme_val, 0.0, current_line, col);
}


Token Lexer::errorToken(const std::string& message) const {
    // Similar column calculation logic as makeToken
    int col = (current_pos - current_column_start_of_line) + 1;
    return Token(TokenType::TOKEN_ERROR, message, 0.0, current_line, col);
}


void Lexer::skipWhitespaceAndComments() {
    while (!isAtEnd()) {
        char c = peek();
        if (std::isspace(c)) {
            if (c == '\n') {
                current_line++;
                current_column_start_of_line = current_pos + 1; // Next char is start of new line
            }
            advance();
        } else if (c == '/' && current_pos + 1 < source_code.length() && source_code[current_pos + 1] == '/') {
            // Single-line comment, skip to end of line
            while (peek() != '\n' && !isAtEnd()) {
                advance();
            }
            if(peek() == '\n'){ // Consume the newline
                advance();
                current_line++;
                current_column_start_of_line = current_pos;
            }
        } else {
            break; // Not whitespace or comment
        }
    }
}

Token Lexer::number() {
    size_t start_pos = current_pos;
    while (std::isdigit(peek())) {
        advance();
    }

    // Look for a fractional part
    if (peek() == '.' && std::isdigit(source_code[current_pos + 1])) {
        advance(); // Consume the '.'
        while (std::isdigit(peek())) {
            advance();
        }
    }

    std::string lexeme = source_code.substr(start_pos, current_pos - start_pos);
    double value = 0.0;
    try {
        value = std::stod(lexeme);
    } catch (const std::out_of_range& oor) {
        return errorToken("Numeric literal out of range: " + lexeme);
    } catch (const std::invalid_argument& ia) {
        // This should not happen if isdigit checks are correct
        return errorToken("Invalid numeric literal: " + lexeme);
    }
    
    int col = (start_pos - current_column_start_of_line) + 1;
    return Token(TokenType::TOKEN_NUMBER, lexeme, value, current_line, col);
}

Token Lexer::identifierOrKeyword() {
    size_t start_pos = current_pos;
    while (std::isalnum(peek()) || peek() == '_') { // Allow underscores in identifiers
        advance();
    }
    std::string lexeme = source_code.substr(start_pos, current_pos - start_pos);
    int col = (start_pos - current_column_start_of_line) + 1;

    static const std::unordered_map<std::string, TokenType> keywords = {
        {"PRINT", TokenType::TOKEN_PRINT}
    };

    auto it = keywords.find(lexeme);
    if (it != keywords.end()) {
        return Token(it->second, lexeme, 0.0, current_line, col);
    }

    // For now, we only have PRINT. If it's not PRINT, it's an error for v0.1
    // In the future, this would be TokenType::TOKEN_IDENTIFIER
    // return Token(TokenType::TOKEN_IDENTIFIER, lexeme, 0.0, current_line, col);
    return errorToken("Unexpected identifier or keyword: " + lexeme);
}


Token Lexer::getNextToken() {
    skipWhitespaceAndComments();

    if (isAtEnd()) return makeToken(TokenType::TOKEN_EOF);

    char c = advance(); // Consume the character

    // For single-character tokens, their lexeme is just the character itself.
    // The column is calculated based on the position *before* advancing for this char.
    int col = (current_pos - 1 - current_column_start_of_line) + 1;


    if (std::isalpha(c) || c == '_') { // Start of an identifier or keyword
        // Put the character back to be consumed by identifierOrKeyword
        current_pos--; 
        return identifierOrKeyword();
    }

    if (std::isdigit(c)) {
        // Put the character back to be consumed by number()
        current_pos--;
        return number();
    }

    switch (c) {
        case '(': return Token(TokenType::TOKEN_LPAREN, "(", 0.0, current_line, col);
        case ')': return Token(TokenType::TOKEN_RPAREN, ")", 0.0, current_line, col);
        case ';': return Token(TokenType::TOKEN_SEMICOLON, ";", 0.0, current_line, col);
        case '+': return Token(TokenType::TOKEN_PLUS, "+", 0.0, current_line, col);
        case '-': return Token(TokenType::TOKEN_MINUS, "-", 0.0, current_line, col);
        case '*': return Token(TokenType::TOKEN_STAR, "*", 0.0, current_line, col);
        case '/': return Token(TokenType::TOKEN_SLASH, "/", 0.0, current_line, col);
        // Add other single-character tokens here
        default:
            return errorToken(std::string("Unexpected character: ") + c);
    }
}
