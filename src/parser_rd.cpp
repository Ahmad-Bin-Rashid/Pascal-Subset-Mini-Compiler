#include "parser_rd.h"

ParserRD::ParserRD(Lexer& lexer, SymbolTableManager& symTable, ErrorHandler& errHandler)
    : lexer(lexer), symTable(symTable), errHandler(errHandler) {
    advance();
}

void ParserRD::advance() {
    currentToken = lexer.getNextToken();
}

bool ParserRD::match(TokenType type) {
    return currentToken.type == type;
}

bool ParserRD::consume(TokenType type, const std::string& errMsg) {
    if (match(type)) {
        advance();
        return true;
    }
    reportSyntaxError(errMsg);
    return false;
}

void ParserRD::reportSyntaxError(const std::string& msg) {
    errHandler.report("Syntactic", currentToken.line, currentToken.col, msg + " (Found: '" + currentToken.lexeme + "')");
}

void ParserRD::reportSemanticError(const std::string& msg) {
    errHandler.report("Semantic", currentToken.line, currentToken.col, msg);
}

std::shared_ptr<ProgramNode> ParserRD::parse() {
    return parseProgram();
}

std::shared_ptr<ProgramNode> ParserRD::parseProgram() {
    auto progNode = std::make_shared<ProgramNode>();
    if (consume(TokenType::PROGRAM, "Expected 'program' keyword")) {
        progNode->name = currentToken.lexeme;
        consume(TokenType::IDENTIFIER, "Expected program name identifier");
        consume(TokenType::SEMICOLON, "Expected ';' after program name");
        parseDeclPart(progNode->declarations);
        progNode->compoundStmt = parseCompoundStmt();
        consume(TokenType::DOT, "Expected '.' at end of program");
    }
    return progNode;
}

void ParserRD::parseDeclPart(std::vector<std::shared_ptr<ASTNode>>& decls) {
    parseConstDecl(decls);
    parseVarDecl(decls);
    parseProcDecl(decls);
}

void ParserRD::parseConstDecl(std::vector<std::shared_ptr<ASTNode>>& decls) {
    if (match(TokenType::CONST)) {
        advance();
        parseConstList(decls);
    }
}

void ParserRD::parseConstList(std::vector<std::shared_ptr<ASTNode>>& decls) {
    std::string name = currentToken.lexeme;
    int line = currentToken.line;
    if (consume(TokenType::IDENTIFIER, "Expected identifier in constant list")) {
        consume(TokenType::EQUAL, "Expected '=' in constant declaration");

        // Capture token type BEFORE parseConstVal() advances
        TokenType constTokType = currentToken.type;
        std::string val = parseConstVal();
        consume(TokenType::SEMICOLON, "Expected ';' after constant value");

        Symbol sym;
        sym.name = name;
        sym.kind = "constant";
        if (constTokType == TokenType::TRUE || constTokType == TokenType::FALSE)
            sym.type = "boolean";
        else if (constTokType == TokenType::STRING_LIT)
            sym.type = "string";
        else if (constTokType == TokenType::CHAR_LIT)
            sym.type = "char";
        else if (constTokType == TokenType::NUM_REAL)
            sym.type = "real";
        else
            sym.type = "integer";
        sym.scopeLevel = symTable.getCurrentLevel();
        sym.lineNum = line;

        if (!symTable.insert(sym)) {
            reportSemanticError("Duplicate declaration of constant: " + name);
        }

        auto constNode = std::make_shared<ConstDeclNode>();
        constNode->name = name;
        constNode->val = val;
        decls.push_back(constNode);

        parseConstListTail(decls);
    }
}

void ParserRD::parseConstListTail(std::vector<std::shared_ptr<ASTNode>>& decls) {
    if (match(TokenType::IDENTIFIER)) {
        parseConstList(decls);
    }
}

std::string ParserRD::parseConstVal() {
    std::string val = currentToken.lexeme;
    if (match(TokenType::NUM_INT) || match(TokenType::NUM_REAL) || match(TokenType::STRING_LIT) || match(TokenType::CHAR_LIT) || match(TokenType::TRUE) || match(TokenType::FALSE)) {
        advance();
        return val;
    }
    reportSyntaxError("Expected numeric, string, or boolean value");
    return "";
}

void ParserRD::parseVarDecl(std::vector<std::shared_ptr<ASTNode>>& decls) {
    if (match(TokenType::VAR)) {
        advance();
        parseVarList(decls);
    }
}

