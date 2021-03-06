#include "./../../include/Interpreter/Interpreter.h"

Interpreter::Interpreter()
{
    this->globals = new Environment();
    this->environment = this->globals;

    this->locals = new std::unordered_map<Expr::Expr*, int>();
}

void Interpreter::setupNativeFunctions()
{
    this->globals->define(
        new std::string("clock"),
        static_cast<void*>(new Clock())
    );
}

std::string* Interpreter::visitLiteralExpr(Expr::Literal* expr)
{
    return expr->value;
}

std::string* Interpreter::visitGroupingExpr(Expr::Grouping* expr)
{
    return evaluate(expr->expression);
}

std::string* Interpreter::visitUnaryExpr(Expr::Unary* expr)
{
    std::string* right = evaluate(expr->right);

    // This is what that makes a language dynamically typed
    switch (expr->operator_->type) {
        case TokenType::MINUS:
            checkNumberOperand(expr->operator_, right);
            *right = "-" + *right;
            return right;

        case TokenType::BANG:
            
            return right;

        default:
            return nullptr;
            break;
    }
}

std::string* Interpreter::visitLogicalExpr(Expr::Logical* expr)
{
    // Calculated in in-order to support short circuit evaluation

    std::string* left = evaluate(expr->left);

    // Checking if we can short circuit the logical expression
    // Based on the evaluated left value
    if (expr->operator_->type == TokenType::OR) {
        if (*isTruthy(left) != "false") {
            return left;
        } 
    } else {
        if (*isTruthy(left) == "false") {
            return left;
        }
    }

    return evaluate(expr->right);
}

std::string* Interpreter::visitBinaryExpr(Expr::Binary* expr)
{
    std::string* left = evaluate(expr->left);
    std::string* right = evaluate(expr->right);

    switch (expr->operator_->type) {
        case TokenType::MINUS:
            checkNumberOperands(expr->operator_, left, right);
            return new std::string(std::to_string(
                string_to_double(left) - 
                string_to_double(right)
            ));

        case TokenType::SLASH:
            checkNumberOperands(expr->operator_, left, right);
            return new std::string(std::to_string(
                string_to_double(left) / 
                string_to_double(right)
            ));

        case TokenType::STAR:
            checkNumberOperands(expr->operator_, left, right);
            return new std::string(std::to_string(
                string_to_double(left) * 
                string_to_double(right)
            ));

        // Handles string concatenation and double addtion
        // If anyone one of operands is string then,
        // returns their concatenation
        case TokenType::PLUS:
            if (!isDouble(left) || !isDouble(right)) {
                return new std::string(*left + *right);
            }

            return new std::string(std::to_string(
                string_to_double(left) +
                string_to_double(right)
            ));

        case TokenType::GREATER:
            checkNumberOperands(expr->operator_, left, right);
            return new std::string(std::to_string(
                string_to_double(left) >
                string_to_double(right)
            ));

        case TokenType::GREATER_EQUAL:
            checkNumberOperands(expr->operator_, left, right);
            return new std::string(std::to_string(
                string_to_double(left) >=
                string_to_double(right)
            ));

        case TokenType::LESS:
            checkNumberOperands(expr->operator_, left, right);
            return new std::string(std::to_string(
                string_to_double(left) <
                string_to_double(right)
            ));

        case TokenType::LESS_EQUAL:
            checkNumberOperands(expr->operator_, left, right);
            return new std::string(std::to_string(
                string_to_double(left) <=
                string_to_double(right)
            ));

        case TokenType::BANG_EQUAL:
            return new std::string(std::to_string(
                !isEqual(left, right))
            );

        case TokenType::EQUAL_EQUAL:
            return new std::string(std::to_string(
                isEqual(left, right))
            );

        default:
            return nullptr;
    }
}

