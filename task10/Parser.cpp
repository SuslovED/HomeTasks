#include "Parser.h"
#include <stdexcept>
#include <sstream>

Parser::Parser(std::istream &input) : scanner(input), cycle_depth(0) {
    gl();
}

void Parser::gl() {
    curr_lex = scanner.getLex();
}

void Parser::error(const std::string &msg) {
    std::ostringstream oss;
    oss << "Error at line " << curr_lex.line << ", col " << curr_lex.col
        << ": " << msg;
    throw std::runtime_error(oss.str());
}

int Parser::addIdent(const std::string &name) {
    for (size_t i = 0; i < TID.size(); ++i)
        if (TID[i].name == name) return i;
    TID.push_back(Ident(name, TYPE_NONE, false));
    return TID.size() - 1;
}

void Parser::dec(Type t) {
    while (!st_int.empty()) {
        int idx = st_int.top(); st_int.pop();
        if (TID[idx].declared) error("Variable '" + TID[idx].name + "' already declared");
        TID[idx].declared = true;
        TID[idx].type = t;
    }
}

void Parser::check_id() {
    int idx = addIdent(curr_lex.str);
    if (!TID[idx].declared) error("Identifier '" + curr_lex.str + "' not declared");
    LexType lex_type;
    switch (TID[idx].type) {
        case TYPE_INT:    lex_type = LEX_INT; break;
        case TYPE_STRING: lex_type = LEX_STR; break;
        case TYPE_BOOL:   lex_type = LEX_BOOL; break;
        default: error("Identifier without type");
    }
    st_lex.push(lex_type);
}

void Parser::check_op() {
    LexType t2 = st_lex.top(); st_lex.pop();
    LexType op = st_lex.top(); st_lex.pop();
    LexType t1 = st_lex.top(); st_lex.pop();

    if (op == LEX_PLUS || op == LEX_MINUS || op == LEX_STAR || op == LEX_SLASH) {
        if (t1 == LEX_STR && t2 == LEX_STR && op == LEX_PLUS) {
            st_lex.push(LEX_STR); return;
        }
        if (t1 == LEX_INT && t2 == LEX_INT) {
            st_lex.push(LEX_INT); return;
        }
        error("Incompatible types for arithmetic operation");
    }
    if (op == LEX_AND || op == LEX_OR) {
        if (t1 == LEX_BOOL && t2 == LEX_BOOL) {
            st_lex.push(LEX_BOOL); return;
        }
        error("Boolean operations require boolean operands");
    }
    if (op == LEX_EQ || op == LEX_NEQ || op == LEX_LT || op == LEX_GT ||
        op == LEX_LE || op == LEX_GE) {
        if ((t1 == LEX_INT && t2 == LEX_INT) ||
            (t1 == LEX_STR && t2 == LEX_STR) ||
            (t1 == LEX_BOOL && t2 == LEX_BOOL)) {
            st_lex.push(LEX_BOOL); return;
        }
        error("Comparison requires compatible types");
    }
    error("Unknown operator");
}

void Parser::check_not() {
    LexType t = st_lex.top(); st_lex.pop();
    if (t != LEX_BOOL) error("not requires boolean operand");
    st_lex.push(LEX_BOOL);
}

void Parser::check_unary_minus() {
    LexType t = st_lex.top(); st_lex.pop();
    if (t != LEX_INT) error("unary minus requires integer operand");
    st_lex.push(LEX_INT);
}

void Parser::eq_type() {
    LexType t_expr = st_lex.top(); st_lex.pop();
    LexType t_var = st_lex.top(); st_lex.pop();
    if (!((t_var == LEX_INT && t_expr == LEX_INT) ||
          (t_var == LEX_STR && t_expr == LEX_STR) ||
          (t_var == LEX_BOOL && t_expr == LEX_BOOL)))
        error("Type mismatch in assignment");
    st_lex.push(t_expr);   // результат присваивания имеет тип правой части
}

void Parser::eq_bool() {
    if (st_lex.top() != LEX_BOOL) error("Condition must be boolean");
    st_lex.pop();
}

