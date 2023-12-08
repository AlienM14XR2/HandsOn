/**
 * 11 章 Proxy パターン（保護プロキシ）
 * 
 * RMI は私には C++ で書けない（@see chapter_11_proxy.cpp）。
 * ここでは、サンプルの「保護プロキシ」について書いてみる。
 * 
 * e.g. compile
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_11b_proxy.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug)-> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

int main(void) {
    puts("START 11 章 Proxy パターン（保護プロキシ）===");
    if(0.01) {
        double pi = 3.14159;
        ptr_lambda_debug<const char*,const double>("pi is ",pi);
    }
    puts("=== 11 章 Proxy パターン（保護プロキシ）END");
    return 0;
}