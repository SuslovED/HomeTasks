#pragma once
#include <string>

enum LexType {
    LEX_NULL,
    LEX_PROGRAM, LEX_INT, LEX_STRING, LEX_BOOL, LEX_IF, LEX_ELSE,
    LEX_WHILE, LEX_DO, LEX_READ, LEX_WRITE, LEX_BREAK, LEX_CONTINUE,
    LEX_TRUE, LEX_FALSE, LEX_NOT, LEX_AND, LEX_OR,
    LEX_ID, LEX_NUM, LEX_STR,
    LEX_PLUS, LEX_MINUS, LEX_STAR, LEX_SLASH,
    LEX_EQ, LEX_NEQ, LEX_LT, LEX_GT, LEX_LE, LEX_GE,
    LEX_ASSIGN,
    LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_LPAREN, LEX_RPAREN,
    LEX_LBRACE, LEX_RBRACE, LEX_FIN,
    LEX_ERROR
};

struct Lex {
    LexType type;
    std::string str;
    int num;
    int line;
    int col;

    Lex() : type(LEX_NULL), num(0), line(0), col(0) {}
    Lex(LexType t, const std::string& s, int n, int l, int c)
        : type(t), str(s), num(n), line(l), col(c) {}
};

// Команды внутреннего представления (ПОЛИЗ)
enum PolizCmdType {
    CMD_PUSH_INT,
    CMD_PUSH_STR,
    CMD_PUSH_BOOL,
    CMD_PUSH_VAR,
    CMD_LOAD_VAR,
    CMD_STORE,
    CMD_ADD, CMD_SUB, CMD_MUL, CMD_DIV,
    CMD_NEG,
    CMD_NOT,
    CMD_AND, CMD_OR,
    CMD_EQ, CMD_NEQ, CMD_LT, CMD_GT, CMD_LE, CMD_GE,
    CMD_READ,
    CMD_WRITE,
    CMD_JUMP,
    CMD_JUMP_IF_FALSE,
    CMD_LABEL,
    CMD_HALT
};

struct PolizCmd {
    PolizCmdType cmd;
    int arg_int;
    std::string arg_str;
    bool arg_bool;

    PolizCmd(PolizCmdType c) : cmd(c), arg_int(0), arg_bool(false) {}
    PolizCmd(PolizCmdType c, int a) : cmd(c), arg_int(a), arg_bool(false) {}
    PolizCmd(PolizCmdType c, const std::string& s) : cmd(c), arg_int(0), arg_str(s), arg_bool(false) {}
    PolizCmd(PolizCmdType c, bool b) : cmd(c), arg_int(0), arg_bool(b) {}
};