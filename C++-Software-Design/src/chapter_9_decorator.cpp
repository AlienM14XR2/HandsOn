/**
 * 9 章 Decorator パターン
 * 
 * ガイドライン 35 ：カスタマイズを階層化するには Decorator パターン
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_9_decorator.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template <class Message, class Debug>
void (*ptr_lambda_debug)(Message, Debug) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

int main(void) {
    puts("START カスタマイズを階層化するには Decorator パターン ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    puts("=== カスタマイズを階層化するには Decorator パターン END");
    return 0;
}