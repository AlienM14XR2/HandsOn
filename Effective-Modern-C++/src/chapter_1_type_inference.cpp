/**
 * 1 章 型推論
 * 
 * 項目 1 ：テンプレートの型推論を理解する
 * 
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_1_type_inference.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

int main(void) {
    puts("START 1 章 型推論 ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const double>("pi is ",pi);
    }
    puts("=== 1 章 型推論 END");
    return 0;
}