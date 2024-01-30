/**
 * 3 章 現代の C++ への移行
 * 
 * 項目 9 ：typedef よりも エイリアス宣言を優先する
 * 
 * e.g.) compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_3_modern_c++_using_alias.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](const auto message, const auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * typedef と エイリアス宣言（C++11）は完全に同じ内容ですから、どちらかを優先する技術的に充分な理由
 * などあるのだろうかと疑問に思うのも当然です。
 * 理由はあります。しかし、それを説明する前に、関数ポインタが関係する型の場合は、エイリアス宣言の方
 * が分かりやすいという意見が多いことに触れておきます。
*/

typedef void (*FP1)(int, const std::string&);              // typedef
using FP2 = void (*)(int, const std::string&);             // using alias

// 上記は可読性に差異はないと思う

void foo(int n, const std::string& str) {
    puts("------ foo");
    for(int i = 0; i < n; i++) {
        ptr_lambda_debug<const char*,const decltype(str)&>("str is ",str);
    }
}

void sample() {
    puts("--- sample");
    FP1 fp1 = foo;
    fp1(3, "Jack");
    FP2 fp2 = foo;
    fp2(6, "Derek");
}

/**
 * 著者が考える妥当な理由は、テンプレートにあるようだ。
*/

int main(void) {
    puts("START 項目 9 ：typedef よりも エイリアス宣言を優先する ===");
    if(0.01) {
        auto pi = 3.141592;
        ptr_lambda_debug<const char*,const decltype(pi)&>("pi is ",pi);
    }
    if(1.00) {
        sample();
    }
    puts("=== 項目 9 ：typedef よりも エイリアス宣言を優先する END");
    return 0;
}