//
// Created by xys on 2021/1/2.
//

#ifndef COMPILE_TO_TARGET_PARSER_HPP
#define COMPILE_TO_TARGET_PARSER_HPP

#include <ostream>
#include <vector>
#include <string>

#include "type.hpp"
#include "Lexer.hpp"

class Parser {
private:
    vector<string> paramStack;
    Lexer lexer;
    TOKEN curToken{};
public:

    friend ostream &operator<<(ostream &os, const Parser &parser);

    const Lexer &getLexer() const;

    TOKEN getCurToken() const;

    explicit Parser(Lexer lexer);

    TOKEN getNextToken();

    void parseToken();
};

#endif //COMPILE_TO_TARGET_PARSER_HPP
