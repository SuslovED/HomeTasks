// Executor.h
#pragma once
#include "Lex.h"
#include "Parser.h"    // для Ident и Type
#include <vector>
#include <stack>
#include <string>
#include <variant>
#include <iostream>

class Executor {
public:
    using Value = std::variant<int, std::string, bool>;
private:
    const std::vector<PolizCmd>& poliz;
    std::vector<Ident> TID;
    std::vector<Value> vars;
    std::stack<Value> stack;
    int pc;

    Value get_var(int idx) const;
    void set_var(int idx, const Value& val);
    Value pop_val();
    void push_val(const Value& val);
    void execute_command(const PolizCmd& cmd);
public:
    Executor(const std::vector<PolizCmd>& program, const std::vector<Ident>& tid);
    void run();
};