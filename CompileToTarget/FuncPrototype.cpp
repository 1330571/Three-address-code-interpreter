//
// Created by xys on 2021/1/3.
//

#include "FuncPrototype.hpp"
#include <iostream>
#include <fstream>

map<string, functionType> funcProto;

void buildFunc(const string &filepath) {
    ifstream file(filepath);
    if (!file.is_open()) {
        cerr << "函数原型加载失败" << endl;
        return;
    }
    string funcName;
    while(file >> funcName) {
        string param;
        file >> param;
        funcProto[funcName].name = funcName;
        while (file >> param && param != "end") {
            funcProto[funcName].args.push_back(param);
        }
    }
}