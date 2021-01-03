//
// Created by xys on 2021/1/2.
//

#ifndef COMPILE_TO_TARGET_CODE_HELPER_HPP
#define COMPILE_TO_TARGET_CODE_HELPER_HPP

#include <string>
#include <iostream>
#include <vector>
#include "FuncPrototype.hpp"

using std::string, std::cout, std::endl;

static int process(string &str) {
    if (str[0] != '(') cout << "Error in process code " << str << endl;
    int idx = 1, real = 0;
    while (str[idx] != ')') real = real * 10 + str[idx++] - '0';
    str = str.substr(idx + 2);
    return real;
}

static vector<string> generateEmptyVecStr(const string &name) {
    vector<string> res;
    functionType func = funcProto[name];
    for(const auto& s:func.args)
        res.push_back(s);
    return std::move(res);
}

#endif //COMPILE_TO_TARGET_CODE_HELPER_HPP
