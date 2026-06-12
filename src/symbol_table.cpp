#include "symbol_table.h"
#include <iomanip>

SymbolTableManager::SymbolTableManager() : currentLevel(0) {
    currentScope = std::make_shared<Scope>(nullptr, 0, "global");
    allScopes.push_back(currentScope);
}

void SymbolTableManager::enterScope(const std::string& ownerName) {
    currentLevel++;
    currentScope = std::make_shared<Scope>(currentScope, currentLevel, ownerName);
    allScopes.push_back(currentScope);
}

void SymbolTableManager::exitScope() {
    if (currentScope->getParent()) {
        currentScope = currentScope->getParent();
        currentLevel--;
    }
}

bool SymbolTableManager::insert(const Symbol& sym) {
    return currentScope->insert(sym);
}

bool SymbolTableManager::remove(const std::string& name) {
    return currentScope->remove(name);
}

Symbol* SymbolTableManager::lookup(const std::string& name) {
    return currentScope->lookup(name);
}

Symbol* SymbolTableManager::lookupLocal(const std::string& name) {
    return currentScope->lookupLocal(name);
}

void SymbolTableManager::print(std::ostream& os) const {
    os << "========================================= SYMBOL TABLE HISTORY =========================================\n";
    for (const auto& sc : allScopes) {
        os << "Scope Name: \"" << sc->getOwnerName() << "\" | Scope Level: " << sc->getLevel() << "\n";
        os << "--------------------------------------------------------------------------------------------------------\n";
        os << std::left << std::setw(15) << "Name"
           << std::setw(12) << "Kind"
           << std::setw(15) << "Type"
           << std::setw(10) << "Line"
           << std::setw(25) << "Extra Info" << "\n";
        os << "--------------------------------------------------------------------------------------------------------\n";
        if (sc->getTable().empty()) {
            os << " (Empty Scope)\n";
        } else {
            for (const auto& pair : sc->getTable()) {
                const auto& sym = pair.second;
                os << std::left << std::setw(15) << sym.name
                   << std::setw(12) << sym.kind
                   << std::setw(15) << sym.type
                   << std::setw(10) << sym.lineNum;
                
                if (sym.kind == "array") {
                    os << "Bounds: [" << sym.arrayLower << ".." << sym.arrayUpper << "] of " << sym.arrayElemType;
                } else if (sym.kind == "procedure" || sym.kind == "function") {
                    os << "Params: (";
                    for (size_t i = 0; i < sym.parameters.size(); ++i) {
                        if (i > 0) os << ", ";
                        os << sym.parameters[i].first << ":" << sym.parameters[i].second;
                    }
                    os << ")";
                    if (sym.kind == "function") {
                        os << " Return Type: " << sym.returnType;
                    }
                }
                os << "\n";
            }
        }
        os << "========================================================================================================\n\n";
    }
}