std::string* Interpreter::visitCallExpr(Expr::Call* expr)
{
    // MAY NEED TO FIX THIS IF FUNCTIONS DOESNT WORK
    void* callee = static_cast<void*>(evaluate(expr->callee));

    // Contains evaluated arguements
    std::vector<std::string*>* arguements = new std::vector<std::string*>();

    for (Expr::Expr* arguement: *(expr->arguments)) {
        arguements->push_back(evaluate(arguement));
    }

    // No need to dynamic cast
    // Since Environment stores the function as void*
    if (LoxCallable* function = static_cast<LoxCallable*>(callee)) {
        // Handling Errors before calling a function
        if (arguements->size() != function->arity()) {
            throw new RuntimeError(
                expr->paren,
                "Exprected " + std::to_string(function->arity()) + " arguements but got " +
                std::to_string(arguements->size()) + "."
            );
        }
        // Calling the Function by its name and evaluated arguements
        return function->call(this, arguements);

    } else {
        throw new RuntimeError(expr->paren, "Can only call functions and classes.");
    }

}

std::string* Interpreter::visitAssignExpr(Expr::Assign* expr)
{
    // Resolving method similar to Variable Expression
    // This will require to update the variable values
    std::string* value = evaluate(expr->value);

    if (locals->find(expr) != locals->end()) {
        environment->assignAt(
            locals->at(expr),
            expr->name,
            static_cast<void*>(value)
        );
    } else {
        environment->assign(
            expr->name, 
            static_cast<void*>(value)
        );
    }
    
    return value;
}

std::string* Interpreter::visitGetExpr(Expr::Get* expr)
{
    void* object = static_cast<void*>(evaluate(expr->object));

    // If expression is not instance type, then error is throw
    if (LoxInstance* instance = static_cast<LoxInstance*>(object)) {
        return static_cast<std::string*>(instance->get(expr->name));
    }

    throw new RuntimeError(expr->name,
        "Only instances have properties."
    );
}

std::string* Interpreter::visitSetExpr(Expr::Set* expr)
{
    void* object = static_cast<void*>(expr->object);

    if (LoxInstance* set = static_cast<LoxInstance*>(object)) {
        // Evaluating the object whole property is being set
        std::string* value = evaluate(expr->value);

        set->set(expr->name, static_cast<void*>(value));

        return value;
    } 

    throw new RuntimeError(expr->name,
        "Only instances have fields."
    );
}

std::string* Interpreter::visitVariableExpr(Expr::Variable* expr)
{
    // return static_cast<std::string*>(environment->get(expr->name));

    return static_cast<std::string*>(lookUpVariable(expr->name, expr));
}

void* Interpreter::visitExpressionStmt(Stmt::Expression* stmt)
{
    evaluate(stmt->expression);
    
    return nullptr;
}

void* Interpreter::visitClassStmt(Stmt::Class* stmt)
{
    // Seperate Define and Assign because of Global Classes
    // Which are not handled by Resolver
    environment->define(stmt->name->lexeme, nullptr);
    LoxClass* klass = new LoxClass(stmt->name->lexeme);

    // The two stage variable binding process allows references to the class 
    // inside its own methods
    environment->assign(stmt->name, klass);

    return nullptr;
}

void* Interpreter::visitPrintStmt(Stmt::Print* stmt)
{
    std::string* value = evaluate(stmt->expression);
    
    std::cout << stringify(value) << std::endl;

    return nullptr;
}

void* Interpreter::visitVarStmt(Stmt::Var* stmt)
{
    std::string* value = nullptr;

    if (stmt->initializer != nullptr) {
        value = evaluate(stmt->initializer);
    }

    environment->define(
        stmt->name->lexeme, 
        static_cast<void*>(value)
    );

    return nullptr;
}

void* Interpreter::visitBlockStmt(Stmt::Block* stmt)
{
    executeBlock(stmt->statements, new Environment(environment));

    return nullptr;
}

void* Interpreter::visitIfStmt(Stmt::If* stmt)
{
    std::string evaluatedCondition = *isTruthy(evaluate(stmt->condition));
    if (
        evaluatedCondition != "false" && 
        evaluatedCondition != "0"
    ) {
        execute(stmt->thenBranch);
    } else if (stmt->elseBranch != nullptr) {
        execute(stmt->elseBranch);
    }

    return nullptr;
}

