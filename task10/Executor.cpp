// Executor.cpp
#include "Executor.h"
#include <stdexcept>
#include <sstream>
#include <cctype>

void print_value(const Executor::Value& val, std::ostream& os = std::cout) {
    if (std::holds_alternative<int>(val)) os << std::get<int>(val);
    else if (std::holds_alternative<std::string>(val)) os << std::get<std::string>(val);
    else if (std::holds_alternative<bool>(val)) os << (std::get<bool>(val) ? "true" : "false");
}

Executor::Executor(const std::vector<PolizCmd>& program, const std::vector<Ident>& tid)
    : poliz(program), TID(tid), pc(0) {
    vars.resize(TID.size());
    for (size_t i = 0; i < TID.size(); ++i) {
        if (TID[i].type == TYPE_INT) vars[i] = 0;
        else if (TID[i].type == TYPE_STRING) vars[i] = std::string("");
        else if (TID[i].type == TYPE_BOOL) vars[i] = false;
    }
}

Executor::Value Executor::get_var(int idx) const {
    if (idx < 0 || idx >= (int)vars.size()) throw std::runtime_error("Variable index out of range");
    return vars[idx];
}

void Executor::set_var(int idx, const Value& val) {
    if (idx < 0 || idx >= (int)vars.size()) throw std::runtime_error("Variable index out of range");
    vars[idx] = val;
}

Executor::Value Executor::pop_val() {
    if (stack.empty()) throw std::runtime_error("Stack underflow");
    Value v = stack.top(); stack.pop();
    return v;
}

void Executor::push_val(const Value& val) {
    stack.push(val);
}

