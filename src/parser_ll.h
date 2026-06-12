#ifndef PARSER_LL_H
#define PARSER_LL_H

#include "lexer.h"
#include "grammar.h"
#include "error_handler.h"
#include <stack>
#include <string>

class ParserLL {
public:
    ParserLL(Lexer& lexer, const Grammar& grammar, ErrorHandler& errHandler);

    bool parse();

private:
    Lexer& lexer;
    const Grammar& grammar;
    ErrorHandler& errHandler;

    std::stack<std::string> symbolStack;
    Token currentToken;

    void advance();
    void reportSyntaxError(const std::string& msg);
    void recover(const std::string& nonTerminal);
};

#endif