void ParserRD::parseVarList(std::vector<std::shared_ptr<ASTNode>>& decls) {
    std::vector<std::string> ids;
    parseIdList(ids);
    consume(TokenType::COLON, "Expected ':' after identifier list");

    std::string typeName;
    int arrayLower = 0;
    int arrayUpper = 0;
    std::string elemType = "";
    parseType(typeName, arrayLower, arrayUpper, elemType);
    consume(TokenType::SEMICOLON, "Expected ';' after type declaration");

    for (const auto& id : ids) {
        Symbol sym;
        sym.name = id;
        sym.scopeLevel = symTable.getCurrentLevel();
        sym.lineNum = currentToken.line;

        if (typeName == "array") {
            sym.kind = "array";
            sym.type = "array";
            sym.arrayLower = arrayLower;
            sym.arrayUpper = arrayUpper;
            sym.arrayElemType = elemType;
        } else {
            sym.kind = "variable";
            sym.type = typeName;
        }

        if (!symTable.insert(sym)) {
            reportSemanticError("Duplicate declaration of variable: " + id);
        }
    }

    auto varNode = std::make_shared<VarDeclNode>();
    varNode->names = ids;
    varNode->type = (typeName == "array") ? ("array[" + std::to_string(arrayLower) + ".." + std::to_string(arrayUpper) + "] of " + elemType) : typeName;
    decls.push_back(varNode);

    parseVarListTail(decls);
}

void ParserRD::parseVarListTail(std::vector<std::shared_ptr<ASTNode>>& decls) {
    if (match(TokenType::IDENTIFIER)) {
        parseVarList(decls);
    }
}

void ParserRD::parseIdList(std::vector<std::string>& ids) {
    std::string id = currentToken.lexeme;
    if (consume(TokenType::IDENTIFIER, "Expected identifier in ID list")) {
        ids.push_back(id);
        parseIdListTail(ids);
    }
}

void ParserRD::parseIdListTail(std::vector<std::string>& ids) {
    if (match(TokenType::COMMA)) {
        advance();
        parseIdList(ids);
    }
}

void ParserRD::parseType(std::string& typeName, int& lower, int& upper, std::string& elemType) {
    if (match(TokenType::ARRAY)) {
        parseArrayType(lower, upper, elemType);
        typeName = "array";
    } else {
        parseSimpleType(typeName);
    }
}

void ParserRD::parseSimpleType(std::string& typeName) {
    typeName = currentToken.lexeme;
    if (match(TokenType::INTEGER) || match(TokenType::REAL) || match(TokenType::BOOLEAN) || match(TokenType::CHAR)) {
        advance();
    } else {
        reportSyntaxError("Expected type name (integer, real, boolean, char)");
    }
}

void ParserRD::parseArrayType(int& lower, int& upper, std::string& elemType) {
    consume(TokenType::ARRAY, "Expected 'array' keyword");
    consume(TokenType::LBRACKET, "Expected '[' in array index declaration");
    
    std::string lowVal = currentToken.lexeme;
    consume(TokenType::NUM_INT, "Expected lower bound integer");
    lower = std::stoi(lowVal);

    consume(TokenType::RANGE, "Expected '..' range indicator");

    std::string highVal = currentToken.lexeme;
    consume(TokenType::NUM_INT, "Expected upper bound integer");
    upper = std::stoi(highVal);

    consume(TokenType::RBRACKET, "Expected ']' in array index declaration");
    consume(TokenType::OF, "Expected 'of' keyword");
    parseSimpleType(elemType);
}

void ParserRD::parseProcDecl(std::vector<std::shared_ptr<ASTNode>>& decls) {
    if (match(TokenType::PROCEDURE) || match(TokenType::FUNCTION)) {
        auto node = parseProcDeclProj();
        if (node) {
            decls.push_back(node);
        }
        parseProcDecl(decls);
    }
}

