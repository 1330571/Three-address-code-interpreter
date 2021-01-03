//
// Created by xys on 2021/1/2.
//

#include "Lexer.hpp"
#include "codeHelper.hpp"
#include "FuncPrototype.hpp"
#include <iostream>
#include <cctype>

Lexer::Lexer(const vector<string> &code) : code(code) {
    codeProcess();
}

const vector<string> &Lexer::getCode() {
    return code;
}

int Lexer::getCur() const {
    return cur;
}

const string &Lexer::getIdentifier() const {
    return identifier;
}

int Lexer::getNumLiteral() const {
    return numLiteral;
}

bool isOp(const string &str) {
    return (str == "+" || str == "-" || str == "/" || str == "*" || str == ":=" || str == "==" || str == "!=");
}

TOKEN Lexer::getToken() {
    static unsigned char LastChar = ' ';
    if (code[cur].size() <= this->lineCur) {
        this->cur++;
        this->lineCur = 0;
    }
    if (cur >= code.size()) return TK_EOF;
    LastChar = code[cur][lineCur++];
    while (isspace(LastChar))
        LastChar = code[cur][lineCur++];

    if (isalpha(LastChar)) { //标识符 [a-zA-Z][a-zA-Z0-9]* 变量
        this->identifier = LastChar;
        while (isalnum((LastChar = code[cur][lineCur++])))
            this->identifier += LastChar;

        if (this->identifier == "Prog")
            return TK_DEF; //function definition
        if (this->identifier == "extern")
            return TK_EOF;
        if (this->identifier == "return")
            return TK_RTN;
        if (this->identifier == "param")
            return TK_PRM;
        if (this->identifier == "call")
            return TK_CALL;
        if (this->identifier == "Goto")
            return TK_GOTO;
        if (this->identifier == "if")
            return TK_IF;
        if (this->identifier == "else")
            return TK_ELSE;
        if (this->identifier == "then")
            return TK_THEN;
        if (this->identifier == "end")
            return TK_END;
        return TK_IDT;
    }
    this->identifier = "";
    while (LastChar == ':' || LastChar == '+' || LastChar == '-' || LastChar == '*' || LastChar == '/' ||
           LastChar == '!' || LastChar == '=') {
        this->identifier += LastChar;
        LastChar = code[cur][lineCur++];
        if (isOp(this->identifier))
            return TK_OP;
    }

    if (isdigit(LastChar)) { //数字 [0-9]+
        this->numLiteral = 0;
        do {
            this->numLiteral = this->numLiteral * 10 + (LastChar - '0');
        } while (isdigit((LastChar = code[cur][++lineCur])));
        return TK_NUM;
    }

    if (LastChar == (unsigned char) EOF) return TK_EOF;
    ++lineCur;

    return TK_UKN;
}

int Lexer::getLineCur() const {
    return lineCur;
}

ostream &operator<<(ostream &os, const Lexer &lexer) {
    os << "cur: " << lexer.cur << " lineCur: " << lexer.lineCur << " identifier: " << lexer.identifier
       << " numLiteral: " << lexer.numLiteral;
    return os;
}

void Lexer::codeProcess() {
    for (auto &lineCode:this->code) {
        int extractLineInfo = process(lineCode);
        this->codeLineInfo.push_back(extractLineInfo);
    }
}
