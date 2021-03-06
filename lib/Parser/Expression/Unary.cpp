#include "./../../../include/Parser/Expression/Unary.h"      
                                                        
Expr::Unary::Unary(Token* operator_, Expr* right)                
{                                                      
    this->operator_ = operator_;
	this->right = right;              
};                      

std::string* Expr::Unary::accept(Visitor<std::string*>* visitor)
{
    return visitor->visitUnaryExpr(this);    
}
