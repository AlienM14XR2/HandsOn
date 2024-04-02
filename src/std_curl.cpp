/**
 * 課題 curl
 * 
 * そのサンプリング。
 * 
 * curl の開発環境を整備する、必要なヘッダファイルやライブラリのインストール。
 * sudo apt install libcurl4-openssl-dev
 * 
 * @see /usr/include/x86_64-linux-gnu/curl/curl.h
 * dpkg -L libcurl4
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror std_curl.cpp -o ../bin/main
*/
#include <iostream>
#include <cassert>
#include <curl/curl.h>

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](const auto message, const auto debug) -> void {
    std::cout << "DEBUG: " << message << '\t' << debug << std::endl;
};

template <typename ERR>
concept ErrReasonable = requires(ERR& e) {
    e.what();
};
template <typename ERR>
requires ErrReasonable<ERR>
void (*ptr_print_error)(ERR) = [](const auto e) -> void {
    std::cout << "ERROR: " << e.what() << std::endl;
};

int test_debug_and_error() {
    puts("=== test_debug_and_error");
    try {
        double pi = 3.141592;
        printf("pi is \t%lf\n", pi);
        ptr_lambda_debug<const char*, const double&>("pi is ", pi);
        throw std::runtime_error("It's test error.");
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int main(void) {
    puts("START 課題 curl ===");
    if(0.01) {
        auto ret = 0;
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_debug_and_error());
        assert(ret == 1);
    }
    puts("===   課題 curl END");
    return 0;
}