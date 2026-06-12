#include "grammar.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <iomanip>

Grammar::Grammar() : startSymbol("S'") {}

void Grammar::addProduction(const std::string& lhs, const std::vector<std::string>& rhs) {
    Production p;
    p.lhs = lhs;
    p.rhs = rhs;
    p.index = (int)productions.size();
    productions.push_back(p);
    nonTerminals.insert(lhs);
    for (const auto& sym : rhs) {
        if (sym != "epsilon" && !isNonTerminal(sym)) {
            terminals.insert(sym);
        }
    }
}

bool Grammar::isTerminal(const std::string& sym) const {
    if (sym.empty()) return false;
    return !isNonTerminal(sym) && sym != "epsilon";
}

bool Grammar::isNonTerminal(const std::string& sym) const {
    if (sym.empty()) return false;
    return (std::isupper(sym[0]) || sym == "S'");
}

void Grammar::initialize() {
    productions.clear();
    nonTerminals.clear();
    terminals.clear();

    // 0: Augmented Start production
    addProduction("S'", {"Program"});

    // 1: Program structure
    addProduction("Program", {"program", "id", ";", "DeclPart", "CompoundStmt", "."});

    // DeclPart
    addProduction("DeclPart", {"ConstDecl", "VarDecl", "ProcDecl"});

    // ConstDecl
    addProduction("ConstDecl", {"const", "ConstList"});
    addProduction("ConstDecl", {}); // epsilon

    // ConstList
    addProduction("ConstList", {"id", "=", "ConstVal", ";", "ConstListTail"});
    
    // ConstListTail
    addProduction("ConstListTail", {"id", "=", "ConstVal", ";", "ConstListTail"});
    addProduction("ConstListTail", {}); // epsilon

    // ConstVal
    addProduction("ConstVal", {"num"});
    addProduction("ConstVal", {"string"});
    addProduction("ConstVal", {"true"});
    addProduction("ConstVal", {"false"});

    // VarDecl
    addProduction("VarDecl", {"var", "VarList"});
    addProduction("VarDecl", {}); // epsilon

    // VarList
    addProduction("VarList", {"IdList", ":", "Type", ";", "VarListTail"});

    // VarListTail
    addProduction("VarListTail", {"IdList", ":", "Type", ";", "VarListTail"});
    addProduction("VarListTail", {}); // epsilon

    // IdList
    addProduction("IdList", {"id", "IdListTail"});

    // IdListTail
    addProduction("IdListTail", {",", "id", "IdListTail"});
    addProduction("IdListTail", {}); // epsilon

    // Type
    addProduction("Type", {"SimpleType"});
    addProduction("Type", {"ArrayType"});

    // SimpleType
    addProduction("SimpleType", {"integer"});
    addProduction("SimpleType", {"real"});
    addProduction("SimpleType", {"boolean"});
    addProduction("SimpleType", {"char"});

    // ArrayType
    addProduction("ArrayType", {"array", "[", "num", "..", "num", "]", "of", "SimpleType"});

    // ProcDecl
    addProduction("ProcDecl", {"ProcDeclProj", "ProcDecl"});
    addProduction("ProcDecl", {}); // epsilon

    // ProcDeclProj
    addProduction("ProcDeclProj", {"ProcedureHeader", ";", "DeclPart", "CompoundStmt", ";"});
    addProduction("ProcDeclProj", {"FunctionHeader", ";", "DeclPart", "CompoundStmt", ";"});

    // ProcedureHeader
    addProduction("ProcedureHeader", {"procedure", "id", "FormalParams"});

    // FunctionHeader
    addProduction("FunctionHeader", {"function", "id", "FormalParams", ":", "SimpleType"});

    // FormalParams
    addProduction("FormalParams", {"(", "FormalList", ")"});
    addProduction("FormalParams", {}); // epsilon

    // FormalList
    addProduction("FormalList", {"IdList", ":", "SimpleType", "FormalListTail"});

    // FormalListTail
    addProduction("FormalListTail", {";", "IdList", ":", "SimpleType", "FormalListTail"});
    addProduction("FormalListTail", {}); // epsilon

    // CompoundStmt
    addProduction("CompoundStmt", {"begin", "StmtList", "end"});

    // StmtList
    addProduction("StmtList", {"Stmt", "StmtListTail"});

    // StmtListTail
    addProduction("StmtListTail", {";", "Stmt", "StmtListTail"});
    addProduction("StmtListTail", {}); // epsilon

    // Stmt
    addProduction("Stmt", {"SimpleStmt"});
    addProduction("Stmt", {"StructuredStmt"});

    // SimpleStmt
    addProduction("SimpleStmt", {"AssignOrCall"});
    addProduction("SimpleStmt", {"WriteStmt"});
    addProduction("SimpleStmt", {"ReadStmt"});
    addProduction("SimpleStmt", {}); // epsilon

    // AssignOrCall
    addProduction("AssignOrCall", {"id", "AssignOrCallTail"});

    // AssignOrCallTail
    addProduction("AssignOrCallTail", {":=", "Expr"});
    addProduction("AssignOrCallTail", {"[", "Expr", "]", ":=", "Expr"});
    addProduction("AssignOrCallTail", {"ActualParams"});

    // ActualParams
    addProduction("ActualParams", {"(", "ExprList", ")"});
    addProduction("ActualParams", {}); // epsilon

    // ExprList
    addProduction("ExprList", {"Expr", "ExprListTail"});

    // ExprListTail
    addProduction("ExprListTail", {",", "Expr", "ExprListTail"});
    addProduction("ExprListTail", {}); // epsilon

    // WriteStmt
    addProduction("WriteStmt", {"writeln", "(", "ExprList", ")"});
    addProduction("WriteStmt", {"write", "(", "ExprList", ")"});

    // ReadStmt
    addProduction("ReadStmt", {"readln", "(", "IdList", ")"});
    addProduction("ReadStmt", {"read", "(", "IdList", ")"});

    // StructuredStmt
    addProduction("StructuredStmt", {"CompoundStmt"});
    addProduction("StructuredStmt", {"IfStmt"});
    addProduction("StructuredStmt", {"WhileStmt"});
    addProduction("StructuredStmt", {"ForStmt"});

    // IfStmt
    addProduction("IfStmt", {"if", "Expr", "then", "Stmt", "ElsePart"});

    // ElsePart
    addProduction("ElsePart", {"else", "Stmt"});
    addProduction("ElsePart", {}); // epsilon

    // WhileStmt
    addProduction("WhileStmt", {"while", "Expr", "do", "Stmt"});

    // ForStmt
    addProduction("ForStmt", {"for", "id", ":=", "Expr", "to", "Expr", "do", "Stmt"});

    // Expr
    addProduction("Expr", {"SimpleExpr", "RelOpExpr"});

    // RelOpExpr
    addProduction("RelOpExpr", {"RelOp", "SimpleExpr"});
    addProduction("RelOpExpr", {}); // epsilon

    // RelOp
    addProduction("RelOp", {"="});
    addProduction("RelOp", {"<>"});
    addProduction("RelOp", {"<"});
    addProduction("RelOp", {"<="});
    addProduction("RelOp", {">"});
    addProduction("RelOp", {">="});

    // SimpleExpr
    addProduction("SimpleExpr", {"Term", "TermTail"});

    // TermTail
    addProduction("TermTail", {"AddOp", "Term", "TermTail"});
    addProduction("TermTail", {}); // epsilon

    // AddOp
    addProduction("AddOp", {"+"});
    addProduction("AddOp", {"-"});
    addProduction("AddOp", {"or"});

    // Term
    addProduction("Term", {"Factor", "FactorTail"});

    // FactorTail
    addProduction("FactorTail", {"MulOp", "Factor", "FactorTail"});
    addProduction("FactorTail", {}); // epsilon

    // MulOp
    addProduction("MulOp", {"*"});
    addProduction("MulOp", {"/"});
    addProduction("MulOp", {"div"});
    addProduction("MulOp", {"mod"});
    addProduction("MulOp", {"and"});

    // Factor
    addProduction("Factor", {"id", "FactorIdentTail"});
    addProduction("Factor", {"num"});
    addProduction("Factor", {"string"});
    addProduction("Factor", {"true"});
    addProduction("Factor", {"false"});
    addProduction("Factor", {"(", "Expr", ")"});
    addProduction("Factor", {"not", "Factor"});

    // FactorIdentTail
    addProduction("FactorIdentTail", {"[", "Expr", "]"});
    addProduction("FactorIdentTail", {"ActualParams"});

    // S' is the only augmented symbol, terminals must include "$"
    terminals.insert("$");
    nonTerminals.erase("S'"); // S' is separate from standard grammar nonTerminals
}

