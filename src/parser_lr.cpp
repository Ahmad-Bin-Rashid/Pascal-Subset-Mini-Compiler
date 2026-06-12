#include "parser_lr.h"
#include <iomanip>
#include <sstream>

ParserLR::ParserLR(Lexer& lexer, const Grammar& grammar, ErrorHandler& errHandler)
    : lexer(lexer), grammar(grammar), errHandler(errHandler) {}

void ParserLR::advance() {
    currentToken = lexer.getNextToken();
}

void ParserLR::reportSyntaxError(const std::string& msg) {
    errHandler.report("Syntactic", currentToken.line, currentToken.col, msg + " (Found: '" + currentToken.lexeme + "')");
}

bool ParserLR::parse(std::ostream& traceOs) {
    lexer.reset();
    advance();

    stateStack.clear();
    symbolStack.clear();
    stateStack.push_back(0);
    symbolStack.push_back("$");

    traceOs << "========================================= LR PARSING TRACE =========================================\n";
    traceOs << std::left << std::setw(32) << "Symbol Stack"
            << " | " << std::setw(22) << "State Stack"
            << " | " << std::setw(15) << "Lookahead"
            << " | Action\n";
    traceOs << "----------------------------------------------------------------------------------------------------\n";

    bool success = true;

    while (true) {
        int s = stateStack.back();
        std::string a = grammar.mapTokenToTerminal(currentToken);
        LRAction action = grammar.getLRAction(s, a);

        if (action.type == LRActionType::SHIFT) {
            std::stringstream ss;
            ss << "Shift to State " << action.target;
            printTraceStep(traceOs, ss.str());

            symbolStack.push_back(a);
            stateStack.push_back(action.target);
            advance();
        } else if (action.type == LRActionType::REDUCE) {
            const auto& prod = grammar.getProductions()[action.target];
            std::stringstream ss;
            ss << "Reduce by Rule " << action.target << " (" << prod.lhs << " -> ";
            if (prod.rhs.empty()) ss << "epsilon";
            else {
                for (const auto& sym : prod.rhs) ss << sym << " ";
            }
            ss << ")";
            printTraceStep(traceOs, ss.str());

            // Pop RHS size elements
            int popCount = (int)prod.rhs.size();
            // Epsilon productions have size 0, pop nothing
            if (!prod.rhs.empty() && prod.rhs[0] == "epsilon") {
                popCount = 0;
            }

            for (int i = 0; i < popCount; ++i) {
                if (!symbolStack.empty()) symbolStack.pop_back();
                if (!stateStack.empty()) stateStack.pop_back();
            }

            int sPrime = stateStack.back();
            int nextState = grammar.getLRGoto(sPrime, prod.lhs);
            if (nextState == -1 && prod.lhs == "Program") {
                // Special case for augmented start GOTO check
                nextState = grammar.getLRGoto(sPrime, "Program");
            }

            if (nextState == -1) {
                reportSyntaxError("Goto error during reduction to " + prod.lhs);
                success = false;
                break;
            }

            symbolStack.push_back(prod.lhs);
            stateStack.push_back(nextState);
        } else if (action.type == LRActionType::ACCEPT) {
            printTraceStep(traceOs, "ACCEPT");
            break;
        } else {
            // Error
            reportSyntaxError("Syntax error. No transition in SLR(1) table");
            success = false;
            
            printTraceStep(traceOs, "ERROR (Invoking recovery...)");
            if (!recover()) {
                traceOs << "Recovery failed. Aborting parser.\n";
                break;
            }
            traceOs << "Recovered. Resuming parse steps...\n";
        }
    }

    traceOs << "====================================================================================================\n\n";
    return success && !errHandler.hasErrors();
}

bool ParserLR::recover() {
    std::vector<std::string> syncNTs = {"Stmt", "DeclPart", "VarList", "ConstList", "ProcDecl"};

    for (int i = (int)stateStack.size() - 1; i >= 0; --i) {
        int s = stateStack[i];
        for (const auto& nt : syncNTs) {
            int gotoState = grammar.getLRGoto(s, nt);
            if (gotoState != -1) {
                while ((int)stateStack.size() > i + 1) {
                    stateStack.pop_back();
                    symbolStack.pop_back();
                }

                const auto& followSets = grammar.getFollowSets();
                auto it = followSets.find(nt);
                if (it == followSets.end()) continue;
                const auto& follow = it->second;

                while (true) {
                    std::string a = grammar.mapTokenToTerminal(currentToken);
                    if (a == "$") break;
                    if (follow.find(a) != follow.end()) break;
                    advance();
                }

                symbolStack.push_back(nt);
                stateStack.push_back(gotoState);
                return true;
            }
        }
    }
    return false;
}

void ParserLR::printTraceStep(std::ostream& os, const std::string& actionStr) const {
    std::string symStr = "";
    for (size_t i = 0; i < symbolStack.size(); ++i) {
        symStr += symbolStack[i] + " ";
    }
    if (symStr.length() > 30) {
        symStr = "..." + symStr.substr(symStr.length() - 27);
    }

    std::string stateStr = "";
    for (size_t i = 0; i < stateStack.size(); ++i) {
        stateStr += std::to_string(stateStack[i]) + " ";
    }
    if (stateStr.length() > 20) {
        stateStr = "..." + stateStr.substr(stateStr.length() - 17);
    }

    std::string lookaheadStr = currentToken.lexeme;
    if (lookaheadStr.empty()) {
        lookaheadStr = "$";
    }

    os << std::left << std::setw(32) << symStr
       << " | " << std::setw(22) << stateStr
       << " | " << std::setw(15) << lookaheadStr
       << " | " << actionStr << "\n";
}
