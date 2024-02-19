/**
 * 6 章 ラムダ式
 * 
 * 項目 33 ：auto&& 仮引数を std::forward する場合は decltype を用いる
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_6_lambda_33.cpp -o ../bin/main
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
    puts("START 項目 33 ：auto&& 仮引数を std::forward する場合は decltype を用いる ===");
    if(0.01) {
        int ret = 0;
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", ret = test_debug_and_error());
        assert(ret == 1);   // エラーが発生したことを期待している。
    }
    puts("=== 項目 33 ：auto&& 仮引数を std::forward する場合は decltype を用いる END");
    return 0;
}