bool Grammar::computeFirstOfSequence(const std::vector<std::string>& seq, std::set<std::string>& result) const {
    if (seq.empty()) {
        result.insert("epsilon");
        return true;
    }
    bool allEpsilon = true;
    for (const auto& sym : seq) {
        if (isTerminal(sym)) {
            result.insert(sym);
            allEpsilon = false;
            break;
        } else {
            auto it = first.find(sym);
            if (it == first.end()) {
                allEpsilon = false;
                break;
            }
            const auto& symFirst = it->second;
            for (const auto& f : symFirst) {
                if (f != "epsilon") {
                    result.insert(f);
                }
            }
            if (symFirst.find("epsilon") == symFirst.end()) {
                allEpsilon = false;
                break;
            }
        }
    }
    if (allEpsilon) {
        result.insert("epsilon");
    }
    return allEpsilon;
}

void Grammar::computeFirstAndFollow() {
    // 1. Initialize First sets for Terminals
    for (const auto& t : terminals) {
        first[t] = {t};
    }
    first["epsilon"] = {"epsilon"};

    for (const auto& nt : nonTerminals) {
        first[nt] = std::set<std::string>();
    }
    first["S'"] = std::set<std::string>();

    // 2. Compute First sets iteratively
    bool changed = true;
    while (changed) {
        changed = false;
        for (const auto& prod : productions) {
            std::set<std::string> rhsFirst;
            computeFirstOfSequence(prod.rhs, rhsFirst);
            size_t beforeSize = first[prod.lhs].size();
            first[prod.lhs].insert(rhsFirst.begin(), rhsFirst.end());
            if (first[prod.lhs].size() > beforeSize) {
                changed = true;
            }
        }
    }

    // 3. Initialize Follow sets
    for (const auto& nt : nonTerminals) {
        follow[nt] = std::set<std::string>();
    }
    follow["S'"] = {"$"};
    follow["Program"] = {"$"};

    // 4. Compute Follow sets iteratively
    changed = true;
    while (changed) {
        changed = false;
        for (const auto& prod : productions) {
            std::string A = prod.lhs;
            for (size_t i = 0; i < prod.rhs.size(); ++i) {
                std::string B = prod.rhs[i];
                if (isNonTerminal(B)) {
                    std::vector<std::string> beta(prod.rhs.begin() + i + 1, prod.rhs.end());
                    std::set<std::string> betaFirst;
                    bool betaDerivesEpsilon = computeFirstOfSequence(beta, betaFirst);
                    
                    size_t beforeSize = follow[B].size();
                    for (const auto& f : betaFirst) {
                        if (f != "epsilon") {
                            follow[B].insert(f);
                        }
                    }
                    if (betaDerivesEpsilon) {
                        const auto& followA = follow[A];
                        follow[B].insert(followA.begin(), followA.end());
                    }
                    if (follow[B].size() > beforeSize) {
                        changed = true;
                    }
                }
            }
        }
    }
}

