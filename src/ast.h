#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void print(std::ostream& os, int indent = 0) const = 0;
};

class ProgramNode : public ASTNode {
public:
    std::string name;
    std::vector<std::shared_ptr<ASTNode>> declarations;
    std::shared_ptr<ASTNode> compoundStmt;

    void print(std::ostream& os, int indent = 0) const override;
};

class VarDeclNode : public ASTNode {
public:
    std::vector<std::string> names;
    std::string type;

    void print(std::ostream& os, int indent = 0) const override;
};

class ConstDeclNode : public ASTNode {
public:
    std::string name;
    std::string val;

    void print(std::ostream& os, int indent = 0) const override;
};

class ProcDeclNode : public ASTNode {
public:
    std::string name;
    bool isFunction = false;
    std::vector<std::pair<std::string, std::string>> params;
    std::string returnType = "";
    std::vector<std::shared_ptr<ASTNode>> declarations;
    std::shared_ptr<ASTNode> body;

    void print(std::ostream& os, int indent = 0) const override;
};

class AssignNode : public ASTNode {
public:
    std::string name;
    std::shared_ptr<ASTNode> arrayIndex = nullptr;
    std::shared_ptr<ASTNode> expr;

    void print(std::ostream& os, int indent = 0) const override;
};

class CallNode : public ASTNode {
public:
    std::string name;
    std::vector<std::shared_ptr<ASTNode>> args;

    void print(std::ostream& os, int indent = 0) const override;
};

class IfNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> cond;
    std::shared_ptr<ASTNode> thenBranch;
    std::shared_ptr<ASTNode> elseBranch = nullptr;

    void print(std::ostream& os, int indent = 0) const override;
};

class WhileNode : public ASTNode {
public:
    std::shared_ptr<ASTNode> cond;
    std::shared_ptr<ASTNode> body;

    void print(std::ostream& os, int indent = 0) const override;
};

class ForNode : public ASTNode {
public:
    std::string varName;
    std::shared_ptr<ASTNode> startExpr;
    std::shared_ptr<ASTNode> endExpr;
    std::shared_ptr<ASTNode> body;

    void print(std::ostream& os, int indent = 0) const override;
};

class WriteNode : public ASTNode {
public:
    bool isWriteln;
    std::vector<std::shared_ptr<ASTNode>> args;

    void print(std::ostream& os, int indent = 0) const override;
};

class ReadNode : public ASTNode {
public:
    bool isReadln;
    std::vector<std::shared_ptr<ASTNode>> vars; // variable nodes (could be indexed)

    void print(std::ostream& os, int indent = 0) const override;
};

class CompoundStmtNode : public ASTNode {
public:
    std::vector<std::shared_ptr<ASTNode>> statements;

    void print(std::ostream& os, int indent = 0) const override;
};

class BinaryOpNode : public ASTNode {
public:
    std::string op;
    std::shared_ptr<ASTNode> left;
    std::shared_ptr<ASTNode> right;

    void print(std::ostream& os, int indent = 0) const override;
};

class UnaryOpNode : public ASTNode {
public:
    std::string op;
    std::shared_ptr<ASTNode> expr;

    void print(std::ostream& os, int indent = 0) const override;
};

class LiteralNode : public ASTNode {
public:
    std::string val;
    std::string type; // "integer", "real", "boolean", "string"

    void print(std::ostream& os, int indent = 0) const override;
};

class IdentifierNode : public ASTNode {
public:
    std::string name;
    std::shared_ptr<ASTNode> arrayIndex = nullptr;

    void print(std::ostream& os, int indent = 0) const override;
};

#endif
