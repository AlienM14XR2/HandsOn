/**
 * 3 章 現代の C++ への移行
 * 
 * 項目 14 ：例外を発生させない関数は noexcept と宣言する
 * 
 * 重要ポイント
 * - noexcept は関数インタフェースの一部であり、呼び出し側に依存性が生まれる。
 * - noexcept と宣言した関数は、そう宣言しない関数と比較し、より高度な最適化が可能になる。
 * - noexcept はムーブ演算、swap、メモリ開放関数、デストラクタに、特に効果的である。
 * - 大多数の関数は noexcept ではなく、例外中立である。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_3_modern_c++_noexcept.cpp -o ../bin/main
*/
#include <iostream>

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
 * RetType function(params) noexcept;   // C++11 最高度に最適化可能
 * 
 * RetType function(params) throw();    // C++98 ほとんど最適化できない、コールスタックの巻き戻しを考慮している。
 * 
 * RetType function(params);            // ほとんど最適化できない、コールスタックの巻き戻しを考慮している。
 * 
 * これだけでも、例外を発生させないことがわかっている関数には、常に noexcept と宣言充分な理由になるでしょう。
 * この状況は一部の関数に顕著に現れます。ムーブ演算はその最たる例です。
 * 
*/



int main(void) {
    puts("START 項目 14 ：例外を発生させない関数は noexcept と宣言する ===");
    if(0.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_debug());
    }
    puts("=== 項目 14 ：例外を発生させない関数は noexcept と宣言する END");
}