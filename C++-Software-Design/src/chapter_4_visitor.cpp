/**
 * 4 章 Visitor パターン
 * 
 * 確かになぜこれが最初に紹介すべきパターンなのか理解できない。
 * 
 * ガイドライン 15: 型または処理の追加に備え設計する。
 * 
 * 図形描画をサンプルに説明するようだ。
 * 
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_4_visitor.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

int main() {
    puts("START 4 章 Visitor パターン ===");
    if(0.01) {
        double pi = 3.14159;
        ptr_lambda_debug<const char*,const double&>("pi is ", pi);
    }
    puts("=== 4 章 Visitor パターン END");
}