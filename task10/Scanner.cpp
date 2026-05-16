// Scanner.cpp
#include "Scanner.h"
#include <cctype>
#include <stdexcept>

Scanner::Scanner(std::istream &input) : in(input), line(1), col(0) {
    nextChar();
}

void Scanner::nextChar() {
    if (in.get(c)) {
        if (c == '\n') { ++line; col = 0; }
        else ++col;
    } else {
        c = EOF;
    }
}

void Scanner::skipSpacesAndComments() {
    while (c != EOF) {
        if (isspace(c)) { nextChar(); continue; }
        if (c == '/') {
            nextChar();
            if (c == '*') {
                nextChar();
                while (c != EOF) {
                    if (c == '*') {
                        nextChar();
                        if (c == '/') { nextChar(); break; }
                    } else nextChar();
                }
                if (c == EOF) throw std::runtime_error("Unterminated comment");
                continue;
            } else {
                break;
            }
        }
        break;
    }
}

Lex Scanner::getIdent() {
    std::string buf;
    int start_col = col;
    while (isalnum(c)) {
        buf += c;
        nextChar();
    }
    if (buf == "program") return Lex(LEX_PROGRAM, buf, 0, line, start_col);
    if (buf == "int") return Lex(LEX_INT, buf, 0, line, start_col);
    if (buf == "string") return Lex(LEX_STRING, buf, 0, line, start_col);
    if (buf == "bool") return Lex(LEX_BOOL, buf, 0, line, start_col);
    if (buf == "if") return Lex(LEX_IF, buf, 0, line, start_col);
    if (buf == "else") return Lex(LEX_ELSE, buf, 0, line, start_col);
    if (buf == "while") return Lex(LEX_WHILE, buf, 0, line, start_col);
    if (buf == "do") return Lex(LEX_DO, buf, 0, line, start_col);
    if (buf == "read") return Lex(LEX_READ, buf, 0, line, start_col);
    if (buf == "write") return Lex(LEX_WRITE, buf, 0, line, start_col);
    if (buf == "break") return Lex(LEX_BREAK, buf, 0, line, start_col);
    if (buf == "continue") return Lex(LEX_CONTINUE, buf, 0, line, start_col);
    if (buf == "true") return Lex(LEX_TRUE, buf, 0, line, start_col);
    if (buf == "false") return Lex(LEX_FALSE, buf, 0, line, start_col);
    if (buf == "not") return Lex(LEX_NOT, buf, 0, line, start_col);
    if (buf == "and") return Lex(LEX_AND, buf, 0, line, start_col);
    if (buf == "or") return Lex(LEX_OR, buf, 0, line, start_col);
    return Lex(LEX_ID, buf, 0, line, start_col);
}

Lex Scanner::getNumber() {
    std::string buf;
    int start_col = col;
    while (isdigit(c)) {
        buf += c;
        nextChar();
    }
    return Lex(LEX_NUM, "", std::stoi(buf), line, start_col);
}

Lex Scanner::getString() {
    nextChar();
    std::string buf;
    int start_col = col;
    while (c != '"') {
        if (c == EOF) throw std::runtime_error("Unterminated string constant");
        buf += c;
        nextChar();
    }
    nextChar();
    return Lex(LEX_STR, buf, 0, line, start_col);
}

Lex Scanner::getLex() {
    skipSpacesAndComments();
    if (c == EOF) {
        return Lex(LEX_FIN, "", 0, line, col);
    }
    
    if (isalpha(c)) {
        return getIdent();
    }
    if (isdigit(c)) {
        return getNumber();
    }
    if (c == '"')  {
        return getString();
    }

    char cur = c;
    nextChar();
    switch(cur) {
        case '+': return Lex(LEX_PLUS, "", 0, line, col-1);
        case '-': return Lex(LEX_MINUS, "", 0, line, col-1);
        case '*': return Lex(LEX_STAR, "", 0, line, col-1);
        case '/': return Lex(LEX_SLASH, "", 0, line, col-1);
        case '=':
            if (c == '=') { nextChar(); return Lex(LEX_EQ, "", 0, line, col-2); }
            return Lex(LEX_ASSIGN, "", 0, line, col-1);
        case '!':
            if (c == '=') { nextChar(); return Lex(LEX_NEQ, "", 0, line, col-2); }
            throw std::runtime_error("Invalid character '!'");
        case '<':
            if (c == '=') { nextChar(); return Lex(LEX_LE, "", 0, line, col-2); }
            return Lex(LEX_LT, "", 0, line, col-1);
        case '>':
            if (c == '=') { nextChar(); return Lex(LEX_GE, "", 0, line, col-2); }
            return Lex(LEX_GT, "", 0, line, col-1);
        case ';': return Lex(LEX_SEMICOLON, "", 0, line, col-1);
        case ',': return Lex(LEX_COMMA, "", 0, line, col-1);
        case ':': return Lex(LEX_COLON, "", 0, line, col-1);
        case '(': return Lex(LEX_LPAREN, "", 0, line, col-1);
        case ')': return Lex(LEX_RPAREN, "", 0, line, col-1);
        case '{': return Lex(LEX_LBRACE, "", 0, line, col-1);
        case '}': return Lex(LEX_RBRACE, "", 0, line, col-1);
        default:
            throw std::runtime_error("Unknown character '" + std::string(1, cur) + "'");
    }
}