void Parser::check_break_continue() {
    if (cycle_depth == 0) error("break/continue outside of loop");
}

//=== Грамматика ===

void Parser::P() {
    if (curr_lex.type != LEX_PROGRAM) error("Expected 'program'");
    gl();
    if (curr_lex.type != LEX_LBRACE) error("Expected '{'");
    gl();
    Descriptions();
    Operators();
    if (curr_lex.type != LEX_RBRACE) error("Expected '}'");
    gl();
    if (curr_lex.type != LEX_FIN) error("Extra tokens after end of program");
}

void Parser::Descriptions() {
    while (curr_lex.type == LEX_INT || curr_lex.type == LEX_STRING ||
           curr_lex.type == LEX_BOOL) {
        Description();
        if (curr_lex.type != LEX_SEMICOLON) error("Expected ';' after description");
        gl();
    }
}

void Parser::Description() {
    Type t;
    TypeDec(t);
    VarList();
    dec(t);
}

void Parser::TypeDec(Type &t) {
    if (curr_lex.type == LEX_INT) t = TYPE_INT;
    else if (curr_lex.type == LEX_STRING) t = TYPE_STRING;
    else if (curr_lex.type == LEX_BOOL) t = TYPE_BOOL;
    else error("Expected type name");
    gl();
}

void Parser::VarList() {
    if (curr_lex.type != LEX_ID) error("Expected variable name");
    st_int.push(addIdent(curr_lex.str));
    gl();
    while (curr_lex.type == LEX_COMMA) {
        gl();
        if (curr_lex.type != LEX_ID) error("Expected variable name after ','");
        st_int.push(addIdent(curr_lex.str));
        gl();
    }
}

void Parser::Operators() {
    while (curr_lex.type != LEX_FIN && curr_lex.type != LEX_RBRACE) {
        Operator();
    }
}

void Parser::Operator() {
    if (curr_lex.type == LEX_IF) {
        gl();
        if (curr_lex.type != LEX_LPAREN) error("Expected '('");
        gl();
        Expression();
        eq_bool();
        if (curr_lex.type != LEX_RPAREN) error("Expected ')'");
        gl();
        Operator();  // then
        if (curr_lex.type == LEX_ELSE) {
            gl();
            Operator();
        }
    }
    else if (curr_lex.type == LEX_WHILE) {
        gl();
        if (curr_lex.type != LEX_LPAREN) error("Expected '('");
        gl();
        Expression();
        eq_bool();
        if (curr_lex.type != LEX_RPAREN) error("Expected ')'");
        gl();
        ++cycle_depth;
        Operator();
        --cycle_depth;
    }
    else if (curr_lex.type == LEX_DO) {
        gl();
        ++cycle_depth;
        Operator();
        --cycle_depth;
        if (curr_lex.type != LEX_WHILE) error("Expected 'while' after do");
        gl();
        if (curr_lex.type != LEX_LPAREN) error("Expected '('");
        gl();
        Expression();
        eq_bool();
        if (curr_lex.type != LEX_RPAREN) error("Expected ')'");
        gl();
        if (curr_lex.type != LEX_SEMICOLON) error("Expected ';' after do-while");
        gl();
    }
    else if (curr_lex.type == LEX_BREAK) {
        check_break_continue();
        gl();
        if (curr_lex.type != LEX_SEMICOLON) error("Expected ';' after break");
        gl();
    }
    else if (curr_lex.type == LEX_CONTINUE) {
        check_break_continue();
        gl();
        if (curr_lex.type != LEX_SEMICOLON) error("Expected ';' after continue");
        gl();
    }
    else if (curr_lex.type == LEX_READ) {
        gl();
        if (curr_lex.type != LEX_LPAREN) error("Expected '('");
        gl();
        if (curr_lex.type != LEX_ID) error("Expected identifier in read");
        int idx = addIdent(curr_lex.str);
        if (!TID[idx].declared) error("Identifier not declared");
        if (TID[idx].type == TYPE_BOOL) error("Cannot read boolean variable");
        gl();
        if (curr_lex.type != LEX_RPAREN) error("Expected ')'");
        gl();
        if (curr_lex.type != LEX_SEMICOLON) error("Expected ';' after read");
        gl();
    }
    else if (curr_lex.type == LEX_WRITE) {
        gl();
        if (curr_lex.type != LEX_LPAREN) error("Expected '('");
        gl();
        Expression();
        while (curr_lex.type == LEX_COMMA) {
            gl();
            Expression();
        }
        if (curr_lex.type != LEX_RPAREN) error("Expected ')'");
        gl();
        if (curr_lex.type != LEX_SEMICOLON) error("Expected ';' after write");
        gl();
        // очищаем стек типов выведенных выражений
        while (!st_lex.empty()) st_lex.pop();
    }
    else if (curr_lex.type == LEX_LBRACE) {   // составной оператор
        gl();
        while (curr_lex.type != LEX_RBRACE) Operator();
        gl();
    }
    else if (curr_lex.type == LEX_SEMICOLON) { // пустой оператор
        gl();
    }
    else { // оператор-выражение
        Expression();
        if (curr_lex.type != LEX_SEMICOLON) error("Expected ';' after expression");
        gl();
    }
}

