/**
 * 5 章 右辺値参照、ムーブセマンティクス、完全転送
 * 
 * 項目 29 ：ムーブ演算は存在しない、コスト安でもない、使用もされないと想定する
 * 
 * 何か見も蓋ない タイトルだが、言いたいことは判る。
 * 
 *  C++11 の最大の目玉機能はムーブセマンティクスであることはまず間違いありません。「コンテナのムーブは、現代ではポインタ程度のコストなのだ！」
 * 、「一時オブジェクトのコピーは、現代ではとても効率が良く、これを避けるコーディングは早計な最適化に等しいのだ！」そんな言葉を耳にしたことが
 * あるでしょう。このような情動的な文言は受け入れられやすいものです。ムーブセマンティクスは真に重要な機能です。コスト高なコピー演算を比較的コ
 * スト安なムーブ演算に単に置き換え可能にするばかりか、実際にそうすることが『コンパイラの要件』とされています（適切な条件が満たされていれば）。
 *  性能を引き出せるムーブセマンティクスにはすでに伝説めいたオーラが漂っています。しかし、伝説とは一般に誇張です。本項目は過度に期待せず、地
 * に足のついた理解を目的とします。
 * 
 * ※ 本項はコーディングよりも Reading が主になりそうだだな。
 * 
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_5_move_forward_29.cpp -o ../bin/main
*/
#include <iostream>
#include <cassert>

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](const auto message, const auto debug) -> void {
    std::cout << "DEBUG: " << message << '\t' << debug << std::endl;
};

template <class Error>
concept ErrReasonable = requires(Error& e) {
    e.what();
};
template <class Error>
requires ErrReasonable<Error>
void (*ptr_print_error)(Error) = [](const auto e) -> void {
    std::cout << "ERROR: " << e.what() << std::endl;
};

int test_debug_and_error() {
    puts("=== test_debug_and_error");
    try {
        // ptr_print_now();
        auto pi = 3.141592;
        ptr_lambda_debug<const char*,const decltype(pi)&>("pi is ", pi);
        ptr_lambda_debug<const char*,const decltype(pi)*>("pi addr is ", &pi);
        throw std::runtime_error("It's test error.");
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        printf("e type is ... %s\n", typeid(e).name());
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int main(void) {
    puts("START 項目 29 ：ムーブ演算は存在しない、コスト安でもない、使用もされないと想定する ===");
    if(0.01) {
        auto ret = 0;
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", ret = test_debug_and_error());
        assert(ret == 1);   // error になることを期待している。
    }
    puts("=== 項目 29 ：ムーブ演算は存在しない、コスト安でもない、使用もされないと想定する END");
    return 0;
}