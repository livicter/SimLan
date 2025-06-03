#include "ast.hpp"
#include <iostream>
#include <stdexcept> // Required for std::runtime_error
#include <string>    // Required for std::string in error messages
#include <iomanip>   // For std::fixed and std::setprecision (optional formatting)

//------------------------------------------------------------------------------
// NumberNode
//------------------------------------------------------------------------------
void NumberNode::print(int indentLevel) const {
    printIndent(indentLevel);
    std::cout << "NumberNode: " << value << std::endl;
}

double NumberNode::evaluate() const {
    return value;
}

//------------------------------------------------------------------------------
// BinaryOpNode
//------------------------------------------------------------------------------
void BinaryOpNode::print(int indentLevel) const {
    printIndent(indentLevel);
    std::cout << "BinaryOpNode: '" << op << "'" << std::endl;

    printIndent(indentLevel + 1);
    std::cout << "Left:" << std::endl;
    if (left) {
        left->print(indentLevel + 2);
    } else {
        printIndent(indentLevel + 2);
        std::cout << "<null>" << std::endl;
    }

    printIndent(indentLevel + 1);
    std::cout << "Right:" << std::endl;
    if (right) {
        right->print(indentLevel + 2);
    } else {
        printIndent(indentLevel + 2);
        std::cout << "<null>" << std::endl;
    }
}

double BinaryOpNode::evaluate() const {
    if (!left || !right) {
        throw std::runtime_error("Runtime Error: Null operand in binary operation");
    }
    double leftVal = left->evaluate();
    double rightVal = right->evaluate();

    switch (op) {
        case '+': return leftVal + rightVal;
        case '-': return leftVal - rightVal;
        case '*': return leftVal * rightVal;
        case '/':
            if (rightVal == 0) {
                throw std::runtime_error("Runtime Error: Division by zero");
            }
            return leftVal / rightVal;
        default:
            // Create a string from the char for the error message
            throw std::runtime_error("Runtime Error: Unknown binary operator '" + std::string(1, op) + "'");
    }
}

//------------------------------------------------------------------------------
// PrintNode
//------------------------------------------------------------------------------
void PrintNode::print(int indentLevel) const {
    printIndent(indentLevel);
    std::cout << "PrintNode:" << std::endl;
    if (expression) {
        expression->print(indentLevel + 1);
    } else {
        printIndent(indentLevel + 1);
        std::cout << "<null expression>" << std::endl;
    }
}

void PrintNode::execute() const {
    if (!expression) {
        throw std::runtime_error("Runtime Error: PrintNode has null expression to execute");
    }
    double result = expression->evaluate();
    // std::cout << std::fixed << std::setprecision(6) << result << std::endl;
    std::cout << result << std::endl;
}

//------------------------------------------------------------------------------
// ProgramNode
//------------------------------------------------------------------------------
void ProgramNode::print(int indentLevel) const {
    printIndent(indentLevel);
    std::cout << "ProgramNode:" << std::endl;
    for (const auto& stmt : statements) {
        if (stmt) {
            stmt->print(indentLevel + 1);
        } else {
            printIndent(indentLevel + 1);
            std::cout << "<null statement>" << std::endl;
        }
    }
}

void ProgramNode::execute() const {
    for (const auto& stmt : statements) {
        if (stmt) {
            stmt->execute();
        }
        // else: could log a warning or error if a null statement is encountered
    }
}
