#include "lexer.h"
#include <algorithm>
#include <cctype>

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::PROGRAM: return "PROGRAM";
        case TokenType::CONST: return "CONST";
        case TokenType::VAR: return "VAR";
        case TokenType::PROCEDURE: return "PROCEDURE";
        case TokenType::FUNCTION: return "FUNCTION";
        case TokenType::BEGIN: return "BEGIN";
        case TokenType::END: return "END";
        case TokenType::INTEGER: return "INTEGER";
        case TokenType::REAL: return "REAL";
        case TokenType::BOOLEAN: return "BOOLEAN";
        case TokenType::CHAR: return "CHAR";
        case TokenType::ARRAY: return "ARRAY";
        case TokenType::OF: return "OF";
        case TokenType::IF: return "IF";
        case TokenType::THEN: return "THEN";
        case TokenType::ELSE: return "ELSE";
        case TokenType::WHILE: return "WHILE";
        case TokenType::DO: return "DO";
        case TokenType::FOR: return "FOR";
        case TokenType::TO: return "TO";
        case TokenType::WRITELN: return "WRITELN";
        case TokenType::WRITE: return "WRITE";
        case TokenType::READLN: return "READLN";
        case TokenType::READ: return "READ";
        case TokenType::DIV: return "DIV";
        case TokenType::MOD: return "MOD";
        case TokenType::AND: return "AND";
        case TokenType::OR: return "OR";
        case TokenType::NOT: return "NOT";
        case TokenType::TRUE: return "TRUE";
        case TokenType::FALSE: return "FALSE";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::NUM_INT: return "NUM_INT";
        case TokenType::NUM_REAL: return "NUM_REAL";
        case TokenType::STRING_LIT: return "STRING_LIT";
        case TokenType::CHAR_LIT: return "CHAR_LIT";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::STAR: return "STAR";
        case TokenType::SLASH: return "SLASH";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::NEQ: return "NEQ";
        case TokenType::LESS: return "LESS";
        case TokenType::LEQ: return "LEQ";
        case TokenType::GREATER: return "GREATER";
        case TokenType::GEQ: return "GEQ";
        case TokenType::ASSIGN: return "ASSIGN";
        case TokenType::COLON: return "COLON";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::LBRACKET: return "LBRACKET";
        case TokenType::RBRACKET: return "RBRACKET";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::RANGE: return "RANGE";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

Lexer::Lexer(const std::string& filepath)
    : filepath(filepath), bytesInBuffer1(0), bytesInBuffer2(0),
      currentBufferIdx(1), cursor(0), lineNum(1), colNum(1),
      tokenLine(1), tokenCol(1), errorOccurred(false) {
    file.open(filepath, std::ios::binary);
    if (!file.is_open()) {
        reportError("Failed to open file: " + filepath);
    }
    loadBuffer(1);
}

Lexer::~Lexer() {
    if (file.is_open()) {
        file.close();
    }
}

void Lexer::reset() {
    if (file.is_open()) {
        file.close();
    }
    file.open(filepath, std::ios::binary);
    bytesInBuffer1 = 0;
    bytesInBuffer2 = 0;
    currentBufferIdx = 1;
    cursor = 0;
    lineNum = 1;
    colNum = 1;
    errorOccurred = false;
    errors.clear();
    loadBuffer(1);
}

void Lexer::loadBuffer(int bufferNum) {
    char* target = (bufferNum == 1) ? buffer1 : buffer2;
    size_t& bytes = (bufferNum == 1) ? bytesInBuffer1 : bytesInBuffer2;
    if (file.eof()) {
        bytes = 0;
        return;
    }
    file.read(target, BUFFER_SIZE);
    bytes = file.gcount();
}

char Lexer::getNextChar() {
    char* currentBuf = (currentBufferIdx == 1) ? buffer1 : buffer2;
    size_t currentBytes = (currentBufferIdx == 1) ? bytesInBuffer1 : bytesInBuffer2;

    if (cursor >= currentBytes) {
        if (currentBufferIdx == 1) {
            currentBufferIdx = 2;
            loadBuffer(2);
            currentBuf = buffer2;
            currentBytes = bytesInBuffer2;
        } else {
            currentBufferIdx = 1;
            loadBuffer(1);
            currentBuf = buffer1;
            currentBytes = bytesInBuffer1;
        }
        cursor = 0;
        if (currentBytes == 0) {
            return '\0';
        }
    }

    char c = currentBuf[cursor++];
    if (c == '\n') {
        lineNum++;
        colNum = 1;
    } else {
        colNum++;
    }
    return c;
}

