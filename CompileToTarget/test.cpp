//
// Created by xys on 2021/1/3.
//

#include<iostream>

using namespace std;
extern "C" {
int gcd(int x, int y);
};

int main() {
    cout << gcd(10, 4) << endl;
    cout << gcd(10, 9) << endl;
    cout << gcd(777, 333) << endl;
    return 0;
}