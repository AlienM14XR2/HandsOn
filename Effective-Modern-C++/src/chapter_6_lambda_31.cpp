/**
 * 6 章 ラムダ式
 * 
 * 『ラムダ式』
 * その名の通り、式である。ソースコードの一部である。
 * 
 * std::find_if(container.begin(), container.end(),
 *      [](int val) { return 0 < val && val 10 });
 * 
 * 『クロージャ』
 * ラムダが作成する実行時オブジェクトである。キャプチャモードに従い、キャプチャデータの参照もしくはコピーを保持する。
 * 上例の 第 3 実引数に渡しているオブジェクトがクロージャである。
 * 
 * 項目 31 ：デフォルトのキャプチャモードは避ける
 * 
 * e.g. compile.
 * g++ -O3 -DDBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_6_lambda_31.cpp -o ../bin/main
 * 
*/
#include <iostream>

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](const auto message, const auto debug) -> void {
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

void sample() {
    puts("=== sample");
    int x = 3;

    auto c1 = [x](int y) {return x * y > 55;};
    auto c2 = c1;
    auto c3 = c2;

    auto ret3 = c3(100);
    ptr_lambda_debug<const char*, const decltype(ret3)&>("ret3 is ", ret3);
    auto ret4 = c2(6);
    ptr_lambda_debug<const char*, const decltype(ret4)&>("ret4 is ", ret4);
}

/**
 * c1、c2、c3 はすべて、ラムダから生成したクロージャのコピーです。
 * 正式とは言いませんが、ラムダ、クロージャ、クロージャクラスの境界線を曖昧にしてもまったく問題ありません。しかし、以降の
 * 項目では、コンパイル時に存在するもの（ラムダとクロージャクラス）、実行時に存在するもの（クロージャ）を区別し、その関係性
 * の把握が重要になる場面が多くあります。
 * 
 * 項目 31 ：デフォルトのキャプチャモードは避ける
*/

int main(void) {
    puts("START 項目 31 ：デフォルトのキャプチャモードは避ける ===");
    if(0.01) {
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_debug_and_error());
    }
    if(1.00) {
        sample();
    }
    puts("=== 項目 31 ：デフォルトのキャプチャモードは避ける END");
    return 0;
}
