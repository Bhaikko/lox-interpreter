#pragma once

#include <unordered_map>
#include <vector>

#include "./Token.h"
#include "./../Lox.h"

class Scanner {
    private:    
        // Field to track Scanner Position
        int start,      // points to first character of lexeme
            current,    // points to character being considered
            line;

        std::string* source;         // Source Code
        std::vector<Token*>* tokens;

        std::unordered_map<std::string, TokenType>* keywords;

    public:
        Scanner(std::string* source);
        std::vector<Token*>* scanTokens();

    private:
        void fillKeywords();

    private:
        void addToken(TokenType type);
        void addToken(TokenType type, std::string* literal);
        char advance();
        void scanToken();
        bool isAtEnd();
        char peek();
        char peekNext();
        void identifier();
    
    // Methods for checking non one length lexemes
    private:
        bool match(char expected);
        void string();
        bool isDigit(char c);
        bool isAlpha(char c);
        bool isAlphaNumeric(char c);
        void number();
};