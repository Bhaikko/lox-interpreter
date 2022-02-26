#pragma once

#include <iostream>

#include "./../Parser/Expression/ExpressionHeaders.h"
#include "./../Parser/Stmt/StmtHeaders.h"
#include "./../Lox.h"

#include "./RuntimeError.h"
#include "./Environment.h"

class Interpreter: 
    public Expr::Visitor<std::string*>,
    public Stmt::Visitor<void*>
{
    private:
        Environment* environment;

    public:
        Interpreter();

    // Semantics handling for Expression and Statements
    // Expressions Handling
    public:
        virtual std::string* visitLiteralExpr(Expr::Literal* expr) override;
        virtual std::string* visitGroupingExpr(Expr::Grouping* expr) override;
        virtual std::string* visitUnaryExpr(Expr::Unary* expr) override;
        virtual std::string* visitBinaryExpr(Expr::Binary* expr) override;
        virtual std::string* visitVariableExpr(Expr::Variable* expr) override;
        virtual std::string* visitAssignExpr(Expr::Assign* expr) override;
        virtual std::string* visitLogicalExpr(Expr::Logical* expr) override;

    // Statements Handling
    public:
        virtual void* visitPrintStmt(Stmt::Print* stmt) override;
        virtual void* visitExpressionStmt(Stmt::Expression* stmt) override;
        virtual void* visitVarStmt(Stmt::Var* stmt) override;
        virtual void* visitBlockStmt(Stmt::Block* stmt) override;
        virtual void* visitIfStmt(Stmt::If* stmt) override;
        virtual void* visitWhileStmt(Stmt::While* stmt) override;

    private:
        // Evaluation of Every expression is done in post order
        std::string* evaluate(Expr::Expr* expr);
        std::string* isTruthy(std::string* object);
        void execute(Stmt::Stmt* stmt);
        void executeBlock(std::vector<Stmt::Stmt*>* statements, Environment* environment);

    private:
        // Error Handling based on semantics
        void checkNumberOperand(Token* operator_, std::string* operand);
        void checkNumberOperands(Token* operator_, std::string* left, std::string* right);

    private:
        // Conversion methods from string to other datatypes
        // Converting String to double
        double string_to_double(std::string* literal);
        bool isDouble(std::string* literal);
        bool isEqual(std::string* a, std::string* b);

    private:
        // Utilities
        std::string stringify(std::string* object);

    public:
        // Evaluates the expression and displays in proper format
        void interpret(std::vector<Stmt::Stmt*>* statements);

    
};