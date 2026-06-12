#ifndef PARSER_LR_H
#define PARSER_LR_H

#include "lexer.h"
#include "grammar.h"
#include "error_handler.h"
#include <vector>
#include <string>
#include <iostream>

class ParserLR {
public:
    ParserLR(Lexer& lexer, const Grammar& grammar, ErrorHandler& errHandler);

    bool parse(std::ostream& traceOs);

private:
    Lexer& lexer;
    const Grammar& grammar;
    ErrorHandler& errHandler;

    std::vector<int> stateStack;
    std::vector<std::string> symbolStack;
    Token currentToken;

    void advance();
    void reportSyntaxError(const std::string& msg);
    bool recover();

    void printTraceStep(std::ostream& os, const std::string& actionStr) const;
};

#endif
