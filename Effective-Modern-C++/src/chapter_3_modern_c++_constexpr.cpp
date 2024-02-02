/**
 * 3 章 現代の C++ への移行
 * 
 * 項目 15 ：可能な場面では常に constexpr を用いる
 * 
 * C++11 で最も混乱する新用語を表彰すれば、恐らく constexpr が受賞するでしょう。
 * オブジェクトに使用すれば本質的な意味は強化された const ですが、関数に使用した場合の意味は大きく異なります。
 * この混乱はくぐり抜ける価値があります。表現したい内容が constexpr にふさわしい場面では、間違いなく使用した
 * くなるでしょう。
 * 
 * constexpr は、その値が単なる定数というよりも、コンパイル時に既知の定数であることを概念的に表現します。
 * この基礎概念から出発し、関数に対して constexpr を用いた場合は他にも多くのニュアンスを含みます。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_3_modern_c++_constexpr.cpp -o ../bin/main
*/
#include <iostream>
#include <array>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](const auto message, const auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

int test_debug() {
    puts("--- test_debug");
    try {
        auto pi = 3.141592;
        ptr_lambda_debug<const char*,const decltype(pi)&>("pi is ", pi);
        ptr_lambda_debug<const char*,const decltype(pi)*>("pi addr is ", &pi);
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << "ERROR: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}

/**
 * 配列の要素数をコンパイル時定数として返す（要素数のみを考慮するため、仮引数の配列に名前はない）。
*/
template <class T, std::size_t N>
constexpr std::size_t arraySize(T(&)[N]) noexcept {
    return N;
}

int test_arraySize() {
    puts("--- test_arraySize");
    try {
        int keyVals[] = {1,3,7,9,11,22,35};
        int mappedVals[arraySize(keyVals)];
        size_t mappedValsSize = sizeof(mappedVals) / sizeof(mappedVals[0]);
        ptr_lambda_debug<const char*,const size_t&>("mappedValsSize is ", mappedValsSize);
        // もちろん、現代の C++ 開発者ならば、組み込み配列よりも std::array の方が当然好みでしょう。
        std::array<int, arraySize(keyVals)> mappedVals2;
        ptr_lambda_debug<const char*,const size_t&>("mappedVals2 size is ", mappedVals2.size());
        
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

/**
 * 1 章ですでに上記のようなサンプルがあった。
*/

int main(void) {
    puts("START 項目 15 ：可能な場面では常に constexpr を用いる ===");
    if(0.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_debug());
    }
    puts("=== 項目 15 ：可能な場面では常に constexpr を用いる END");
}