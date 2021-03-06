#include "./../../include/Scanner/Token.h"

Token::Token(TokenType type, std::string* lexeme, std::string* literal, int line) 
{
    this->type = type;
    this->lexeme = lexeme;
    this->literal = literal;
    this->line = line;
}

std::ostream& operator<<(std::ostream& os, const Token& t) {
    std::string lexemeStr = t.lexeme ? *(t.lexeme) : "";
    std::string literalStr = t.literal ? *(t.literal) : "";

    os << std::to_string(t.type) + " " + lexemeStr + " " + literalStr;
    return os;
}

