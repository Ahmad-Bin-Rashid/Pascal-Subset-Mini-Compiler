#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <fstream>

enum class TokenType {
    // Keywords
    PROGRAM, CONST, VAR, PROCEDURE, FUNCTION, BEGIN, END,
    INTEGER, REAL, BOOLEAN, CHAR, ARRAY, OF,
    IF, THEN, ELSE, WHILE, DO, FOR, TO,
    WRITELN, WRITE, READLN, READ,
    DIV, MOD, AND, OR, NOT,
    TRUE, FALSE,

    // Literals and Identifiers
    IDENTIFIER, NUM_INT, NUM_REAL, STRING_LIT, CHAR_LIT,

    // Operators and Punctuation
    PLUS, MINUS, STAR, SLASH, EQUAL, NEQ, LESS, LEQ, GREATER, GEQ,
    ASSIGN, COLON, SEMICOLON, COMMA, DOT, LBRACKET, RBRACKET, LPAREN, RPAREN, RANGE,

    // Special
    END_OF_FILE, ERROR
};

std::string tokenTypeToString(TokenType type);

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int col;
};

class Lexer {
public:
    explicit Lexer(const std::string& filepath);
    ~Lexer();

    Token getNextToken();
    void reset();

    bool hasError() const { return errorOccurred; }
    const std::vector<std::string>& getErrors() const { return errors; }

private:
    std::ifstream file;
    std::string filepath;

    static const size_t BUFFER_SIZE = 4096;
    char buffer1[BUFFER_SIZE];
    char buffer2[BUFFER_SIZE];
    
    size_t bytesInBuffer1;
    size_t bytesInBuffer2;
    size_t currentBufferIdx;
    size_t cursor;

    int lineNum;
    int colNum;
    
    int tokenLine;
    int tokenCol;

    bool errorOccurred;
    std::vector<std::string> errors;

    void loadBuffer(int bufferNum);
    char getNextChar();
    void retract();
    void reportError(const std::string& message);

    TokenType checkKeyword(const std::string& lexeme);
};

#endif