std::shared_ptr<ProcDeclNode> ParserRD::parseProcDeclProj() {
    auto node = std::make_shared<ProcDeclNode>();
    std::string name;
    std::vector<std::pair<std::string, std::string>> params;
    std::string retType = "";

    bool isFunc = false;
    int line = currentToken.line;
    if (match(TokenType::PROCEDURE)) {
        parseProcedureHeader(name, params);
    } else if (match(TokenType::FUNCTION)) {
        isFunc = true;
        parseFunctionHeader(name, params, retType);
    } else {
        reportSyntaxError("Expected procedure or function header");
        return nullptr;
    }

    node->name = name;
    node->isFunction = isFunc;
    node->params = params;
    node->returnType = retType;

    Symbol sym;
    sym.name = name;
    sym.kind = isFunc ? "function" : "procedure";
    sym.type = isFunc ? retType : "void";
    sym.scopeLevel = symTable.getCurrentLevel();
    sym.lineNum = line;
    sym.parameters = params;
    sym.returnType = retType;
    if (!symTable.insert(sym)) {
        reportSemanticError("Duplicate declaration of " + sym.kind + ": " + name);
    }

    symTable.enterScope(name);

    for (const auto& param : params) {
        Symbol paramSym;
        paramSym.name = param.first;
        paramSym.kind = "variable";
        paramSym.type = param.second;
        paramSym.scopeLevel = symTable.getCurrentLevel();
        paramSym.lineNum = line;
        if (!symTable.insert(paramSym)) {
            reportSemanticError("Duplicate formal parameter name: " + param.first);
        }
    }

    if (isFunc) {
        Symbol funcRetSym;
        funcRetSym.name = name;
        funcRetSym.kind = "variable";
        funcRetSym.type = retType;
        funcRetSym.scopeLevel = symTable.getCurrentLevel();
        funcRetSym.lineNum = line;
        symTable.insert(funcRetSym);
    }

    consume(TokenType::SEMICOLON, "Expected ';' after header");
    parseDeclPart(node->declarations);
    node->body = parseCompoundStmt();
    consume(TokenType::SEMICOLON, "Expected ';' after block");

    symTable.exitScope();

    return node;
}

void ParserRD::parseProcedureHeader(std::string& name, std::vector<std::pair<std::string, std::string>>& params) {
    consume(TokenType::PROCEDURE, "Expected 'procedure' keyword");
    name = currentToken.lexeme;
    consume(TokenType::IDENTIFIER, "Expected procedure name identifier");
    parseFormalParams(params);
}

void ParserRD::parseFunctionHeader(std::string& name, std::vector<std::pair<std::string, std::string>>& params, std::string& retType) {
    consume(TokenType::FUNCTION, "Expected 'function' keyword");
    name = currentToken.lexeme;
    consume(TokenType::IDENTIFIER, "Expected function name identifier");
    parseFormalParams(params);
    consume(TokenType::COLON, "Expected ':' before return type");
    parseSimpleType(retType);
}

void ParserRD::parseFormalParams(std::vector<std::pair<std::string, std::string>>& params) {
    if (match(TokenType::LPAREN)) {
        advance();
        parseFormalList(params);
        consume(TokenType::RPAREN, "Expected ')'");
    }
}

void ParserRD::parseFormalList(std::vector<std::pair<std::string, std::string>>& params) {
    std::vector<std::string> ids;
    parseIdList(ids);
    consume(TokenType::COLON, "Expected ':'");
    std::string typeName;
    parseSimpleType(typeName);

    for (const auto& id : ids) {
        params.push_back({id, typeName});
    }

    parseFormalListTail(params);
}

void ParserRD::parseFormalListTail(std::vector<std::pair<std::string, std::string>>& params) {
    if (match(TokenType::SEMICOLON)) {
        advance();
        parseFormalList(params);
    }
}

std::shared_ptr<CompoundStmtNode> ParserRD::parseCompoundStmt() {
    auto node = std::make_shared<CompoundStmtNode>();
    consume(TokenType::BEGIN, "Expected 'begin' keyword");
    parseStmtList(node->statements);
    consume(TokenType::END, "Expected 'end' keyword");
    return node;
}

void ParserRD::parseStmtList(std::vector<std::shared_ptr<ASTNode>>& stmts) {
    auto stmt = parseStmt();
    if (stmt) {
        stmts.push_back(stmt);
    }
    parseStmtListTail(stmts);
}

void ParserRD::parseStmtListTail(std::vector<std::shared_ptr<ASTNode>>& stmts) {
    if (match(TokenType::SEMICOLON)) {
        advance();
        parseStmtList(stmts);
    }
}

std::shared_ptr<ASTNode> ParserRD::parseStmt() {
    if (match(TokenType::BEGIN) || match(TokenType::IF) || match(TokenType::WHILE) || match(TokenType::FOR)) {
        return parseStructuredStmt();
    }
    return parseSimpleStmt();
}

std::shared_ptr<ASTNode> ParserRD::parseSimpleStmt() {
    if (match(TokenType::IDENTIFIER)) {
        return parseAssignOrCall();
    } else if (match(TokenType::WRITELN) || match(TokenType::WRITE)) {
        return parseWriteStmt();
    } else if (match(TokenType::READLN) || match(TokenType::READ)) {
        return parseReadStmt();
    }
    // Epsilon
    return nullptr;
}