void Executor::execute_command(const PolizCmd& cmd) {
    switch(cmd.cmd) {
        case CMD_PUSH_INT: push_val(cmd.arg_int); break;
        case CMD_PUSH_STR: push_val(cmd.arg_str); break;
        case CMD_PUSH_BOOL: push_val(cmd.arg_bool); break;
        case CMD_PUSH_VAR: push_val(get_var(cmd.arg_int)); break;
        case CMD_LOAD_VAR: push_val(cmd.arg_int); break; // кладём индекс как адрес
        case CMD_STORE: {
            Value val = pop_val();
            Value addr = pop_val();
            if (!std::holds_alternative<int>(addr)) throw std::runtime_error("Store address is not int");
            int idx = std::get<int>(addr);
            set_var(idx, val);
            break;
        }
        case CMD_ADD: {
            Value right = pop_val(), left = pop_val();
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right))
                push_val(std::get<int>(left) + std::get<int>(right));
            else if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
                push_val(std::get<std::string>(left) + std::get<std::string>(right));
            else throw std::runtime_error("Invalid operands for +");
            break;
        }
        case CMD_SUB: {
            Value right = pop_val(), left = pop_val();
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right))
                push_val(std::get<int>(left) - std::get<int>(right));
            else throw std::runtime_error("Invalid operands for -");
            break;
        }
        case CMD_MUL: {
            Value right = pop_val(), left = pop_val();
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right))
                push_val(std::get<int>(left) * std::get<int>(right));
            else throw std::runtime_error("Invalid operands for *");
            break;
        }
        case CMD_DIV: {
            Value right = pop_val(), left = pop_val();
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
                int r = std::get<int>(right);
                if (r == 0) throw std::runtime_error("Division by zero");
                push_val(std::get<int>(left) / r);
            } else throw std::runtime_error("Invalid operands for /");
            break;
        }
        case CMD_NEG: {
            Value v = pop_val();
            if (std::holds_alternative<int>(v)) push_val(-std::get<int>(v));
            else throw std::runtime_error("Invalid operand for unary -");
            break;
        }
        case CMD_NOT: {
            Value v = pop_val();
            if (std::holds_alternative<bool>(v)) push_val(!std::get<bool>(v));
            else throw std::runtime_error("Invalid operand for not");
            break;
        }
        case CMD_AND: {
            Value right = pop_val(), left = pop_val();
            if (std::holds_alternative<bool>(left) && std::holds_alternative<bool>(right))
                push_val(std::get<bool>(left) && std::get<bool>(right));
            else throw std::runtime_error("Invalid operands for and");
            break;
        }
        case CMD_OR: {
            Value right = pop_val(), left = pop_val();
            if (std::holds_alternative<bool>(left) && std::holds_alternative<bool>(right))
                push_val(std::get<bool>(left) || std::get<bool>(right));
            else throw std::runtime_error("Invalid operands for or");
            break;
        }
        case CMD_EQ: {
            Value right = pop_val(), left = pop_val();
            if (left.index() != right.index()) push_val(false);
            else {
                if (std::holds_alternative<int>(left))
                    push_val(std::get<int>(left) == std::get<int>(right));
                else if (std::holds_alternative<std::string>(left))
                    push_val(std::get<std::string>(left) == std::get<std::string>(right));
                else if (std::holds_alternative<bool>(left))
                    push_val(std::get<bool>(left) == std::get<bool>(right));
                else push_val(false);
            }
            break;
        }
        case CMD_NEQ: {
            Value right = pop_val(), left = pop_val();
            if (left.index() != right.index()) push_val(true);
            else {
                if (std::holds_alternative<int>(left))
                    push_val(std::get<int>(left) != std::get<int>(right));
                else if (std::holds_alternative<std::string>(left))
                    push_val(std::get<std::string>(left) != std::get<std::string>(right));
                else if (std::holds_alternative<bool>(left))
                    push_val(std::get<bool>(left) != std::get<bool>(right));
                else push_val(true);
            }
            break;
        }
        case CMD_LT: case CMD_GT: case CMD_LE: case CMD_GE: {
            Value right = pop_val(), left = pop_val();
            bool res = false;
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
                int l = std::get<int>(left), r = std::get<int>(right);
                if (cmd.cmd == CMD_LT) res = l < r;
                else if (cmd.cmd == CMD_GT) res = l > r;
                else if (cmd.cmd == CMD_LE) res = l <= r;
                else if (cmd.cmd == CMD_GE) res = l >= r;
            } else if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right)) {
                std::string l = std::get<std::string>(left), r = std::get<std::string>(right);
                if (cmd.cmd == CMD_LT) res = l < r;
                else if (cmd.cmd == CMD_GT) res = l > r;
                else if (cmd.cmd == CMD_LE) res = l <= r;
                else if (cmd.cmd == CMD_GE) res = l >= r;
            } else throw std::runtime_error("Invalid operands for comparison");
            push_val(res);
            break;
        }
        case CMD_READ: {
            int idx = cmd.arg_int;
            std::string input;
            std::cin >> input;
            if (TID[idx].type == TYPE_INT) {
                bool is_num = true;
                for (char c : input) if (!isdigit(c) && c != '-') is_num = false;
                if (is_num) vars[idx] = std::stoi(input);
                else {
                    std::cout << "Invalid integer input" << std::endl;
                    vars[idx] = 0;
                }
            } else if (TID[idx].type == TYPE_STRING) {
                vars[idx] = input;
            }
            break;
        }
        case CMD_WRITE: {
            Value val = pop_val();
            print_value(val);
            std::cout << std::endl;
            break;
        }
        case CMD_JUMP: {
            pc = cmd.arg_int - 1;
            break;
        }
        case CMD_JUMP_IF_FALSE: {
            Value cond = pop_val();
            if (std::holds_alternative<bool>(cond) && !std::get<bool>(cond)) {
                pc = cmd.arg_int - 1;
            }
            break;
        }
        case CMD_LABEL: break;
        case CMD_HALT: {
            pc = poliz.size();
            break;
        }
        default: throw std::runtime_error("Unknown command");
    }
}

void Executor::run() {
    pc = 0;
    while (pc < (int)poliz.size()) {
        execute_command(poliz[pc]);
        pc++;
    }
}