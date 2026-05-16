#pragma once
#include "Lex.h"
#include <istream>
#include <string>

class Scanner {
    std::istream &in;
    char c;
    int line, col;

    void nextChar();
    void skipSpacesAndComments();
    Lex getIdent();
    Lex getNumber();
    Lex getString();

public:
    Scanner(std::istream &input);
    Lex getLex();
};
