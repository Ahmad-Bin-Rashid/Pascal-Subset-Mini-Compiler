#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <algorithm>
#include <iomanip>
#include "lexer.h"
#include "grammar.h"
#include "symbol_table.h"
#include "error_handler.h"
#include "parser_rd.h"
#include "parser_ll.h"
#include "parser_lr.h"

// Helper to get file base name
std::string getBaseName(const std::string& path) {
    size_t lastSlash = path.find_last_of("/\\");
    std::string name = (lastSlash == std::string::npos) ? path : path.substr(lastSlash + 1);
    size_t lastDot = name.find_last_of(".");
    return (lastDot == std::string::npos) ? name : name.substr(0, lastDot);
}

void printBanner() {
    std::cout << "======================================================================\n";
    std::cout << "               PASCAL SUBSET MINI COMPILER DRIVER                     \n";
    std::cout << "======================================================================\n";
    std::cout << " Group Members:\n";
    std::cout << "  -> Abubakar Munir (2023-CS-60) [Group Leader]\n";
    std::cout << "  -> Ahmad Bin Rashid (2023-CS-52)\n";
    std::cout << "  -> Muhammad Omer (2023-CS-68)\n";
    std::cout << "======================================================================\n\n";
}

int main(int argc, char* argv[]) {
    printBanner();

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file.pas> [output_directory]\n";
        return 1;
    }

    std::string sourcePath = argv[1];
    std::string outputDir = "output";
    if (argc >= 3) {
        outputDir = argv[2];
    }

    std::string baseName = getBaseName(sourcePath);

    // 1. Initialize Grammar Engine
    std::cout << "[Step 1] Initializing Grammar Engine...\n";
    Grammar grammar;
    grammar.initialize();
    grammar.computeFirstAndFollow();
    grammar.buildLLTable();
    grammar.buildSLRTable();

    // Auto-save tables to docs/
    std::string tablesPath = "docs/parser_tables.md";
    grammar.saveTablesToMarkdown(tablesPath);
    std::cout << " -> Parsing tables and grammar analysis saved to: " << tablesPath << "\n\n";

    // 2. Token Stream Generation
    std::cout << "[Step 2] Running Lexical Analyzer...\n";
    Lexer lexer(sourcePath);
    std::string tokenOutPath = outputDir + "/" + baseName + "_tokens.txt";
    std::ofstream tokenFile(tokenOutPath);
    if (!tokenFile.is_open()) {
        std::cerr << "Error: Could not open output file: " << tokenOutPath << "\n";
        return 1;
    }

    tokenFile << "Token stream for: " << sourcePath << "\n";
    tokenFile << "--------------------------------------------------------------------------------\n";
    tokenFile << std::left << std::setw(20) << "Token Type" 
              << " | " << std::setw(20) << "Lexeme" 
              << " | Line | Col\n";
    tokenFile << "--------------------------------------------------------------------------------\n";

    while (true) {
        Token tok = lexer.getNextToken();
        tokenFile << std::left << std::setw(20) << tokenTypeToString(tok.type)
                  << " | " << std::setw(20) << tok.lexeme
                  << " | " << std::setw(4) << tok.line
                  << " | " << tok.col << "\n";
        if (tok.type == TokenType::END_OF_FILE) break;
    }
    tokenFile.close();
    std::cout << " -> Token stream written to: " << tokenOutPath << "\n\n";

    // 3. Recursive Descent Parser & Semantic Analysis
    std::cout << "[Step 3] Running Recursive Descent Parser & Semantic Analyzer...\n";
    lexer.reset();
    SymbolTableManager symTableRD;
    ErrorHandler errHandlerRD;
    ParserRD parserRD(lexer, symTableRD, errHandlerRD);

    auto astRoot = parserRD.parse();

    std::string rdErrOutPath = outputDir + "/" + baseName + "_errors_rd.txt";
    std::ofstream rdErrFile(rdErrOutPath);
    errHandlerRD.printSummary(rdErrFile);
    rdErrFile.close();

    std::cout << " -> Recursive Descent Results:\n";
    if (errHandlerRD.hasErrors()) {
        std::cout << "    [FAILED] Status: FAILED (Check errors in " << rdErrOutPath << ")\n";
    } else {
        std::cout << "    [SUCCESS] Status: SUCCESSFUL\n";
        // Write AST
        std::string astOutPath = outputDir + "/" + baseName + "_ast.txt";
        std::ofstream astFile(astOutPath);
        if (astRoot) {
            astRoot->print(astFile);
        }
        astFile.close();
        std::cout << "    -> Abstract Syntax Tree written to: " << astOutPath << "\n";
    }

    // Write symbol table history
    std::string symOutPath = outputDir + "/" + baseName + "_symtable.txt";
    std::ofstream symFile(symOutPath);
    symTableRD.print(symFile);
    symFile.close();
    std::cout << "    -> Symbol Table history written to: " << symOutPath << "\n\n";

    // 4. LL(1) Non-Recursive Predictive Parser
    std::cout << "[Step 4] Running LL(1) Predictive Stack Parser...\n";
    lexer.reset();
    ErrorHandler errHandlerLL;
    ParserLL parserLL(lexer, grammar, errHandlerLL);
    
    bool llResult = parserLL.parse();
    
    std::string llErrOutPath = outputDir + "/" + baseName + "_errors_ll.txt";
    std::ofstream llErrFile(llErrOutPath);
    errHandlerLL.printSummary(llErrFile);
    llErrFile.close();

    std::cout << " -> LL(1) Parser Results:\n";
    if (!llResult || errHandlerLL.hasErrors()) {
        std::cout << "    [FAILED] Status: FAILED (Check errors in " << llErrOutPath << ")\n\n";
    } else {
        std::cout << "    [SUCCESS] Status: SUCCESSFUL\n\n";
    }

    // 5. SLR(1) LR Parser
    std::cout << "[Step 5] Running SLR(1) LR Shift-Reduce Parser...\n";
    lexer.reset();
    ErrorHandler errHandlerLR;
    ParserLR parserLR(lexer, grammar, errHandlerLR);

    std::string lrTracePath = outputDir + "/" + baseName + "_lr_trace.txt";
    std::ofstream lrTraceFile(lrTracePath);
    bool lrResult = parserLR.parse(lrTraceFile);
    lrTraceFile.close();

    std::string lrErrOutPath = outputDir + "/" + baseName + "_errors_lr.txt";
    std::ofstream lrErrFile(lrErrOutPath);
    errHandlerLR.printSummary(lrErrFile);
    lrErrFile.close();

    std::cout << " -> SLR(1) LR Parser Results:\n";
    if (!lrResult || errHandlerLR.hasErrors()) {
        std::cout << "    [FAILED] Status: FAILED (Check trace in " << lrTracePath << ")\n";
    } else {
        std::cout << "    [SUCCESS] Status: SUCCESSFUL\n";
    }
    std::cout << "    -> Shift-Reduce trace written to: " << lrTracePath << "\n";
    std::cout << "======================================================================\n\n";

    return 0;
}
