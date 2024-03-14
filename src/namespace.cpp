/**
 * namespace に関する疑問解決
 * 
 * - ヘッダ、ソースにおける書き方
 * 
 * ヘッダファイル（宣言）では namespace [TAG_NAME] { ... } と記述し
 * ソースファイル（定義）では [TAG_NAME]::[メンバ関数名] のように記述する。
 * 
 * 本来は何らかのプロジェクト単位、あるいはモジュール単位で使うべきなのだろうと思った。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror -I../inc/ Circle.cpp namespace.cpp -o ../bin/main
*/
#include <iostream>
#include <cassert>
#include <memory>
#include "Circle.hpp"

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






int test_Circle() {
    puts("=== test_Circle");
    try {
        std::unique_ptr<shape::Shape> shape = std::make_unique<shape::Circle>();
        shape->draw();
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int main(void) {
    puts("=== START namespace に関する疑問解決");
    if(0.01) {
        auto ret = 0;
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", ret = test_debug_and_error());
        assert(ret == 1);
    }
    if(1.00) {
        auto ret = 0;
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", ret = test_Circle());
        assert(ret == 0);
    }
    puts("namespace に関する疑問解決 END   ===");
}