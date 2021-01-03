//
// Created by xys on 2021/1/4.
//
#include<iostream>
using namespace std;

extern "C" {
    int fib(int x);
};

int main() {
    for (int i = 1; i <= 15; ++i)
        cout << "Fib[" << i << "] = " << fib(i) << endl;
    return 0;
}