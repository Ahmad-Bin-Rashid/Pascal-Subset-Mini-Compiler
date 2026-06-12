#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <string>
#include <vector>
#include <iostream>

struct CompilerError {
    std::string type; // "Lexical", "Syntactic", "Semantic"
    int line;
    int col;
    std::string message;
};

class ErrorHandler {
public:
    ErrorHandler() = default;

    void report(const std::string& type, int line, int col, const std::string& msg) {
        errors.push_back({type, line, col, msg});
    }

    void clear() {
        errors.clear();
    }

    bool hasErrors() const {
        return !errors.empty();
    }

    const std::vector<CompilerError>& getErrors() const {
        return errors;
    }

    void printSummary(std::ostream& os) const {
        if (errors.empty()) {
            os << "Compilation successful. No errors detected.\n";
            return;
        }
        os << "========================================= COMPILATION ERROR LOG =========================================\n";
        for (const auto& err : errors) {
            os << err.type << " Error [Line " << err.line << ", Col " << err.col << "]: " << err.message << "\n";
        }
        os << "--------------------------------------------------------------------------------------------------------\n";
        os << "Total Errors: " << errors.size() << "\n";
        os << "========================================================================================================\n";
    }

private:
    std::vector<CompilerError> errors;
};

#endif