void Lexer::retract() {
    if (cursor > 0) {
        cursor--;
    } else {
        if (currentBufferIdx == 1) {
            currentBufferIdx = 2;
            cursor = bytesInBuffer2 - 1;
        } else {
            currentBufferIdx = 1;
            cursor = bytesInBuffer1 - 1;
        }
    }
    char* currentBuf = (currentBufferIdx == 1) ? buffer1 : buffer2;
    char c = currentBuf[cursor];
    if (c == '\n') {
        lineNum--;
        // colNum will be corrected by next getNextChar() call
        colNum = 1;
    } else {
        colNum--;
    }
}

void Lexer::reportError(const std::string& message) {
    errorOccurred = true;
    errors.push_back("Lexical Error [Line " + std::to_string(lineNum) + ", Col " + std::to_string(colNum) + "]: " + message);
}

TokenType Lexer::checkKeyword(const std::string& lexeme) {
    std::string lower = lexeme;
    std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) {
        return std::tolower(c);
    });

    if (lower == "program") return TokenType::PROGRAM;
    if (lower == "const") return TokenType::CONST;
    if (lower == "var") return TokenType::VAR;
    if (lower == "procedure") return TokenType::PROCEDURE;
    if (lower == "function") return TokenType::FUNCTION;
    if (lower == "begin") return TokenType::BEGIN;
    if (lower == "end") return TokenType::END;
    if (lower == "integer") return TokenType::INTEGER;
    if (lower == "real") return TokenType::REAL;
    if (lower == "boolean") return TokenType::BOOLEAN;
    if (lower == "char") return TokenType::CHAR;
    if (lower == "array") return TokenType::ARRAY;
    if (lower == "of") return TokenType::OF;
    if (lower == "if") return TokenType::IF;
    if (lower == "then") return TokenType::THEN;
    if (lower == "else") return TokenType::ELSE;
    if (lower == "while") return TokenType::WHILE;
    if (lower == "do") return TokenType::DO;
    if (lower == "for") return TokenType::FOR;
    if (lower == "to") return TokenType::TO;
    if (lower == "writeln") return TokenType::WRITELN;
    if (lower == "write") return TokenType::WRITE;
    if (lower == "readln") return TokenType::READLN;
    if (lower == "read") return TokenType::READ;
    if (lower == "div") return TokenType::DIV;
    if (lower == "mod") return TokenType::MOD;
    if (lower == "and") return TokenType::AND;
    if (lower == "or") return TokenType::OR;
    if (lower == "not") return TokenType::NOT;
    if (lower == "true") return TokenType::TRUE;
    if (lower == "false") return TokenType::FALSE;

    return TokenType::IDENTIFIER;
}

