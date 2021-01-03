//
// Created by xys on 2021/1/2.
//

#ifndef COMPILE_TO_TARGET_LEXER_HPP
#define COMPILE_TO_TARGET_LEXER_HPP

#include "type.hpp"
#include <string>
#include <vector>
#include <ostream>
#include <map>
#include "FuncPrototype.hpp"
using namespace std;

struct lineStructure {
    int line;
    vector<pair<string, TOKEN>> v;
};

class Lexer {
private:

    vector<string> code;
    int cur{};
    int lineCur{};
    //下面的变量用来标记当前词法分析的状态
    string identifier;
    int numLiteral{};
    vector<int> codeLineInfo;

    void codeProcess();

public:
    vector<lineStructure> codeInfo;

    map<string, int> labelRecord;

    friend ostream &operator<<(ostream &os, const Lexer &lexer);

    explicit Lexer(const vector<string> &code);

    const vector<string> &getCode();

    int getCur() const;

    TOKEN getToken();

    int getLineCur() const;

    const string &getIdentifier() const;

    int getNumLiteral() const;
};

#endif //COMPILE_TO_TARGET_LEXER_HPP
