/**
 * 6 章 Adapter パターン、Observer パターン、CRTP パターン
 * 
 * CRTP（Curiously Recurring Template Pattern）... 直訳 ... 奇妙なことに繰り返し発生するテンプレート パターン。
 * 
 * ガイドライン 26：型を静的に区別するには CRTP パターン
 * 
 * e.g. compile
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_6_CRTP.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

int main(void) {
    puts("START CRTP パターン ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    puts("=== CRTP パターン END");
    return 0;
}
