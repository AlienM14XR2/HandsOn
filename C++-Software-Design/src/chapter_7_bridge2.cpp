/**
 * 7 章 Bridge パターン、Prototype パターン、External Polymorphism パターン
 * 
 * Pimpl( Pointer-to-implementation ) イディオム
 * Bridge パターンにはずっち簡潔な形があり、C 及び C++ では数十年にわたり非常に広く使用されています。
 * Person クラスを例に考えてみましょう。
 * 
 * e.g. compile
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_7_bridge2.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

int main(void) {
    puts("START Pimpl( Pointer-to-implementation ) イディオム ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    puts("=== Pimpl( Pointer-to-implementation ) イディオム END");
    return 0;
}