/**
 * 9 章の続き Composite パターン
 * 
 * Composite パターンの定義
 * 
 * オブジェクトをツリー構造に構成して部分 ー 全体階層を表現できる。
 * Composite パターンを使うと、クライアントは個々のオブジェクトと
 * オブジェクトのコンポジションを統一的に扱うことができる。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_9_composite.cpp -o ../bin/main
 * 
*/
#include <iostream>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

int main(void) {
    puts("START 9 章の続き Composite パターン ===");
    if(0.01) {
        double pi = 3.14159;
        ptr_lambda_debug<const string&,const double&>("pi is ",pi);
    }
    puts("=== 9 章の続き Composite パターン END");
    return 0;
}