void Grammar::buildLLTable() {
    llTable.clear();
    for (const auto& prod : productions) {
        if (prod.lhs == "S'") continue; // Skip augmented production for LL(1)

        std::set<std::string> rhsFirst;
        computeFirstOfSequence(prod.rhs, rhsFirst);

        for (const auto& t : rhsFirst) {
            if (t != "epsilon") {
                std::pair<std::string, std::string> cell = {prod.lhs, t};
                llTable[cell] = prod.index;
            }
        }

        if (rhsFirst.find("epsilon") != rhsFirst.end()) {
            for (const auto& b : follow[prod.lhs]) {
                std::pair<std::string, std::string> cell = {prod.lhs, b};
                // Resolve ambiguity if any, default to the first defined rule
                if (llTable.find(cell) == llTable.end()) {
                    llTable[cell] = prod.index;
                }
            }
        }
    }
}

int Grammar::getLLAction(const std::string& nonTerminal, const std::string& terminal) const {
    auto it = llTable.find({nonTerminal, terminal});
    if (it != llTable.end()) {
        return it->second;
    }
    return -1;
}

LRState Grammar::computeClosure(const LRState& itemSet) const {
    LRState closureSet = itemSet;
    bool changed = true;
    while (changed) {
        changed = false;
        LRState newItems;
        for (const auto& item : closureSet) {
            const Production& prod = productions[item.prodIdx];
            if (item.dotPos < (int)prod.rhs.size()) {
                std::string nextSym = prod.rhs[item.dotPos];
                if (isNonTerminal(nextSym) || nextSym == "Program") {
                    for (size_t pIdx = 0; pIdx < productions.size(); ++pIdx) {
                        if (productions[pIdx].lhs == nextSym) {
                            LRItem newItem = {(int)pIdx, 0};
                            if (closureSet.find(newItem) == closureSet.end() &&
                                newItems.find(newItem) == newItems.end()) {
                                newItems.insert(newItem);
                                changed = true;
                            }
                        }
                    }
                }
            }
        }
        if (changed) {
            closureSet.insert(newItems.begin(), newItems.end());
        }
    }
    return closureSet;
}