void Parser::Expression() {
    E();
}

void Parser::E() {
    E1();
    if (curr_lex.type == LEX_ASSIGN) {
        gl();
        E();
        eq_type();
    }
}

void Parser::E1() {
    E2();
    while (curr_lex.type == LEX_OR) {
        st_lex.push(curr_lex.type);   // кладём оператор
        gl();
        E2();
        check_op();
    }
}

void Parser::E2() {
    E3();
    while (curr_lex.type == LEX_AND) {
        st_lex.push(curr_lex.type);
        gl();
        E3();
        check_op();
    }
}

void Parser::E3() {
    E4();
    if (curr_lex.type == LEX_EQ || curr_lex.type == LEX_NEQ ||
        curr_lex.type == LEX_LT || curr_lex.type == LEX_GT ||
        curr_lex.type == LEX_LE || curr_lex.type == LEX_GE) {
        LexType rel = curr_lex.type;
        st_lex.push(rel);   // оператор отношения
        gl();
        E4();
        check_op();
    }
}

void Parser::E4() {
    E5();
    while (curr_lex.type == LEX_PLUS || curr_lex.type == LEX_MINUS) {
        LexType op = curr_lex.type;
        st_lex.push(op);    // оператор сложения/вычитания
        gl();
        E5();
        check_op();
    }
}

void Parser::E5() {
    E6();
    while (curr_lex.type == LEX_STAR || curr_lex.type == LEX_SLASH) {
        LexType op = curr_lex.type;
        st_lex.push(op);    // оператор умножения/деления
        gl();
        E6();
        check_op();
    }
}

void Parser::E6() {
    if (curr_lex.type == LEX_MINUS) {
        gl();
        E6();
        check_unary_minus();
    }
    else if (curr_lex.type == LEX_NOT) {
        gl();
        E6();
        check_not();
    }
    else {
        E7();
    }
}

void Parser::E7() {
    if (curr_lex.type == LEX_ID) {
        check_id();
        gl();
    }
    else if (curr_lex.type == LEX_NUM) {
        st_lex.push(LEX_INT);
        gl();
    }
    else if (curr_lex.type == LEX_STR) {
        st_lex.push(LEX_STR);
        gl();
    }
    else if (curr_lex.type == LEX_TRUE || curr_lex.type == LEX_FALSE) {
        st_lex.push(LEX_BOOL);
        gl();
    }
    else if (curr_lex.type == LEX_LPAREN) {
        gl();
        Expression();
        if (curr_lex.type != LEX_RPAREN) error("Expected ')'");
        gl();
    }
    else error("Unexpected token in expression");
}

void Parser::analyze() {
    try {
        P();
        std::cout << "OK" << std::endl;
    } catch (const std::exception &e) {
        std::cout << "Analysis error: " << e.what() << std::endl;
    }
}