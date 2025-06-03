#pragma once

#include "lexer.hpp"
#include "ast.hpp"
#include <vector>
#include <memory>
#include <stdexcept> // For runtime_error

//------------------------------------------------------------------------------
// Parser Class
//------------------------------------------------------------------------------
class Parser {
public:
    // Constructor: takes a Lexer instance
    Parser(Lexer& lexer);

    // Main parsing method: returns the root of the AST (ProgramNode)
    std::unique_ptr<ProgramNode> parseProgram();

private:
    Lexer& lexer;
    Token currentToken;
    Token previousToken; // Useful for error reporting on currentToken

    // Helper methods for token handling
    void advanceToken(); // Consumes currentToken and gets the next one
    // Checks current token type and consumes it if it matches, otherwise throws error
    void consume(TokenType expectedType, const std::string& errorMessage);
    bool match(TokenType type); // Checks current token type without consuming

    // Parsing methods for different grammar rules
    std::unique_ptr<StatementNode> parseStatement();
    std::unique_ptr<PrintNode> parsePrintStatement();
    
    // Expression parsing (following precedence rules)
    // Lowest precedence: addition and subtraction
    std::unique_ptr<ExprNode> parseExpression(); 
    // Next precedence: multiplication and division
    std::unique_ptr<ExprNode> parseTerm();       
    // Highest precedence: numbers, parenthesized expressions, unary operators (not in v0.1)
    std::unique_ptr<ExprNode> parseFactor();     

    // Error handling
    [[noreturn]] void error(const std::string& message) const;
    // Error at a specific token
    [[noreturn]] void errorAt(const Token& token, const std::string& message) const;
};

// Custom exception for parsing errors
class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& message, int line, int column)
        : std::runtime_error(message + " at line " + std::to_string(line) + ", column " + std::to_string(column)),
          error_line(line), error_column(column) {}
    
    int getLine() const { return error_line; }
    int getColumn() const { return error_column; }

private:
    int error_line;
    int error_column;
};