LRState Grammar::computeGoto(const LRState& itemSet, const std::string& symbol) const {
    LRState gotoSet;
    for (const auto& item : itemSet) {
        const Production& prod = productions[item.prodIdx];
        if (item.dotPos < (int)prod.rhs.size()) {
            if (prod.rhs[item.dotPos] == symbol) {
                gotoSet.insert({item.prodIdx, item.dotPos + 1});
            }
        }
    }
    return computeClosure(gotoSet);
}

void Grammar::buildSLRTable() {
    lrStates.clear();
    actionTable.clear();
    gotoTable.clear();

    // Augmented start is prod index 0: S' -> Program
    LRState s0 = computeClosure({{0, 0}});
    lrStates.push_back(s0);

    std::queue<int> stateQueue;
    stateQueue.push(0);

    std::map<LRState, int> stateIndices;
    stateIndices[s0] = 0;

    // Collect all grammar symbols for transition checks
    std::vector<std::string> allSymbols;
    for (const auto& nt : nonTerminals) allSymbols.push_back(nt);
    allSymbols.push_back("Program"); // Augmented symbol
    for (const auto& t : terminals) {
        if (t != "$") allSymbols.push_back(t);
    }

    while (!stateQueue.empty()) {
        int sIdx = stateQueue.front();
        stateQueue.pop();
        LRState state = lrStates[sIdx];

        for (const auto& sym : allSymbols) {
            LRState nextState = computeGoto(state, sym);
            if (nextState.empty()) continue;

            int nextSIdx;
            auto it = stateIndices.find(nextState);
            if (it == stateIndices.end()) {
                nextSIdx = (int)lrStates.size();
                lrStates.push_back(nextState);
                stateIndices[nextState] = nextSIdx;
                stateQueue.push(nextSIdx);
            } else {
                nextSIdx = it->second;
            }

            if (isTerminal(sym)) {
                actionTable[{sIdx, sym}] = {LRActionType::SHIFT, nextSIdx};
            } else {
                gotoTable[{sIdx, sym}] = nextSIdx;
            }
        }
    }

    // Set Reduce and Accept actions
    for (int sIdx = 0; sIdx < (int)lrStates.size(); ++sIdx) {
        const LRState& state = lrStates[sIdx];
        for (const auto& item : state) {
            const Production& prod = productions[item.prodIdx];
            if (item.dotPos == (int)prod.rhs.size()) {
                if (prod.lhs == "S'") {
                    actionTable[{sIdx, "$"}] = {LRActionType::ACCEPT, 0};
                } else {
                    for (const auto& followSym : follow[prod.lhs]) {
                        std::pair<int, std::string> cell = {sIdx, followSym};
                        if (actionTable.find(cell) != actionTable.end()) {
                            // Resolve Shift-Reduce Conflict: Shift preferred (dangling else)
                            if (actionTable[cell].type == LRActionType::SHIFT) {
                                continue;
                            }
                            // Resolve Reduce-Reduce Conflict: Keep smaller index
                            if (prod.index < actionTable[cell].target) {
                                actionTable[cell] = {LRActionType::REDUCE, prod.index};
                            }
                        } else {
                            actionTable[cell] = {LRActionType::REDUCE, prod.index};
                        }
                    }
                }
            }
        }
    }
}

