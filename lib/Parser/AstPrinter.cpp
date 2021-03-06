#include "./../../include/Parser/AstPrinter.h"

std::string* AstPrinter::print(Expr::Expr* expr)
{
    return expr->accept(this);
}

std::string* AstPrinter::parenthesize(std::string* name, std::vector<Expr::Expr*> exprs) 
{
    std::string builder = "(";
    builder += *name;
    for (Expr::Expr* expr: exprs) {
        builder += " ";
        builder += *(expr->accept(this));
    }

    builder += ")";

    return new std::string(builder);
}

std::string* AstPrinter::visitBinaryExpr(Expr::Binary* expr) 
{
    std::vector<Expr::Expr*> exprs;
    exprs.push_back(expr->left);
    exprs.push_back(expr->right);

    return parenthesize(
        expr->operator_->lexeme, exprs
    );
}

std::string* AstPrinter::visitGroupingExpr(Expr::Grouping* expr) 
{
    std::vector<Expr::Expr*> exprs;
    exprs.push_back(expr->expression);

    return parenthesize(new std::string("group"), exprs);
}

std::string* AstPrinter::visitLiteralExpr(Expr::Literal* expr) 
{
    if (expr->value->size() == 0) {
        return new std::string("nil");
    }
    return expr->value;

}

std::string* AstPrinter::visitUnaryExpr(Expr::Unary* expr) 
{
    std::vector<Expr::Expr*> exprs;
    exprs.push_back(expr->right);

    return parenthesize(expr->operator_->lexeme, exprs);
}
