#pragma once
#include "Lex.h"
#include "Scanner.h"
#include <vector>
#include <stack>
#include <string>
#include <map>
#include <iostream>

enum Type { TYPE_INT, TYPE_STRING, TYPE_BOOL, TYPE_NONE };

struct Ident {
    std::string name;
    Type type;
    bool declared;
    Ident() : type(TYPE_NONE), declared(false) {}
    Ident(const std::string& n, Type t, bool d) : name(n), type(t), declared(d) {}
};

class Parser {
    Scanner scanner;
    Lex curr_lex;
    std::vector<Ident> TID;
    std::stack<int> st_int;          // для накопления индексов переменных при описании
    std::stack<LexType> st_lex;      // для типов операндов
    int cycle_depth;                 // вложенность циклов

    void gl();
    void error(const std::string &msg);
    int addIdent(const std::string &name);

    void dec(Type t);
    void check_id();
    void check_op();
    void check_not();
    void check_unary_minus();
    void eq_type();
    void eq_bool();
    void check_break_continue();

    void P();
    void Descriptions();
    void Description();
    void TypeDec(Type &t);
    void VarList();
    void Operators();
    void Operator();
    void Expression();
    void E();
    void E1();
    void E2();
    void E3();
    void E4();
    void E5();
    void E6();
    void E7();

public:
    Parser(std::istream &input);
    void analyze();
};