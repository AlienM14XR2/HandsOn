/**
 * 4 章 スマートポインタ
 * 
 * 項目 19 ：共有するリソースの管理には std::shared_ptr を用いる
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_4_smart_pointer_shared_ptr.cpp -o ../bin/main
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
    puts("START 項目 19 ：共有するリソースの管理には std::shared_ptr を用いる ===");
    puts("=== 項目 19 ：共有するリソースの管理には std::shared_ptr を用いる END");
    return 0;
}