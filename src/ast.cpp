#include "ast.h"

static void printIndent(std::ostream& os, int indent) {
    for (int i = 0; i < indent; ++i) {
        os << "  ";
    }
}

void ProgramNode::print(std::ostream& os, int indent) const {
    printIndent(os, indent);
    os << "Program: " << name << "\n";
    if (!declarations.empty()) {
        printIndent(os, indent + 1);
        os << "Declarations:\n";
        for (const auto& decl : declarations) {
            decl->print(os, indent + 2);
        }
    }
    if (compoundStmt) {
        printIndent(os, indent + 1);
        os << "Body:\n";
        compoundStmt->print(os, indent + 2);
    }
}

void VarDeclNode::print(std::ostream& os, int indent) const {
    printIndent(os, indent);
    os << "VarDecl: ";
    for (size_t i = 0; i < names.size(); ++i) {
        if (i > 0) os << ", ";
        os << names[i];
    }
    os << " : " << type << "\n";
}

void ConstDeclNode::print(std::ostream& os, int indent) const {
    printIndent(os, indent);
    os << "ConstDecl: " << name << " = " << val << "\n";
}

void ProcDeclNode::print(std::ostream& os, int indent) const {
    printIndent(os, indent);
    os << (isFunction ? "FunctionDecl: " : "ProcedureDecl: ") << name << "\n";
    if (!params.empty()) {
        printIndent(os, indent + 1);
        os << "Parameters:\n";
        for (const auto& p : params) {
            printIndent(os, indent + 2);
            os << p.first << " : " << p.second << "\n";
        }
    }
    if (isFunction) {
        printIndent(os, indent + 1);
        os << "ReturnType: " << returnType << "\n";
    }
    if (!declarations.empty()) {
        printIndent(os, indent + 1);
        os << "Local Declarations:\n";
        for (const auto& decl : declarations) {
            decl->print(os, indent + 2);
        }
    }
    if (body) {
        printIndent(os, indent + 1);
        os << "Body:\n";
        body->print(os, indent + 2);
    }
}

void AssignNode::print(std::ostream& os, int indent) const {
    printIndent(os, indent);
    os << "Assign: " << name << "\n";
    if (arrayIndex) {
        printIndent(os, indent + 1);
        os << "Index:\n";
        arrayIndex->print(os, indent + 2);
    }
    printIndent(os, indent + 1);
    os << "Expr:\n";
    expr->print(os, indent + 2);
}

void CallNode::print(std::ostream& os, int indent) const {
    printIndent(os, indent);
    os << "Call: " << name << "\n";
    if (!args.empty()) {
        printIndent(os, indent + 1);
        os << "Arguments:\n";
        for (const auto& arg : args) {
            arg->print(os, indent + 2);
        }
    }
}

void IfNode::print(std::ostream& os, int indent) const {
    printIndent(os, indent);
    os << "If:\n";
    printIndent(os, indent + 1);
    os << "Cond:\n";
    cond->print(os, indent + 2);
    printIndent(os, indent + 1);
    os << "Then:\n";
    thenBranch->print(os, indent + 2);
    if (elseBranch) {
        printIndent(os, indent + 1);
        os << "Else:\n";
        elseBranch->print(os, indent + 2);
    }
}

void WhileNode::print(std::ostream& os, int indent) const {
    printIndent(os, indent);
    os << "While:\n";
    printIndent(os, indent + 1);
    os << "Cond:\n";
    cond->print(os, indent + 2);
    printIndent(os, indent + 1);
    os << "Body:\n";
    body->print(os, indent + 2);
}

void ForNode::print(std::ostream& os, int indent) const {
    printIndent(os, indent);
    os << "For: " << varName << "\n";
    printIndent(os, indent + 1);
    os << "Start:\n";
    startExpr->print(os, indent + 2);
    printIndent(os, indent + 1);
    os << "End:\n";
    endExpr->print(os, indent + 2);
    printIndent(os, indent + 1);
    os << "Body:\n";
    body->print(os, indent + 2);
}

void WriteNode::print(std::ostream& os, int indent) const {
    printIndent(os, indent);
    os << (isWriteln ? "Writeln:\n" : "Write:\n");
    for (const auto& arg : args) {
        arg->print(os, indent + 1);
    }
}

void ReadNode::print(std::ostream& os, int indent) const {
    printIndent(os, indent);
    os << (isReadln ? "Readln:\n" : "Read:\n");
    for (const auto& var : vars) {
        var->print(os, indent + 1);
    }
}

void CompoundStmtNode::print(std::ostream& os, int indent) const {
    printIndent(os, indent);
    os << "CompoundStmt:\n";
    for (const auto& stmt : statements) {
        stmt->print(os, indent + 1);
    }
}

void BinaryOpNode::print(std::ostream& os, int indent) const {
    printIndent(os, indent);
    os << "BinaryOp: " << op << "\n";
    left->print(os, indent + 1);
    right->print(os, indent + 1);
}

void UnaryOpNode::print(std::ostream& os, int indent) const {
    printIndent(os, indent);
    os << "UnaryOp: " << op << "\n";
    expr->print(os, indent + 1);
}

void LiteralNode::print(std::ostream& os, int indent) const {
    printIndent(os, indent);
    os << "Literal: '" << val << "' (" << type << ")\n";
}

void IdentifierNode::print(std::ostream& os, int indent) const {
    printIndent(os, indent);
    os << "Identifier: " << name << "\n";
    if (arrayIndex) {
        printIndent(os, indent + 1);
        os << "ArrayIndex:\n";
        arrayIndex->print(os, indent + 2);
    }
}
