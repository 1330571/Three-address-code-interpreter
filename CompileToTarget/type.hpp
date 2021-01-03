//
// Created by xys on 2021/1/2.
//

#ifndef COMPILE_TO_TARGET_TYPE_HPP
#define COMPILE_TO_TARGET_TYPE_HPP

#include<string>

enum TOKEN {
    TK_EOF,
    TK_DEF,

    TK_ETN,
    TK_IDT,
    TK_RTN,
    TK_PRM,
    TK_CALL,
    TK_GOTO,
    TK_IF,
    TK_ELSE,
    TK_THEN,
    TK_END,

    TK_NUM,
    TK_OP,
    TK_UKN
};

static std::string print_helper(TOKEN token) {
    switch (token) {
        case TK_GOTO:
            return "GOTO";
        case TK_EOF:
            return "EOF";
        case TK_DEF:
            return "Function Def";
        case TK_ETN:
            return "Function Extern";
        case TK_IDT:
            return "Identity Def";
        case TK_RTN:
            return "Function Return";
        case TK_PRM:
            return "Parameter Def";
        case TK_CALL:
            return "Function Call";
        case TK_NUM:
            return "Value Literal";
        case TK_OP:
            return "Binary OP";
        case TK_UKN:
            return "Unknown";
        case TK_IF:
            return "If";
        case TK_ELSE:
            return "Else";
        case TK_THEN:
            return "Then";
        case TK_END:
            return "End";
    }
}

#endif //COMPILE_TO_TARGET_TYPE_HPP
