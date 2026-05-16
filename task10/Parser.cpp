#include "Parser.h"
#include <stdexcept>
#include <sstream>

Parser::Parser(std::istream &input) : scanner(input), cycle_depth(0), label_counter(0) {
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
    st_lex.push(t_expr);
}

void Parser::eq_bool() {
    if (st_lex.top() != LEX_BOOL) error("Condition must be boolean");
    st_lex.pop();
}

void Parser::check_break_continue() {
    if (cycle_depth == 0) error("break/continue outside of loop");
}

//------------------ Генерация ПОЛИЗа ------------------
void Parser::emit(const PolizCmd& cmd) {
    poliz.push_back(cmd);
}

int Parser::make_label() {
    return ++label_counter;
}

void Parser::emit_label(int lbl) {
    label_addr[lbl] = poliz.size();   // запоминаем индекс, куда вставлена метка
    poliz.push_back(PolizCmd(CMD_LABEL, lbl));
    // разрешаем все переходы, ожидавшие эту метку
    for (size_t i = 0; i < pending_jumps.size(); ) {
        if (pending_jumps[i].second == lbl) {
            int cmd_idx = pending_jumps[i].first;
            poliz[cmd_idx].arg_int = label_addr[lbl];
            // удаляем запись
            pending_jumps.erase(pending_jumps.begin() + i);
            // не увеличиваем i, так как элементы сдвинулись
        } else {
            ++i;
        }
    }
}

void Parser::emit_jump(int lbl) {
    auto it = label_addr.find(lbl);
    if (it != label_addr.end()) {
        emit(PolizCmd(CMD_JUMP, it->second));   // метка уже известна
    } else {
        emit(PolizCmd(CMD_JUMP, lbl));          // временно кладём номер метки
        pending_jumps.emplace_back(poliz.size() - 1, lbl);
    }
}

void Parser::emit_jump_if_false(int lbl) {
    auto it = label_addr.find(lbl);
    if (it != label_addr.end()) {
        emit(PolizCmd(CMD_JUMP_IF_FALSE, it->second));
    } else {
        emit(PolizCmd(CMD_JUMP_IF_FALSE, lbl));
        pending_jumps.emplace_back(poliz.size() - 1, lbl);
    }
}

void Parser::gen_constant() {
    switch (curr_lex.type) {
        case LEX_NUM:
            emit(PolizCmd(CMD_PUSH_INT, curr_lex.num));
            break;
        case LEX_STR:
            emit(PolizCmd(CMD_PUSH_STR, curr_lex.str));
            break;
        case LEX_TRUE:
            emit(PolizCmd(CMD_PUSH_BOOL, true));
            break;
        case LEX_FALSE:
            emit(PolizCmd(CMD_PUSH_BOOL, false));
            break;
        default:
            error("Internal: unexpected constant");
    }
}

//------------------ Грамматика ------------------

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
    emit(PolizCmd(CMD_HALT));
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
        int else_label = make_label();
        emit_jump_if_false(else_label);
        Operator(); // then
        if (curr_lex.type == LEX_ELSE) {
            int end_label = make_label();
            emit_jump(end_label);
            emit_label(else_label);
            gl();
            Operator(); // else
            emit_label(end_label);
        } else {
            emit_label(else_label);
        }
    }
    else if (curr_lex.type == LEX_WHILE) {
        gl();
        if (curr_lex.type != LEX_LPAREN) error("Expected '('");
        gl();
        int begin_loop = make_label();
        int end_loop = make_label();
        emit_label(begin_loop);
        Expression();
        eq_bool();
        if (curr_lex.type != LEX_RPAREN) error("Expected ')'");
        gl();
        emit_jump_if_false(end_loop);
        ++cycle_depth;
        break_stack.push(end_loop);
        continue_stack.push(begin_loop);
        Operator(); // body
        break_stack.pop();
        continue_stack.pop();
        --cycle_depth;
        emit_jump(begin_loop);
        emit_label(end_loop);
    }
    else if (curr_lex.type == LEX_DO) {
        gl();
        int begin_loop = make_label();
        int end_loop = make_label();
        emit_label(begin_loop);
        ++cycle_depth;
        break_stack.push(end_loop);
        continue_stack.push(begin_loop);
        Operator(); // body
        break_stack.pop();
        continue_stack.pop();
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
        emit_jump_if_false(end_loop);
        emit_jump(begin_loop);
        emit_label(end_loop);
    }
    else if (curr_lex.type == LEX_BREAK) {
        check_break_continue();
        if (break_stack.empty()) error("break outside of loop");
        emit_jump(break_stack.top());
        gl();
        if (curr_lex.type != LEX_SEMICOLON) error("Expected ';' after break");
        gl();
    }
    else if (curr_lex.type == LEX_CONTINUE) {
        check_break_continue();
        if (continue_stack.empty()) error("continue outside of loop");
        emit_jump(continue_stack.top());
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
        emit(PolizCmd(CMD_READ, idx));
    }
    else if (curr_lex.type == LEX_WRITE) {
        gl();
        if (curr_lex.type != LEX_LPAREN) error("Expected '('");
        gl();
        Expression();
        emit(PolizCmd(CMD_WRITE));
        while (curr_lex.type == LEX_COMMA) {
            gl();
            Expression();
            emit(PolizCmd(CMD_WRITE));
        }
        if (curr_lex.type != LEX_RPAREN) error("Expected ')'");
        gl();
        if (curr_lex.type != LEX_SEMICOLON) error("Expected ';' after write");
        gl();
    }
    else if (curr_lex.type == LEX_LBRACE) {
        gl();
        while (curr_lex.type != LEX_RBRACE) Operator();
        gl();
    }
    else if (curr_lex.type == LEX_SEMICOLON) {
        gl();
    }
    else {
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
        if (poliz.empty() || poliz.back().cmd != CMD_PUSH_VAR)
            error("Assignment to non-variable");
        int idx = poliz.back().arg_int;
        poliz.pop_back();
        emit(PolizCmd(CMD_LOAD_VAR, idx));
        gl();
        E();
        eq_type();
        emit(PolizCmd(CMD_STORE));
        // для цепочки a = b = c оставляем значение на стеке
        emit(PolizCmd(CMD_PUSH_VAR, idx));
    }
}

