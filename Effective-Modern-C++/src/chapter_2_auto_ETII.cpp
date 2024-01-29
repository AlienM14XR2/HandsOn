/**
 * 2 章 auto
 * 
 * 項目 6 auto が期待値とは異なる型を推論する場面では ETII を用いる
 * 
 * ETII（Explicitly Typed Initializer Idiom）明示的型付け初期化子イディオム
 * 
 * e.g.)
 * auto highPriority = static_cast<bool>(feature(w)[5]);
 * 
 * e.g.) compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_2_auto_ETII.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](const auto message, const auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

int main(void) {
    puts("START 項目 6 auto が期待値とは異なる型を推論する場面では ETII を用いる ===");
    if(0.01) {
        auto pi = 3.141592;
        ptr_lambda_debug<const char*,const decltype(pi)&>("pi is ",pi);
    }
    puts("=== 項目 6 auto が期待値とは異なる型を推論する場面では ETII を用いる END");
    return 0;
}