std::shared_ptr<ASTNode> ParserRD::parseAssignOrCall() {
    std::string name = currentToken.lexeme;
    int line = currentToken.line;
    int col = currentToken.col;
    if (consume(TokenType::IDENTIFIER, "Expected identifier")) {
        return parseAssignOrCallTail(name, line, col);
    }
    return nullptr;
}

std::shared_ptr<ASTNode> ParserRD::parseAssignOrCallTail(const std::string& idName, int line, int col) {
    if (match(TokenType::ASSIGN)) {
        advance();
        
        Symbol* sym = symTable.lookup(idName);
        if (sym) {
            if (sym->kind == "constant") {
                errHandler.report("Semantic", line, col, "Cannot assign to constant: " + idName);
            } else if (sym->kind == "procedure") {
                errHandler.report("Semantic", line, col, "Cannot assign to procedure: " + idName);
            }
        } else {
            errHandler.report("Semantic", line, col, "Undeclared identifier: " + idName);
        }

        auto assignNode = std::make_shared<AssignNode>();
        assignNode->name = idName;
        assignNode->expr = parseExpr();
        return assignNode;

    } else if (match(TokenType::LBRACKET)) {
        advance();
        auto arrayIdx = parseExpr();
        consume(TokenType::RBRACKET, "Expected ']' after array index");
        consume(TokenType::ASSIGN, "Expected ':=' in array assignment");

        Symbol* sym = symTable.lookup(idName);
        if (sym) {
            if (sym->kind != "array") {
                errHandler.report("Semantic", line, col, "Identifier " + idName + " is not an array");
            }
        } else {
            errHandler.report("Semantic", line, col, "Undeclared identifier: " + idName);
        }

        auto assignNode = std::make_shared<AssignNode>();
        assignNode->name = idName;
        assignNode->arrayIndex = arrayIdx;
        assignNode->expr = parseExpr();
        return assignNode;

    } else {
        std::vector<std::shared_ptr<ASTNode>> args;
        parseActualParams(args);

        Symbol* sym = symTable.lookup(idName);
        if (sym) {
            if (sym->kind != "procedure" && sym->kind != "function") {
                errHandler.report("Semantic", line, col, "Identifier " + idName + " is not callable");
            } else {
                if (args.size() != sym->parameters.size()) {
                    errHandler.report("Semantic", line, col, "Parameter count mismatch in call to " + idName + 
                        " (Expected: " + std::to_string(sym->parameters.size()) + ", got: " + std::to_string(args.size()) + ")");
                }
            }
        } else {
            errHandler.report("Semantic", line, col, "Undeclared identifier: " + idName);
        }

        auto callNode = std::make_shared<CallNode>();
        callNode->name = idName;
        callNode->args = args;
        return callNode;
    }
}

void ParserRD::parseActualParams(std::vector<std::shared_ptr<ASTNode>>& args) {
    if (match(TokenType::LPAREN)) {
        advance();
        parseExprList(args);
        consume(TokenType::RPAREN, "Expected ')'");
    }
}

void ParserRD::parseExprList(std::vector<std::shared_ptr<ASTNode>>& exprs) {
    auto expr = parseExpr();
    if (expr) {
        exprs.push_back(expr);
    }
    parseExprListTail(exprs);
}

void ParserRD::parseExprListTail(std::vector<std::shared_ptr<ASTNode>>& exprs) {
    if (match(TokenType::COMMA)) {
        advance();
        parseExprList(exprs);
    }
}

std::shared_ptr<WriteNode> ParserRD::parseWriteStmt() {
    auto node = std::make_shared<WriteNode>();
    node->isWriteln = match(TokenType::WRITELN);
    advance();

    consume(TokenType::LPAREN, "Expected '(' in print statement");
    parseExprList(node->args);
    consume(TokenType::RPAREN, "Expected ')' in print statement");
    return node;
}

std::shared_ptr<ReadNode> ParserRD::parseReadStmt() {
    auto node = std::make_shared<ReadNode>();
    node->isReadln = match(TokenType::READLN);
    advance();

    consume(TokenType::LPAREN, "Expected '(' in read statement");
    std::vector<std::shared_ptr<ASTNode>> list;
    parseExprList(list);
    for (const auto& item : list) {
        node->vars.push_back(item);
    }
    consume(TokenType::RPAREN, "Expected ')' in read statement");
    return node;
}

