#ifndef PARSER_RD_H
#define PARSER_RD_H

#include "lexer.h"
#include "symbol_table.h"
#include "error_handler.h"
#include "ast.h"
#include <memory>

class ParserRD {
public:
    ParserRD(Lexer& lexer, SymbolTableManager& symTable, ErrorHandler& errHandler);

    std::shared_ptr<ProgramNode> parse();

private:
    Lexer& lexer;
    SymbolTableManager& symTable;
    ErrorHandler& errHandler;
    Token currentToken;

    void advance();
    bool match(TokenType type);
    bool consume(TokenType type, const std::string& errMsg);
    void reportSyntaxError(const std::string& msg);
    void reportSemanticError(const std::string& msg);

    // Parsing routines for non-terminals
    std::shared_ptr<ProgramNode> parseProgram();
    void parseDeclPart(std::vector<std::shared_ptr<ASTNode>>& decls);
    void parseConstDecl(std::vector<std::shared_ptr<ASTNode>>& decls);
    void parseConstList(std::vector<std::shared_ptr<ASTNode>>& decls);
    void parseConstListTail(std::vector<std::shared_ptr<ASTNode>>& decls);
    std::string parseConstVal();

    void parseVarDecl(std::vector<std::shared_ptr<ASTNode>>& decls);
    void parseVarList(std::vector<std::shared_ptr<ASTNode>>& decls);
    void parseVarListTail(std::vector<std::shared_ptr<ASTNode>>& decls);
    void parseIdList(std::vector<std::string>& ids);
    void parseIdListTail(std::vector<std::string>& ids);
    void parseType(std::string& typeName, int& lower, int& upper, std::string& elemType);
    void parseSimpleType(std::string& typeName);
    void parseArrayType(int& lower, int& upper, std::string& elemType);

    void parseProcDecl(std::vector<std::shared_ptr<ASTNode>>& decls);
    std::shared_ptr<ProcDeclNode> parseProcDeclProj();
    void parseProcedureHeader(std::string& name, std::vector<std::pair<std::string, std::string>>& params);
    void parseFunctionHeader(std::string& name, std::vector<std::pair<std::string, std::string>>& params, std::string& retType);
    void parseFormalParams(std::vector<std::pair<std::string, std::string>>& params);
    void parseFormalList(std::vector<std::pair<std::string, std::string>>& params);
    void parseFormalListTail(std::vector<std::pair<std::string, std::string>>& params);

    std::shared_ptr<CompoundStmtNode> parseCompoundStmt();
    void parseStmtList(std::vector<std::shared_ptr<ASTNode>>& stmts);
    void parseStmtListTail(std::vector<std::shared_ptr<ASTNode>>& stmts);
    std::shared_ptr<ASTNode> parseStmt();
    std::shared_ptr<ASTNode> parseSimpleStmt();
    std::shared_ptr<ASTNode> parseAssignOrCall();
    std::shared_ptr<ASTNode> parseAssignOrCallTail(const std::string& idName, int line, int col);
    void parseActualParams(std::vector<std::shared_ptr<ASTNode>>& args);
    void parseExprList(std::vector<std::shared_ptr<ASTNode>>& exprs);
    void parseExprListTail(std::vector<std::shared_ptr<ASTNode>>& exprs);

    std::shared_ptr<WriteNode> parseWriteStmt();
    std::shared_ptr<ReadNode> parseReadStmt();
    std::shared_ptr<ASTNode> parseStructuredStmt();
    std::shared_ptr<IfNode> parseIfStmt();
    std::shared_ptr<ASTNode> parseElsePart();
    std::shared_ptr<WhileNode> parseWhileStmt();
    std::shared_ptr<ForNode> parseForStmt();

    std::shared_ptr<ASTNode> parseExpr();
    std::shared_ptr<ASTNode> parseRelOpExpr(std::shared_ptr<ASTNode> left);
    std::string parseRelOp();
    std::shared_ptr<ASTNode> parseSimpleExpr();
    std::shared_ptr<ASTNode> parseTermTail(std::shared_ptr<ASTNode> left);
    std::string parseAddOp();
    std::shared_ptr<ASTNode> parseTerm();
    std::shared_ptr<ASTNode> parseFactorTail(std::shared_ptr<ASTNode> left);
    std::string parseMulOp();
    std::shared_ptr<ASTNode> parseFactor();
    std::shared_ptr<ASTNode> parseFactorIdentTail(const std::string& idName, int line, int col);
};

#endif