Token Lexer::getNextToken() {
    while (true) {
        char c = getNextChar();
        if (c == '\0') {
            return {TokenType::END_OF_FILE, "", lineNum, colNum};
        }

        if (std::isspace(c)) {
            continue;
        }

        if (c == '{') {
            while (true) {
                char next = getNextChar();
                if (next == '\0') {
                    reportError("Unterminated comment starting with '{'");
                    return {TokenType::ERROR, "{", lineNum, colNum};
                }
                if (next == '}') {
                    break;
                }
            }
            continue;
        }

        if (c == '(') {
            char next = getNextChar();
            if (next == '*') {
                while (true) {
                    char n1 = getNextChar();
                    if (n1 == '\0') {
                        reportError("Unterminated comment starting with '(*'");
                        return {TokenType::ERROR, "(*", lineNum, colNum};
                    }
                    if (n1 == '*') {
                        char n2 = getNextChar();
                        if (n2 == ')') {
                            break;
                        }
                        retract();
                    }
                }
                continue;
            } else {
                retract();
                tokenLine = lineNum;
                tokenCol = colNum - 1;
                return {TokenType::LPAREN, "(", tokenLine, tokenCol};
            }
        }

        tokenLine = lineNum;
        tokenCol = colNum - 1;

        if (std::isalpha(c) || c == '_') {
            std::string lexeme;
            lexeme += c;
            while (true) {
                char next = getNextChar();
                if (std::isalnum(next) || next == '_') {
                    lexeme += next;
                } else {
                    retract();
                    break;
                }
            }
            TokenType type = checkKeyword(lexeme);
            return {type, lexeme, tokenLine, tokenCol};
        }

        if (std::isdigit(c)) {
            std::string lexeme;
            lexeme += c;
            bool isReal = false;
            while (true) {
                char next = getNextChar();
                if (std::isdigit(next)) {
                    lexeme += next;
                } else if (next == '.') {
                    char lookAhead = getNextChar();
                    if (lookAhead == '.') {
                        retract();
                        retract();
                        break;
                    } else {
                        retract();
                        if (isReal) {
                            reportError("Invalid number format: multiple decimal points");
                            return {TokenType::ERROR, lexeme + ".", lineNum, colNum};
                        }
                        isReal = true;
                        lexeme += next;
                        char digitAfter = getNextChar();
                        if (std::isdigit(digitAfter)) {
                            lexeme += digitAfter;
                        } else {
                            reportError("Invalid number format: digit expected after decimal point");
                            return {TokenType::ERROR, lexeme, lineNum, colNum};
                        }
                    }
                } else {
                    retract();
                    break;
                }
            }
            return {isReal ? TokenType::NUM_REAL : TokenType::NUM_INT, lexeme, tokenLine, tokenCol};
        }

        if (c == '\'') {
            std::string lexeme;
            while (true) {
                char next = getNextChar();
                if (next == '\0' || next == '\n') {
                    reportError("Unterminated string literal");
                    return {TokenType::ERROR, lexeme, tokenLine, tokenCol};
                }
                if (next == '\'') {
                    char check = getNextChar();
                    if (check == '\'') {
                        lexeme += '\'';
                    } else {
                        retract();
                        break;
                    }
                } else {
                    lexeme += next;
                }
            }
            if (lexeme.length() == 1) {
                return {TokenType::CHAR_LIT, lexeme, tokenLine, tokenCol};
            }
            return {TokenType::STRING_LIT, lexeme, tokenLine, tokenCol};
        }

        switch (c) {
            case '+': return {TokenType::PLUS, "+", tokenLine, tokenCol};
            case '-': return {TokenType::MINUS, "-", tokenLine, tokenCol};
            case '*': return {TokenType::STAR, "*", tokenLine, tokenCol};
            case '/': return {TokenType::SLASH, "/", tokenLine, tokenCol};
            case '=': return {TokenType::EQUAL, "=", tokenLine, tokenCol};
            case ';': return {TokenType::SEMICOLON, ";", tokenLine, tokenCol};
            case ',': return {TokenType::COMMA, ",", tokenLine, tokenCol};
            case '[': return {TokenType::LBRACKET, "[", tokenLine, tokenCol};
            case ']': return {TokenType::RBRACKET, "]", tokenLine, tokenCol};
            case ')': return {TokenType::RPAREN, ")", tokenLine, tokenCol};
            case ':': {
                char next = getNextChar();
                if (next == '=') {
                    return {TokenType::ASSIGN, ":=", tokenLine, tokenCol};
                } else {
                    retract();
                    return {TokenType::COLON, ":", tokenLine, tokenCol};
                }
            }
            case '.': {
                char next = getNextChar();
                if (next == '.') {
                    return {TokenType::RANGE, "..", tokenLine, tokenCol};
                } else {
                    retract();
                    return {TokenType::DOT, ".", tokenLine, tokenCol};
                }
            }
            case '<': {
                char next = getNextChar();
                if (next == '=') {
                    return {TokenType::LEQ, "<=", tokenLine, tokenCol};
                } else if (next == '>') {
                    return {TokenType::NEQ, "<>", tokenLine, tokenCol};
                } else {
                    retract();
                    return {TokenType::LESS, "<", tokenLine, tokenCol};
                }
            }
            case '>': {
                char next = getNextChar();
                if (next == '=') {
                    return {TokenType::GEQ, ">=", tokenLine, tokenCol};
                } else {
                    retract();
                    return {TokenType::GREATER, ">", tokenLine, tokenCol};
                }
            }
            default: {
                reportError(std::string("Unexpected character: '") + c + "'");
                return {TokenType::ERROR, std::string(1, c), tokenLine, tokenCol};
            }
        }
    }
}
