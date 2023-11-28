/**
 * 9 章 Iterator パターンと Composite パターン
 * 
 * 適切に管理されたコレクション
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_9_iterator.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

int main(void) {
    puts("START 9 章 Iterator パターン ===");
    if(0.1) {
        double pi = 3.14159;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    puts("=== 9 章 Iterator パターン END");
    return 0;
}