/**
 * 8 章 Template Method パターン
 * 
 * アルゴリズムのカプセル化
 * 
 * e.g. compile
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_8_template_method.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template<class M, class D>
void (*ptr_lambda_debug)(M, D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

int main(void) {
    puts("START 8 章 Template Method パターン ===");
    puts("=== 8 章 Template Method パターン END");
    return 0;
}