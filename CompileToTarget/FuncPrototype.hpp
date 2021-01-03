//
// Created by xys on 2021/1/3.
//

#ifndef COMPILE_TO_TARGET_FUNC_PROTOTYPE_HPP
#define COMPILE_TO_TARGET_FUNC_PROTOTYPE_HPP
#include <map>
#include<string>
#include <vector>
using namespace  std;
struct functionType{
    string name;
    vector<string> args;
};

extern map<string,functionType> funcProto;

void buildFunc(const string& filepath);


#endif //COMPILE_TO_TARGET_FUNC_PROTOTYPE_HPP