std::shared_ptr<ASTNode> ParserRD::parseStructuredStmt() {
    if (match(TokenType::BEGIN)) {
        return parseCompoundStmt();
    } else if (match(TokenType::IF)) {
        return parseIfStmt();
    } else if (match(TokenType::WHILE)) {
        return parseWhileStmt();
    } else if (match(TokenType::FOR)) {
        return parseForStmt();
    }
    return nullptr;
}

std::shared_ptr<IfNode> ParserRD::parseIfStmt() {
    auto node = std::make_shared<IfNode>();
    consume(TokenType::IF, "Expected 'if'");
    node->cond = parseExpr();
    consume(TokenType::THEN, "Expected 'then' keyword");
    node->thenBranch = parseStmt();
    node->elseBranch = parseElsePart();
    return node;
}

std::shared_ptr<ASTNode> ParserRD::parseElsePart() {
    if (match(TokenType::ELSE)) {
        advance();
        return parseStmt();
    }
    return nullptr;
}

std::shared_ptr<WhileNode> ParserRD::parseWhileStmt() {
    auto node = std::make_shared<WhileNode>();
    consume(TokenType::WHILE, "Expected 'while'");
    node->cond = parseExpr();
    consume(TokenType::DO, "Expected 'do' keyword");
    node->body = parseStmt();
    return node;
}

std::shared_ptr<ForNode> ParserRD::parseForStmt() {
    auto node = std::make_shared<ForNode>();
    consume(TokenType::FOR, "Expected 'for' keyword");
    
    std::string varName = currentToken.lexeme;
    int line = currentToken.line;
    int col = currentToken.col;
    consume(TokenType::IDENTIFIER, "Expected loop variable identifier");
    node->varName = varName;

    Symbol* sym = symTable.lookup(varName);
    if (!sym) {
        errHandler.report("Semantic", line, col, "Undeclared loop variable: " + varName);
    } else if (sym->kind == "constant") {
        errHandler.report("Semantic", line, col, "Cannot assign loop variable to constant: " + varName);
    }

    consume(TokenType::ASSIGN, "Expected ':=' loop initializer");
    node->startExpr = parseExpr();
    consume(TokenType::TO, "Expected 'to' keyword");
    node->endExpr = parseExpr();
    consume(TokenType::DO, "Expected 'do' keyword");
    node->body = parseStmt();
    return node;
}

std::shared_ptr<ASTNode> ParserRD::parseExpr() {
    auto left = parseSimpleExpr();
    return parseRelOpExpr(left);
}

std::shared_ptr<ASTNode> ParserRD::parseRelOpExpr(std::shared_ptr<ASTNode> left) {
    if (match(TokenType::EQUAL) || match(TokenType::NEQ) || match(TokenType::LESS) || match(TokenType::LEQ) || match(TokenType::GREATER) || match(TokenType::GEQ)) {
        std::string op = parseRelOp();
        auto right = parseSimpleExpr();
        auto binOp = std::make_shared<BinaryOpNode>();
        binOp->op = op;
        binOp->left = left;
        binOp->right = right;
        return binOp;
    }
    return left;
}

std::string ParserRD::parseRelOp() {
    std::string op = currentToken.lexeme;
    advance();
    return op;
}

std::shared_ptr<ASTNode> ParserRD::parseSimpleExpr() {
    auto left = parseTerm();
    return parseTermTail(left);
}

std::shared_ptr<ASTNode> ParserRD::parseTermTail(std::shared_ptr<ASTNode> left) {
    if (match(TokenType::PLUS) || match(TokenType::MINUS) || match(TokenType::OR)) {
        std::string op = parseAddOp();
        auto right = parseTerm();
        auto binOp = std::make_shared<BinaryOpNode>();
        binOp->op = op;
        binOp->left = left;
        binOp->right = right;
        return parseTermTail(binOp);
    }
    return left;
}

std::string ParserRD::parseAddOp() {
    std::string op = currentToken.lexeme;
    advance();
    return op;
}

std::shared_ptr<ASTNode> ParserRD::parseTerm() {
    auto left = parseFactor();
    return parseFactorTail(left);
}

std::shared_ptr<ASTNode> ParserRD::parseFactorTail(std::shared_ptr<ASTNode> left) {
    if (match(TokenType::STAR) || match(TokenType::SLASH) || match(TokenType::DIV) || match(TokenType::MOD) || match(TokenType::AND)) {
        std::string op = parseMulOp();
        auto right = parseFactor();
        auto binOp = std::make_shared<BinaryOpNode>();
        binOp->op = op;
        binOp->left = left;
        binOp->right = right;
        return parseFactorTail(binOp);
    }
    return left;
}

