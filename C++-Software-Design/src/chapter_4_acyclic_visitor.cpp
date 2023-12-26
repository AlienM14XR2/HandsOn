/**
 * 4 章 Visitor パターン の続き
 * 
 * # ガイドライン 18: Acyclic Visitor の性能を把握する
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_4_acyclic_visitor.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template<class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

int main(void) {
    puts("START 4 章 Visitor パターン の続き Acyclic Visitor ===");
    if(0.01) {
        double pi = 3.14159;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    puts("=== 4 章 Visitor パターン の続き Acyclic Visitor END");
    return EXIT_SUCCESS;
}