void Parser::E1() {
    E2();
    while (curr_lex.type == LEX_OR) {
        st_lex.push(curr_lex.type);
        gl();
        E2();
        check_op();
        emit(PolizCmd(CMD_OR));
    }
}

void Parser::E2() {
    E3();
    while (curr_lex.type == LEX_AND) {
        st_lex.push(curr_lex.type);
        gl();
        E3();
        check_op();
        emit(PolizCmd(CMD_AND));
    }
}

void Parser::E3() {
    E4();
    if (curr_lex.type == LEX_EQ || curr_lex.type == LEX_NEQ ||
        curr_lex.type == LEX_LT || curr_lex.type == LEX_GT ||
        curr_lex.type == LEX_LE || curr_lex.type == LEX_GE) {
        LexType rel = curr_lex.type;
        st_lex.push(rel);
        gl();
        E4();
        check_op();
        switch(rel) {
            case LEX_EQ: emit(PolizCmd(CMD_EQ)); break;
            case LEX_NEQ: emit(PolizCmd(CMD_NEQ)); break;
            case LEX_LT: emit(PolizCmd(CMD_LT)); break;
            case LEX_GT: emit(PolizCmd(CMD_GT)); break;
            case LEX_LE: emit(PolizCmd(CMD_LE)); break;
            case LEX_GE: emit(PolizCmd(CMD_GE)); break;
            default: break;
        }
    }
}

void Parser::E4() {
    E5();
    while (curr_lex.type == LEX_PLUS || curr_lex.type == LEX_MINUS) {
        LexType op = curr_lex.type;
        st_lex.push(op);
        gl();
        E5();
        check_op();
        emit(op == LEX_PLUS ? PolizCmd(CMD_ADD) : PolizCmd(CMD_SUB));
    }
}

void Parser::E5() {
    E6();
    while (curr_lex.type == LEX_STAR || curr_lex.type == LEX_SLASH) {
        LexType op = curr_lex.type;
        st_lex.push(op);
        gl();
        E6();
        check_op();
        emit(op == LEX_STAR ? PolizCmd(CMD_MUL) : PolizCmd(CMD_DIV));
    }
}

void Parser::E6() {
    if (curr_lex.type == LEX_MINUS) {
        gl();
        E6();
        check_unary_minus();
        emit(PolizCmd(CMD_NEG));
    }
    else if (curr_lex.type == LEX_NOT) {
        gl();
        E6();
        check_not();
        emit(PolizCmd(CMD_NOT));
    }
    else {
        E7();
    }
}

void Parser::E7() {
    if (curr_lex.type == LEX_ID) {
        check_id();
        int idx = addIdent(curr_lex.str);
        emit(PolizCmd(CMD_PUSH_VAR, idx));
        gl();
    }
    else if (curr_lex.type == LEX_NUM || curr_lex.type == LEX_STR ||
             curr_lex.type == LEX_TRUE || curr_lex.type == LEX_FALSE) {
        gen_constant();
        if (curr_lex.type == LEX_NUM) st_lex.push(LEX_INT);
        else if (curr_lex.type == LEX_STR) st_lex.push(LEX_STR);
        else st_lex.push(LEX_BOOL);
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
        poliz.clear();   // очищаем неполную программу
    }
}