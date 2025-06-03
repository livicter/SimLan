#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream> // For printing AST
#include <stdexcept> // For std::runtime_error in evaluate/execute

// Forward declarations
struct NumberNode;
struct BinaryOpNode;
struct PrintNode;
struct ProgramNode;

//------------------------------------------------------------------------------
// Base class for all expression nodes
//------------------------------------------------------------------------------
struct ExprNode {
    virtual ~ExprNode() = default;
    virtual void print(int indentLevel = 0) const = 0;
    virtual double evaluate() const = 0; // To calculate the value of the expression
};

//------------------------------------------------------------------------------
// Represents a numeric literal
//------------------------------------------------------------------------------
struct NumberNode : ExprNode {
    double value;

    explicit NumberNode(double val) : value(val) {}

    void print(int indentLevel = 0) const override;
    double evaluate() const override;
};

//------------------------------------------------------------------------------
// Represents a binary operation
//------------------------------------------------------------------------------
struct BinaryOpNode : ExprNode {
    char op;
    std::unique_ptr<ExprNode> left;
    std::unique_ptr<ExprNode> right;

    BinaryOpNode(char operation, std::unique_ptr<ExprNode> lhs, std::unique_ptr<ExprNode> rhs)
        : op(operation), left(std::move(lhs)), right(std::move(rhs)) {}

    void print(int indentLevel = 0) const override;
    double evaluate() const override;
};

//------------------------------------------------------------------------------
// Base class for all statement nodes
//------------------------------------------------------------------------------
struct StatementNode {
    virtual ~StatementNode() = default;
    virtual void print(int indentLevel = 0) const = 0;
    virtual void execute() const = 0; // To execute the statement
};

//------------------------------------------------------------------------------
// Represents a PRINT statement
//------------------------------------------------------------------------------
struct PrintNode : StatementNode {
    std::unique_ptr<ExprNode> expression;

    explicit PrintNode(std::unique_ptr<ExprNode> expr) : expression(std::move(expr)) {}

    void print(int indentLevel = 0) const override;
    void execute() const override;
};

//------------------------------------------------------------------------------
// Represents the entire program
//------------------------------------------------------------------------------
struct ProgramNode {
    std::vector<std::unique_ptr<StatementNode>> statements;

    void addStatement(std::unique_ptr<StatementNode> stmt) {
        statements.emplace_back(std::move(stmt));
    }

    void print(int indentLevel = 0) const;
    void execute() const; // To execute all statements in the program
};

// Helper function for indentation in print methods
inline void printIndent(int level) {
    for (int i = 0; i < level; ++i) {
        std::cout << "  ";
    }
}
