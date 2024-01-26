/**
 * 2 章 auto
 * 
 * 推論規則に完全に準拠しているにも関わらず、プログラマからみて完全に誤っていることもあります。
 * この場合でも、従来の手続きによる型宣言に戻るなどは可能な限り避けるのが最善であり、auto が型
 * を正しく推論できるようにする方法を把握しておくことが重要です。
 * 
 * ```
 * 個人的感想、意見。
 * そこまで型を意識しない方がいいとする理由がほしい。
 * この章を進めていけば納得できるのだろうか。
 * ```
 * 
 * 項目 5 ：明示的型宣言よりも auto を優先する
 * 
 * e.g.) compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_2_auto.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * auto は初期化子がないとコンパイルエラーになる。
 * つまり、初期化忘れや未定義の状態をなくせる。
*/
void sample() {
    puts("--- sample");
    // int x;          // 未初期化のおそれ ... 私はコンパイルオプションで -Wall -Werror を指定しているのでこれもエラーになった。
                    // error: ‘x’ may be used uninitialized [-Werror=maybe-uninitialized]

    auto x2 = 3;    // 値 3 が確実に定義されている
    // auto x3;     // エラー、初期化子がない   error: declaration of ‘auto x3’ has no initializer

    // ptr_lambda_debug<const char*,const decltype(x)&>("x is ",x);
    ptr_lambda_debug<const char*,const decltype(x2)&>("x2 is ",x2);
    // ptr_lambda_debug<const char*,const decltype(x3)&>("x is ",x3);
}

int main(void) {
    puts("START 2 章 auto ===");
    if(0.01) {
        auto pi = 3.141592;
        ptr_lambda_debug<const char*,const decltype(pi)&>("pi is ",pi);
    }
    if(1.00) {
        sample();
    }
    puts("=== 2 章 auto END");
    return 0;
}
