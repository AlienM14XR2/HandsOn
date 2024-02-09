/**
 * 4 章 スマートポインタ
 * 
 * 項目 20 ：不正ポインタになり得る std::shared_ptr ライクなポインタには std::weak_ptr を用いる
 * 
 * 逆説的に聞こえますが、std::shared_ptr のように振る舞いながらも、『対象リソースの所有権を共有しない』スマートポインタが有用に
 * なる場面もあります。つまり、オブジェクトのレファレンスカウントに影響しない std::shared_ptr ライクなポインタです。
 * std::shared_ptr では無縁の問題に対峙しなければなりません。ポインタが指す対象が破棄される問題です。真にスマートなポインタならば、
 * ポインタが『不正』になったことを追跡管理し、この問題に対応するでしょう。不正なポインタとは、ポインタが指すオブジェクトが存在し
 * ない状態を言います。この問題こそまさに std::weak_ptr というスマートポインタが対処するものです。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_4_smart_pointer_weak_ptr.cpp -o ../bin/main
*/
#include <iostream>

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](const auto message, const auto debug) -> void {
    std::cout << "DEBUG: " << message << '\t' << debug << std::endl;
};

template <class Error>
concept Reasonable = requires(Error& e) {
    e.what();
};

template <class Error>
requires Reasonable<Error>
void (*ptr_print_error)(Error) = [](const auto e) -> void {
    std::cout << "ERROR: " << e.what() << std::endl;
};

int test_debug() {
    puts("=== test_debug");
    try {
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
    puts("START 不正ポインタになり得る std::shared_ptr ライクなポインタには std::weak_ptr を用いる ===");
    if(0.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_debug());
    }
    puts("=== 不正ポインタになり得る std::shared_ptr ライクなポインタには std::weak_ptr を用いる END");
    return 0;
}