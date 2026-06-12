#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include <utility>
#include "lexer.h"

struct Production {
    std::string lhs;
    std::vector<std::string> rhs; // empty means epsilon
    int index;
};

enum class LRActionType {
    SHIFT, REDUCE, ACCEPT, ERROR
};

struct LRAction {
    LRActionType type;
    int target; // state index for Shift, production index for Reduce
};

struct LRItem {
    int prodIdx;
    int dotPos;

    bool operator<(const LRItem& other) const {
        if (prodIdx != other.prodIdx) return prodIdx < other.prodIdx;
        return dotPos < other.dotPos;
    }

    bool operator==(const LRItem& other) const {
        return prodIdx == other.prodIdx && dotPos == other.dotPos;
    }
};

typedef std::set<LRItem> LRState;

class Grammar {
public:
    Grammar();
    
    void initialize();
    void computeFirstAndFollow();
    void buildLLTable();
    void buildSLRTable();

    // Accessors
    const std::vector<Production>& getProductions() const { return productions; }
    const std::set<std::string>& getNonTerminals() const { return nonTerminals; }
    const std::set<std::string>& getTerminals() const { return terminals; }
    const std::map<std::string, std::set<std::string>>& getFirstSets() const { return first; }
    const std::map<std::string, std::set<std::string>>& getFollowSets() const { return follow; }
    
    // Parser Tables
    int getLLAction(const std::string& nonTerminal, const std::string& terminal) const;
    LRAction getLRAction(int state, const std::string& terminal) const;
    int getLRGoto(int state, const std::string& nonTerminal) const;

    // Helpers
    std::string mapTokenToTerminal(const Token& token) const;
    bool isTerminal(const std::string& sym) const;
    bool isNonTerminal(const std::string& sym) const;
    
    // Diagnostics & Printing
    void printFirstAndFollow(std::ostream& os) const;
    void printLLTable(std::ostream& os) const;
    void printSLRTable(std::ostream& os) const;
    void saveTablesToMarkdown(const std::string& filepath) const;

private:
    std::vector<Production> productions;
    std::set<std::string> nonTerminals;
    std::set<std::string> terminals;
    std::string startSymbol;

    std::map<std::string, std::set<std::string>> first;
    std::map<std::string, std::set<std::string>> follow;

    // LL(1) predictive parsing table: [NonTerminal][Terminal] -> production index
    std::map<std::pair<std::string, std::string>, int> llTable;

    // SLR(1) tables
    std::vector<LRState> lrStates;
    std::map<std::pair<int, std::string>, LRAction> actionTable;
    std::map<std::pair<int, std::string>, int> gotoTable;

    void addProduction(const std::string& lhs, const std::vector<std::string>& rhs);
    
    // First/Follow helper methods
    bool computeFirstOfSequence(const std::vector<std::string>& seq, std::set<std::string>& result) const;
    
    // SLR(1) helper methods
    LRState computeClosure(const LRState& itemSet) const;
    LRState computeGoto(const LRState& itemSet, const std::string& symbol) const;
};

#endif
