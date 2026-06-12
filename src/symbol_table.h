#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <iostream>

struct Symbol {
    std::string name;
    std::string kind; // "variable", "constant", "procedure", "function", "array"
    std::string type; // "integer", "real", "boolean", "char"
    int scopeLevel;
    int lineNum;
    
    int arrayLower = 0;
    int arrayUpper = 0;
    std::string arrayElemType = "";

    std::vector<std::pair<std::string, std::string>> parameters; // name, type
    std::string returnType = ""; // for function
};

class Scope {
public:
    explicit Scope(std::shared_ptr<Scope> parent, int level, const std::string& ownerName = "global")
        : parent(parent), level(level), ownerName(ownerName) {}

    bool insert(const Symbol& sym) {
        if (table.find(sym.name) != table.end()) {
            return false;
        }
        table[sym.name] = sym;
        return true;
    }

    bool remove(const std::string& name) {
        auto it = table.find(name);
        if (it != table.end()) {
            table.erase(it);
            return true;
        }
        return false;
    }

    Symbol* lookup(const std::string& name) {
        auto it = table.find(name);
        if (it != table.end()) {
            return &(it->second);
        }
        if (parent) {
            return parent->lookup(name);
        }
        return nullptr;
    }

    Symbol* lookupLocal(const std::string& name) {
        auto it = table.find(name);
        if (it != table.end()) {
            return &(it->second);
        }
        return nullptr;
    }

    int getLevel() const { return level; }
    std::shared_ptr<Scope> getParent() const { return parent; }
    const std::unordered_map<std::string, Symbol>& getTable() const { return table; }
    std::string getOwnerName() const { return ownerName; }

private:
    std::shared_ptr<Scope> parent;
    int level;
    std::string ownerName;
    std::unordered_map<std::string, Symbol> table;
};

class SymbolTableManager {
public:
    SymbolTableManager();
    
    void enterScope(const std::string& ownerName = "local");
    void exitScope();
    
    bool insert(const Symbol& sym);
    bool remove(const std::string& name);
    Symbol* lookup(const std::string& name);
    Symbol* lookupLocal(const std::string& name);

    int getCurrentLevel() const { return currentLevel; }
    std::shared_ptr<Scope> getCurrentScope() const { return currentScope; }
    const std::vector<std::shared_ptr<Scope>>& getAllScopes() const { return allScopes; }

    void print(std::ostream& os) const;

private:
    std::shared_ptr<Scope> currentScope;
    int currentLevel;
    std::vector<std::shared_ptr<Scope>> allScopes;
};

#endif
