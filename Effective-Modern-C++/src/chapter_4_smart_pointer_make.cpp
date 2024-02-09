/**
 * 4 章 スマートポインタ
 * 
 * 項目 21 ：new の直接使用よりも std::make_unique や std::make_shared を優先する
 * 
 * std::make_shared は C++11 の一部となっていますが、std::make_unique は C++14 から標準ライブラリに加えられたものです。
 * もし、C++11 しか使えなくても std::make_unique の基本バージョンは自作が容易ですから。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_4_smart_pointer_make.cpp -o ../bin/main
*/
#include <iostream>
#include <memory>

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

/**
 * std::make_shared は C++11 の一部となっていますが、std::make_unique は C++14 から標準ライブラリに加えられたものです。
 * もし、C++11 しか使えなくても std::make_unique の基本バージョンは自作が容易ですから。
*/

template <class T, class... Args>
std::unique_ptr<T> make_unique(Args&&... params) {
    return std::unique_ptr<T>(new T(std::forward<Args>(params)...));
}

int test_my_make_unique() {
    puts("=== test_my_make_unique");
    try {
        std::unique_ptr<int> ip = make_unique<int>(99);
        ptr_lambda_debug<const char*,const int&>("ip val is ", *ip.get()); 
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

class Widget {

};

void sample() {
    auto upw1(std::make_unique<Widget>());          // make 関数を使用
    ptr_lambda_debug<const char*,const Widget*>("upw1 addr is ", upw1.get());
    
    std::unique_ptr<Widget> upw2(new Widget());     // make 未使用
    ptr_lambda_debug<const char*,const Widget*>("upw2 addr is ", upw2.get());

    auto spw1(std::make_shared<Widget>());          // make 関数を使用
    ptr_lambda_debug<const char*,const Widget*>("spw1 addr is ", spw1.get());

    std::shared_ptr<Widget> spw2(new Widget());      // make 未使用
    ptr_lambda_debug<const char*,const Widget*>("spw2 addr is ", spw2.get());

    /**
     * new を用いたバージョンでは、作成する型を 2 度記述していますが、make 関数を使用するバージョンでは 1 度です。
     * 型を繰り返し記述することは、ソフトウェア工学での重要な信条、コードの重複を避けよ、に反します。
    */
}

/**
 * make 関数が望ましい理由の 2 つ目は、例外安全性への対応です。何らかの優先度を考慮しつつ Widget を処理する関数
 * を考えてみましょう。
*/

int main(void) {
    puts("START 項目 21 ：new の直接使用よりも std::make_unique や std::make_shared を優先する ===");
    if(0.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_debug());
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_my_make_unique());
        sample();
    }
    puts("=== 項目 21 ：new の直接使用よりも std::make_unique や std::make_shared を優先する END");
    return 0;
}