std::string ParserRD::parseMulOp() {
    std::string op = currentToken.lexeme;
    advance();
    return op;
}

std::shared_ptr<ASTNode> ParserRD::parseFactor() {
    if (match(TokenType::IDENTIFIER)) {
        std::string name = currentToken.lexeme;
        int line = currentToken.line;
        int col = currentToken.col;
        advance();
        return parseFactorIdentTail(name, line, col);
    } else if (match(TokenType::NUM_INT)) {
        std::string val = currentToken.lexeme;
        advance();
        auto lit = std::make_shared<LiteralNode>();
        lit->val = val;
        lit->type = "integer";
        return lit;
    } else if (match(TokenType::NUM_REAL)) {
        std::string val = currentToken.lexeme;
        advance();
        auto lit = std::make_shared<LiteralNode>();
        lit->val = val;
        lit->type = "real";
        return lit;
    } else if (match(TokenType::STRING_LIT)) {
        std::string val = currentToken.lexeme;
        advance();
        auto lit = std::make_shared<LiteralNode>();
        lit->val = val;
        lit->type = "string";
        return lit;
    } else if (match(TokenType::CHAR_LIT)) {
        std::string val = currentToken.lexeme;
        advance();
        auto lit = std::make_shared<LiteralNode>();
        lit->val = val;
        lit->type = "char";
        return lit;
    } else if (match(TokenType::TRUE)) {
        advance();
        auto lit = std::make_shared<LiteralNode>();
        lit->val = "true";
        lit->type = "boolean";
        return lit;
    } else if (match(TokenType::FALSE)) {
        advance();
        auto lit = std::make_shared<LiteralNode>();
        lit->val = "false";
        lit->type = "boolean";
        return lit;
    } else if (match(TokenType::LPAREN)) {
        advance();
        auto expr = parseExpr();
        consume(TokenType::RPAREN, "Expected ')'");
        return expr;
    } else if (match(TokenType::NOT)) {
        advance();
        auto notNode = std::make_shared<UnaryOpNode>();
        notNode->op = "not";
        notNode->expr = parseFactor();
        return notNode;
    } else {
        reportSyntaxError("Expected expression factor");
        advance();
        return nullptr;
    }
}

std::shared_ptr<ASTNode> ParserRD::parseFactorIdentTail(const std::string& idName, int line, int col) {
    if (match(TokenType::LBRACKET)) {
        advance();
        auto arrayIdx = parseExpr();
        consume(TokenType::RBRACKET, "Expected ']' after array index");

        Symbol* sym = symTable.lookup(idName);
        if (sym) {
            if (sym->kind != "array") {
                errHandler.report("Semantic", line, col, "Identifier " + idName + " is not an array");
            }
        } else {
            errHandler.report("Semantic", line, col, "Undeclared identifier: " + idName);
        }

        auto identNode = std::make_shared<IdentifierNode>();
        identNode->name = idName;
        identNode->arrayIndex = arrayIdx;
        return identNode;
    } else if (match(TokenType::LPAREN)) {
        std::vector<std::shared_ptr<ASTNode>> args;
        parseActualParams(args);

        Symbol* sym = symTable.lookup(idName);
        if (sym) {
            if (sym->kind != "function") {
                errHandler.report("Semantic", line, col, "Identifier " + idName + " is not a function");
            } else {
                if (args.size() != sym->parameters.size()) {
                    errHandler.report("Semantic", line, col, "Parameter count mismatch in function call " + idName + 
                        " (Expected: " + std::to_string(sym->parameters.size()) + ", got: " + std::to_string(args.size()) + ")");
                }
            }
        } else {
            errHandler.report("Semantic", line, col, "Undeclared identifier: " + idName);
        }

        auto callNode = std::make_shared<CallNode>();
        callNode->name = idName;
        callNode->args = args;
        return callNode;
    } else {
        Symbol* sym = symTable.lookup(idName);
        if (!sym) {
            errHandler.report("Semantic", line, col, "Undeclared identifier: " + idName);
        } else if (sym->kind == "procedure") {
            errHandler.report("Semantic", line, col, "Procedure " + idName + " cannot be used in expressions");
        }

        auto identNode = std::make_shared<IdentifierNode>();
        identNode->name = idName;
        return identNode;
    }
}
