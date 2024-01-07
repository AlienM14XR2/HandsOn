/**
 * 6 章 Adapter パターン、Observer パターン、CRTP パターン
 * 
 * 値セマンティクスベースの Observer パターン実装
 * 
 * e.g. compile
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_6_observer2.cpp -o ../bin/main
 * 
*/
#include <iostream>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

int main(void) {
    puts("START 値セマンティクスベースの Observer パターン ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const int&>("pi is ",pi);
    }
    puts("=== 値セマンティクスベースの Observer パターン END");
    return EXIT_SUCCESS;    // return 0
}