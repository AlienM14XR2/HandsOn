/**
 * 5 章 右辺値参照、ムーブセマンティクス、完全転送
 * 
 * 項目 26 ：転送参照をとるオーバーロードは避ける
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_5_move_forward_26.cpp -o ../bin/main
*/
#include <iostream>
#include <chrono>

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](const auto message, const auto debug) -> void {
    std::cout << "DEBUG: " << message << "\t" << debug << std::endl;
};

template <class Error>
concept ErrReasonable = requires(Error& e) {            // このコンセプトはやはりあまり意味がないのかもな。
    e.what();
};
template <class Error>
requires ErrReasonable<Error>
void (*ptr_print_error)(Error) = [](const auto e) {
    std::cout << "ERROR: " << e.what() << std::endl;
};

void (*ptr_print_now)(void) = [](void) -> void {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::cout << std::ctime(&t) << std::endl;
};

int test_debug_and_error() {
    puts("=== test_debug_and_error");
    try {
        ptr_print_now();
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
    puts("START 項目 26 ：転送参照をとるオーバーロードは避ける ===");
    if(0.01) {
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_debug_and_error());
    }
    puts("=== 項目 26 ：転送参照をとるオーバーロードは避ける END");
    return 0;
}