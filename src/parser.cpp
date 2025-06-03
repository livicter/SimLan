#include "parser.hpp"
#include <iostream> // For error messages

Parser::Parser(Lexer& lex) :
    lexer(lex),
    // Initialize currentToken. Its value will be almost immediately overwritten by
    // the first call to advanceToken(), but it needs a valid state for the
    // first assignment to previousToken within advanceToken().
    currentToken(TokenType::TOKEN_ERROR, "InitialCurrent", 0.0, 0, 1), // Meaningless initial values
    // Explicitly initialize previousToken as well.
    previousToken(TokenType::TOKEN_ERROR, "InitialPrevious", 0.0, 0, 1) // Meaningless initial values
{
    // Now, get the first actual token.
    // previousToken will hold the "InitialCurrent" state.
    // currentToken will hold the first real token from the lexer.
    advanceToken();
}

// Parser::Parser(Lexer& lex) : lexer(lex), currentToken(TokenType::TOKEN_ERROR, "", 0.0, 0, 0) {
//     // Initialize currentToken with the first token from the lexer
//     advanceToken();
// }

void Parser::advanceToken() {
    previousToken = currentToken;
    currentToken = lexer.getNextToken();
    // Skip error tokens from lexer if needed, or handle them
    // For now, we assume the lexer might return TOKEN_ERROR, which the parser should check
}

void Parser::consume(TokenType expectedType, const std::string& errorMessage) {
    if (currentToken.type == expectedType) {
        advanceToken();
    } else {
        errorAt(currentToken, errorMessage);
    }
}

bool Parser::match(TokenType type) {
    if (currentToken.type == type) {
        // advanceToken(); // Typically, match does not consume. Consume does.
        return true;
    }
    return false;
}

// Error reporting (throws a ParseError)
[[noreturn]] void Parser::error(const std::string& message) const {
    errorAt(previousToken, message); // Error is often related to the token just processed or expected after it
}

[[noreturn]] void Parser::errorAt(const Token& token, const std::string& message) const {
    std::string full_message = "Parse Error: " + message;
    if (token.type == TokenType::TOKEN_EOF) {
        full_message += " at end of file.";
    } else {
        full_message += " near '" + token.lexeme + "'";
    }
    throw ParseError(full_message, token.line, token.column);
}


//------------------------------------------------------------------------------
// Parsing Grammar Rules
//------------------------------------------------------------------------------

std::unique_ptr<ProgramNode> Parser::parseProgram() {
    auto programNode = std::make_unique<ProgramNode>();
    while (currentToken.type != TokenType::TOKEN_EOF) {
        if (currentToken.type == TokenType::TOKEN_ERROR) {
             errorAt(currentToken, "Lexical error: " + currentToken.lexeme);
        }
        programNode->addStatement(parseStatement());
    }
    return programNode;
}

std::unique_ptr<StatementNode> Parser::parseStatement() {
    if (match(TokenType::TOKEN_PRINT)) {
        return parsePrintStatement();
    }
    // Add other statement types here (e.g., assignment, if, while)
    errorAt(currentToken, "Expected a statement (e.g., PRINT).");
}

std::unique_ptr<PrintNode> Parser::parsePrintStatement() {
    consume(TokenType::TOKEN_PRINT, "Expected 'PRINT' keyword.");
    std::unique_ptr<ExprNode> expr = parseExpression();
    consume(TokenType::TOKEN_SEMICOLON, "Expected ';' after PRINT statement's expression.");
    return std::make_unique<PrintNode>(std::move(expr));
}

// Expression parsing with precedence:
// expression -> term ( (PLUS | MINUS) term )*
// term       -> factor ( (STAR | SLASH) factor )*
// factor     -> NUMBER | LPAREN expression RPAREN

std::unique_ptr<ExprNode> Parser::parseExpression() {
    std::unique_ptr<ExprNode> left = parseTerm(); // Parse the left-hand side (a term)

    while (match(TokenType::TOKEN_PLUS) || match(TokenType::TOKEN_MINUS)) {
        Token operatorToken = currentToken; // Save the operator token
        advanceToken(); // Consume the operator
        std::unique_ptr<ExprNode> right = parseTerm(); // Parse the right-hand side (another term)
        left = std::make_unique<BinaryOpNode>(operatorToken.lexeme[0], std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<ExprNode> Parser::parseTerm() {
    std::unique_ptr<ExprNode> left = parseFactor(); // Parse the left-hand side (a factor)

    while (match(TokenType::TOKEN_STAR) || match(TokenType::TOKEN_SLASH)) {
        Token operatorToken = currentToken; // Save the operator token
        advanceToken(); // Consume the operator
        std::unique_ptr<ExprNode> right = parseFactor(); // Parse the right-hand side (another factor)
        left = std::make_unique<BinaryOpNode>(operatorToken.lexeme[0], std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<ExprNode> Parser::parseFactor() {
    if (match(TokenType::TOKEN_NUMBER)) {
        // The number token's value is stored in currentToken.value
        // We need to consume it.
        Token numToken = currentToken;
        advanceToken(); // Consume the number token
        return std::make_unique<NumberNode>(numToken.value);
    } else if (match(TokenType::TOKEN_LPAREN)) {
        advanceToken(); // Consume '('
        std::unique_ptr<ExprNode> expr = parseExpression(); // Parse the inner expression
        consume(TokenType::TOKEN_RPAREN, "Expected ')' after expression in parentheses.");
        return expr;
    }
    // Add unary minus/plus here if needed in the future
    // else if (match(TokenType::TOKEN_MINUS)) {
    //    Token opToken = currentToken;
    //    advanceToken();
    //    std::unique_ptr<ExprNode> operand = parseFactor();
    //    return std::make_unique<UnaryOpNode>(opToken.lexeme[0], std::move(operand));
    // }
    else {
        errorAt(currentToken, "Expected a number or a parenthesized expression.");
    }
}
