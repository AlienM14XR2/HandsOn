/**
 * 6 章 ラムダ式
 * 
 * 項目 33 ：auto&& 仮引数を std::forward する場合は decltype を用いる
 * 
 * C++14 で最も興奮する機能の 1 つに汎用ラムダ（Generic Lambda）ー 仮引数指定に auto を記述できるラムダ ー があります。この機能の実装は
 * 直感的に分かりやすいものです。
 * 
 * 重要ポイント
 * - auto&& 仮引数を std::forward する場合は decltype を用いる
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

/**
 * 完全転送ラムダのサンプル（著者は完全転送に取り憑かれているのかな：）
*/

template <class T>
void normalize(T&& param) {
    puts("--- normalize");
    ptr_lambda_debug<const char*, const std::string&>("param's type is ", typeid(param).name());
}

int test_1() {
    puts("=== test_1");
    try {
        auto f = [](auto&& x) {
            return normalize(std::forward<decltype(x)>(x));
        };

        f(1);
        f("Alice");
        f(3.141592);
        std::string name("Derek");
        f(name);
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
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
    if(1.00) {
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_1());
    }
    puts("=== 項目 33 ：auto&& 仮引数を std::forward する場合は decltype を用いる END");
    return 0;
}