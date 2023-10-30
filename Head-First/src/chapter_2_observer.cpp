/**
 * 2 章 Observer パターン
 * 
 * 直訳で観察者を意味する。
 * 
 * ```
 * e.g. コンパイル
 * g++ -O3 -std=c++20 -pedantic-errors -Wall -Werror chapter_2_observer.cpp -o ../bin/main
 * ```
*/
#include <iostream>

using namespace std;

template <class M, class D>
void (*ptr_labmda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " + message + '\t' + debug << endl;
};

int main(void) {
    puts(" START ========= 2 章 Observer パターン");
    puts("2 章 Observer パターン ========= END");
    return 0;
}