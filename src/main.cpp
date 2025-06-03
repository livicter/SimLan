#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <memory> // For std::unique_ptr
#include <stdexcept> // For std::runtime_error

#include "lexer.hpp"
#include "parser.hpp"
#include "ast.hpp"

// Function to read the entire content of a file into a string
std::string readFile(const std::string& filepath) {
    std::ifstream file_stream(filepath);
    if (!file_stream.is_open()) {
        std::cerr << "Error: Could not open file: " << filepath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file_stream.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: simlanc <filepath>" << std::endl;
        return 1;
    }

    std::string filepath = argv[1];
    std::cout << "Compiling Simlan file: " << filepath << std::endl;

    std::string source_code = readFile(filepath);
    if (source_code.empty()) {
        return 1; // Error message already printed by readFile
    }

    // 1. Lexing
    Lexer lexer(source_code);
    std::cout << "\n--- Tokens ---" << std::endl;

    Token token = lexer.getNextToken();
    while(token.type != TokenType::TOKEN_EOF && token.type != TokenType::TOKEN_ERROR){
        std::cout << "Token: " << token.typeToString() << " ('" << token.lexeme << "') Value: " << token.value
                  << " Line: " << token.line << " Col: " << token.column << std::endl;
        token = lexer.getNextToken();
    }
    if(token.type == TokenType::TOKEN_ERROR){
         std::cerr << "Lexical Error: " << token.lexeme << " at line " << token.line << ", column " << token.column << std::endl;
         return 1;
    }
    std::cout << "Token: " << token.typeToString() << " ('" << token.lexeme << "')" << std::endl; // Print EOF
    
    // Re-initialize lexer for the parser (or manage token stream differently)
    // For this simple setup, creating a new lexer is easiest.
    // A more advanced compiler would have the lexer provide a stream that the parser consumes.
    Lexer parser_lexer(source_code); // Create a new lexer instance for the parser


    // 2. Parsing
    Parser parser(parser_lexer); // Pass the new lexer instance
    std::unique_ptr<ProgramNode> ast_root;

    try {
        ast_root = parser.parseProgram();
        std::cout << "\n--- Abstract Syntax Tree (AST) ---" << std::endl;
        if (ast_root) {
            ast_root->print(0);
        } else {
            // This case should ideally not be reached if parsing is successful
            // or throws an error on failure.
            std::cerr << "AST parsing resulted in a null root. Cannot execute." << std::endl;
            return 1; 
        }

        // Execute/Interpret the AST
        std::cout << "\n--- Simlan Output ---" << std::endl; // New section for results
        // No need to check ast_root again if we returned/threw above for null
        ast_root->execute();

    } catch (const ParseError& e) {
        std::cerr << "Parse Error: " << e.what() << std::endl;
        // Line/column info is already in e.what() from ParseError constructor
        return 1;
    } catch (const std::runtime_error& e) { // Catch execution errors
        std::cerr << "Runtime Execution Error: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "An unexpected error occurred: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "\nSimlan processing finished." << std::endl;


    return 0;
}