LRAction Grammar::getLRAction(int state, const std::string& terminal) const {
    auto it = actionTable.find({state, terminal});
    if (it != actionTable.end()) {
        return it->second;
    }
    return {LRActionType::ERROR, 0};
}

int Grammar::getLRGoto(int state, const std::string& nonTerminal) const {
    auto it = gotoTable.find({state, nonTerminal});
    if (it != gotoTable.end()) {
        return it->second;
    }
    return -1;
}

std::string Grammar::mapTokenToTerminal(const Token& token) const {
    switch (token.type) {
        case TokenType::PROGRAM: return "program";
        case TokenType::CONST: return "const";
        case TokenType::VAR: return "var";
        case TokenType::PROCEDURE: return "procedure";
        case TokenType::FUNCTION: return "function";
        case TokenType::BEGIN: return "begin";
        case TokenType::END: return "end";
        case TokenType::INTEGER: return "integer";
        case TokenType::REAL: return "real";
        case TokenType::BOOLEAN: return "boolean";
        case TokenType::CHAR: return "char";
        case TokenType::ARRAY: return "array";
        case TokenType::OF: return "of";
        case TokenType::IF: return "if";
        case TokenType::THEN: return "then";
        case TokenType::ELSE: return "else";
        case TokenType::WHILE: return "while";
        case TokenType::DO: return "do";
        case TokenType::FOR: return "for";
        case TokenType::TO: return "to";
        case TokenType::WRITELN: return "writeln";
        case TokenType::WRITE: return "write";
        case TokenType::READLN: return "readln";
        case TokenType::READ: return "read";
        case TokenType::DIV: return "div";
        case TokenType::MOD: return "mod";
        case TokenType::AND: return "and";
        case TokenType::OR: return "or";
        case TokenType::NOT: return "not";
        case TokenType::TRUE: return "true";
        case TokenType::FALSE: return "false";
        case TokenType::IDENTIFIER: return "id";
        case TokenType::NUM_INT: return "num";
        case TokenType::NUM_REAL: return "num";
        case TokenType::STRING_LIT: return "string";
        case TokenType::CHAR_LIT: return "string";
        case TokenType::PLUS: return "+";
        case TokenType::MINUS: return "-";
        case TokenType::STAR: return "*";
        case TokenType::SLASH: return "/";
        case TokenType::EQUAL: return "=";
        case TokenType::NEQ: return "<>";
        case TokenType::LESS: return "<";
        case TokenType::LEQ: return "<=";
        case TokenType::GREATER: return ">";
        case TokenType::GEQ: return ">=";
        case TokenType::ASSIGN: return ":=";
        case TokenType::COLON: return ":";
        case TokenType::SEMICOLON: return ";";
        case TokenType::COMMA: return ",";
        case TokenType::DOT: return ".";
        case TokenType::LBRACKET: return "[";
        case TokenType::RBRACKET: return "]";
        case TokenType::LPAREN: return "(";
        case TokenType::RPAREN: return ")";
        case TokenType::RANGE: return "..";
        case TokenType::END_OF_FILE: return "$";
        default: return "";
    }
}

