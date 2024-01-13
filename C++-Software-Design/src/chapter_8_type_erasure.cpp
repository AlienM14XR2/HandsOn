/**
 * 8 章 Type Erasure パターン
 * 
 * ガイドライン 32 Type Erasure パターンでの継承階層の置換を検討する
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_8_type_erasure.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

int main(void) {
    puts("START Type Erasure パターンでの継承階層の置換を検討する ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const int&>("pi is ", pi);
    }
    puts("=== Type Erasure パターンでの継承階層の置換を検討する END");
    return 0;
}