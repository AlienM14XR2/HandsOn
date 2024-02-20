/**
 * Lost Chapter Concurrency
 * 
 * 個人的な疑問、不明点等を明らかにする。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror coffee_break_concurrency.cpp -o ../bin/main
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

/**
 * オブジェクトを複数のスレッドで利用しなければ、メンバ変数の const 修飾は必要はないはず。
 * オブジェクトとスレッドを 1 : 1 で管理する。
 * 
 * スレッドの同期はどのように行うのか
*/

int main(void) {
    puts("START Lost Chapter Concurrency ===");
    if(0.01) {
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_debug_and_error());
    }
    puts("=== Lost Chapter Concurrency END");
    return 0;
}