void Grammar::printFirstAndFollow(std::ostream& os) const {
    os << "| Non-Terminal | FIRST Set | FOLLOW Set |\n";
    os << "|--------------|-----------|------------|\n";
    for (const auto& nt : nonTerminals) {
        os << "| **" << nt << "** | { ";
        bool firstVal = true;
        for (const auto& s : first.at(nt)) {
            if (!firstVal) os << ", ";
            os << "`" << s << "`";
            firstVal = false;
        }
        os << " } | { ";
        firstVal = true;
        for (const auto& s : follow.at(nt)) {
            if (!firstVal) os << ", ";
            os << "`" << s << "`";
            firstVal = false;
        }
        os << " } |\n";
    }
}

void Grammar::printLLTable(std::ostream& os) const {
    os << "### LL(1) Parsing Table Entries\n\n";
    os << "| Non-Terminal | Terminal Lookahead | Applied Production Rule |\n";
    os << "|--------------|--------------------|------------------------|\n";
    for (const auto& nt : nonTerminals) {
        for (const auto& t : terminals) {
            int pIdx = getLLAction(nt, t);
            if (pIdx != -1) {
                const auto& prod = productions[pIdx];
                os << "| " << nt << " | `" << t << "` | " << prod.lhs << " &rarr; ";
                if (prod.rhs.empty()) {
                    os << "&epsilon;";
                } else {
                    for (const auto& sym : prod.rhs) os << sym << " ";
                }
                os << " (Rule " << pIdx << ") |\n";
            }
        }
    }
}

void Grammar::printSLRTable(std::ostream& os) const {
    os << "### SLR(1) Action & Goto Table (Sparse Representation)\n\n";
    os << "| State | Lookahead Symbol | Action / Goto Decision |\n";
    os << "|-------|------------------|------------------------|\n";
    
    // Sort table content for readability
    for (size_t sIdx = 0; sIdx < lrStates.size(); ++sIdx) {
        for (const auto& t : terminals) {
            auto it = actionTable.find({(int)sIdx, t});
            if (it != actionTable.end()) {
                const auto& action = it->second;
                os << "| State " << sIdx << " | `" << t << "` | ";
                if (action.type == LRActionType::SHIFT) {
                    os << "**Shift** to State " << action.target;
                } else if (action.type == LRActionType::REDUCE) {
                    const auto& prod = productions[action.target];
                    os << "**Reduce** by Rule " << action.target << " (" << prod.lhs << " &rarr; ";
                    if (prod.rhs.empty()) os << "&epsilon;";
                    else {
                        for (const auto& sym : prod.rhs) os << sym << " ";
                    }
                    os << ")";
                } else if (action.type == LRActionType::ACCEPT) {
                    os << "<span style='color:green;'>**ACCEPT**</span>";
                }
                os << " |\n";
            }
        }
        for (const auto& nt : nonTerminals) {
            auto it = gotoTable.find({(int)sIdx, nt});
            if (it != gotoTable.end()) {
                os << "| State " << sIdx << " | **" << nt << "** | **Goto** State " << it->second << " |\n";
            }
        }
        // Augmented check
        auto it = gotoTable.find({(int)sIdx, "Program"});
        if (it != gotoTable.end()) {
            os << "| State " << sIdx << " | **Program** | **Goto** State " << it->second << " |\n";
        }
    }
}

void Grammar::saveTablesToMarkdown(const std::string& filepath) const {
    std::ofstream os(filepath);
    if (!os.is_open()) {
        std::cerr << "Failed to save grammar tables to " << filepath << std::endl;
        return;
    }

    os << "# Parser Tables & Grammar Analysis\n\n";
    
    os << "## 1. FIRST & FOLLOW Sets\n\n";
    printFirstAndFollow(os);
    os << "\n---\n\n";

    os << "## 2. LL(1) Predictive Parsing Table\n\n";
    printLLTable(os);
    os << "\n---\n\n";

    os << "## 3. SLR(1) Parsing Table\n\n";
    printSLRTable(os);
    os.close();
}
