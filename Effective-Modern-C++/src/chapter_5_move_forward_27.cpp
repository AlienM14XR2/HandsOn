/**
 * 5 章 右辺値参照、ムーブセマンティクス、完全転送
 * 
 * 項目 27 ：転送参照をとるオーバーロードの代替策を把握する
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_5_move_forward_27.cpp -o ../bin/main
*/
#include <iostream>

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

/**
 * 本項は特にコーディングは少ない、あるいはない可能性もある。
 * 
 * 『オーバーロードしない』
 * 
 * 『const T& を渡す』
 * 
 * 『値を渡す』
 * 
 * 『タグディスパッチを用いる』
 * 値渡しも、const オブジェクトの左辺値参照渡しも、完全転送には対応していません。転送参照を用いた目的が完全転送にあるならば、
 * 転送参照を使用せざるを得ません。他には選択はありません。しかし、それでもオーバーロードは諦めたくありません。さて、オーバー
 * ロードも転送参照も諦めずに転送参照をとるオーバーロードにまつわる問題を回避する方法はあるでしょうか？
 * 
*/

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
    puts("START 項目 27 ：転送参照をとるオーバーロードの代替策を把握する ===");
    if(0.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_debug_and_error());
    }
    puts("=== 項目 27 ：転送参照をとるオーバーロードの代替策を把握する END");
    return 0;
}