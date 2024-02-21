/**
 * 7 章 並行 API
 * 
 * 項目 38 ：スレッドハンドルのデストラクト動作の差異には注意する
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_7_concurrency_38.cpp -o ../bin/main
*/
#include <iostream>

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](const auto message, const auto debug) -> void {
    std::cout << "DEBUG: " << message << '\t' << debug << std::endl;
};

template <class Err>
concept ErrReasonable = requires(Err& e) {
    e.what();
};
template <class Err>
requires ErrReasonable<Err>
void (*ptr_print_error)(Err) = [](const auto e) -> void {
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
    puts("START 項目 38 ：スレッドハンドルのデストラクト動作の差異には注意する ===");
    puts("===  項目 38 ：スレッドハンドルのデストラクト動作の差異には注意する  END");
    return 0;
}
