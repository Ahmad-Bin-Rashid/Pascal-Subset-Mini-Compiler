#include "parser_ll.h"
#include <iostream>

ParserLL::ParserLL(Lexer& lexer, const Grammar& grammar, ErrorHandler& errHandler)
    : lexer(lexer), grammar(grammar), errHandler(errHandler) {}

void ParserLL::advance() {
    currentToken = lexer.getNextToken();
}

void ParserLL::reportSyntaxError(const std::string& msg) {
    errHandler.report("Syntactic", currentToken.line, currentToken.col, msg + " (Found: '" + currentToken.lexeme + "')");
}

bool ParserLL::parse() {
    lexer.reset();
    advance();

    while (!symbolStack.empty()) {
        symbolStack.pop();
    }
    symbolStack.push("$");
    symbolStack.push("Program");

    bool success = true;

    while (!symbolStack.empty()) {
        std::string X = symbolStack.top();
        std::string a = grammar.mapTokenToTerminal(currentToken);

        if (X == "$") {
            if (a == "$") {
                symbolStack.pop();
                break;
            } else {
                reportSyntaxError("Extra tokens at the end of the file");
                success = false;
                break;
            }
        }

        if (grammar.isTerminal(X)) {
            if (X == a) {
                symbolStack.pop();
                advance();
            } else {
                reportSyntaxError("Mismatched terminal. Expected: '" + X + "'");
                success = false;
                symbolStack.pop(); // Assume matched and proceed (recovery)
            }
        } else {
            int prodIdx = grammar.getLLAction(X, a);
            if (prodIdx != -1) {
                symbolStack.pop();
                const auto& prod = grammar.getProductions()[prodIdx];
                if (!prod.rhs.empty()) {
                    for (auto rit = prod.rhs.rbegin(); rit != prod.rhs.rend(); ++rit) {
                        symbolStack.push(*rit);
                    }
                }
            } else {
                reportSyntaxError("No transition in LL(1) table for non-terminal '" + X + "' on lookahead '" + a + "'");
                success = false;
                recover(X);
            }
        }
    }

    return success && !errHandler.hasErrors();
}

void ParserLL::recover(const std::string& nonTerminal) {
    const auto& followSets = grammar.getFollowSets();
    auto it = followSets.find(nonTerminal);
    if (it == followSets.end()) {
        symbolStack.pop();
        return;
    }
    const auto& follow = it->second;

    while (true) {
        std::string a = grammar.mapTokenToTerminal(currentToken);
        if (a == "$") {
            break;
        }
        if (follow.find(a) != follow.end()) {
            break;
        }
        advance();
    }
    symbolStack.pop();
}