void* Interpreter::visitFunctionStmt(Stmt::Function* stmt)
{
    // Convertin
    // Compile Time representation of function
    // Runtime representation
    // The below env is active when function is declared 
    // Not when the function is called
    LoxFunction* function = new LoxFunction(stmt, environment);
    environment->define(stmt->name->lexeme, function);

    return nullptr;
}

void* Interpreter::visitWhileStmt(Stmt::While* stmt)
{
    while (true) {
        std::string condition = *isTruthy(evaluate(stmt->condition));
        if (condition == "false" || condition == "0") {
            break;
        }

        execute(stmt->body);
    }

    return nullptr;
}

void* Interpreter::visitReturnStmt(Stmt::Return* stmt)
{
    void* value = nullptr;

    if (stmt->value != nullptr) {
        value = static_cast<void*>(evaluate(stmt->value));
    }
    
    throw new Runtime::Return(value);
}

void Interpreter::interpret(std::vector<Stmt::Stmt*>* statements)
{
    try {
        for (Stmt::Stmt* statement: *statements) {
            execute(statement);
        }
    } catch (RuntimeError* error) {
        Lox::runtimeError(*error);
    }
}

void* Interpreter::lookUpVariable(Token* name, Expr::Expr* expr)
{
    // If variable isnt present in locals
    // It is assumed in globals variables
    // Which throws runtime error if undefined variable accessed
    if (locals->find(expr) != locals->end()) {
        // Found a local variable
        return environment->getAt(locals->at(expr), *name->lexeme);
    } else {
        return globals->get(name);
    }
}


std::string Interpreter::stringify(std::string* object)
{
    if (object == nullptr) {
        return "nil";
    }
    
    return *object;
}

void Interpreter::execute(Stmt::Stmt* stmt)
{
    stmt->accept(this);
}

void Interpreter::executeBlock(std::vector<Stmt::Stmt*>* statments, Environment* environment)
{
    Environment* previous = this->environment;

    try {
        this->environment = environment;

        for (Stmt::Stmt* statement: *statments) {
            execute(statement);
        }
    } catch (RuntimeError* error) {
        Lox::runtimeError(*error);
    }

    this->environment = previous;
}

std::string* Interpreter::evaluate(Expr::Expr* expr)
{
    return expr->accept(this);
}

void Interpreter::resolve(Expr::Expr* expr, int depth)
{
    // Define variable's resolved location
    (*locals)[expr] = depth;
}

std::string* Interpreter::isTruthy(std::string* object)
{
    if (
        object == nullptr || 
        *object == "nil"
    ) {
        return new std::string("false");
    }

    // return "true" or "false" as is
    // Later handled by conversion methods
    return object;
}

double Interpreter::string_to_double(std::string* literal)
{    
    return ::atof(literal->c_str());
}

bool Interpreter::isDouble(std::string* literal)
{
    bool decimal = false;

    unsigned int i = literal->at(0) == '-' ? 1 : 0;
    for (; i < literal->size(); i++) {
        if (literal->at(i) == '.') {
            if (decimal) {
                return false;
            } else {
                decimal = true;
            }
        } else if (
            literal->at(i) < '0' || 
            literal->at(i) > '9'
        ) {
            return false;
        }
    }

    return true;
}

bool Interpreter::isEqual(std::string* a, std::string* b)
{
    if (a == nullptr && b == nullptr) {
        return true;
    }

    if (a == nullptr) {
        return false;
    }

    // May need to handle string equivalent too

    return a->at(0) == b->at(0);
}

void Interpreter::checkNumberOperand(Token* operator_, std::string* operand)
{
    if (isDouble(operand)) {
        return;
    }

    throw new RuntimeError(operator_, "Operand must be a number.");
}


void Interpreter::checkNumberOperands(Token* operator_, std::string* left, std::string* right)
{
    if (isDouble(left) && isDouble(right)) {
        return;
    }

    throw new RuntimeError(operator_, "Operands must be numbers.");
}
