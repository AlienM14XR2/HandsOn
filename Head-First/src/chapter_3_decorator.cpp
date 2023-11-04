/**
 * 3 章 Decorator
 * 
 * g++ -O3 -std=c++20 -pedantic-errors -Wall -Werror chapter_3_decorator.cpp -o ../bin/main
 * 
 * オブジェクトの装飾
*/
#include <iostream>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

int main(void) {
    puts("START 3 章 Decorator =========");
    if(1) {
        double pi = 3.14159;
        ptr_lambda_debug<const char*, const double&>("pi is ",pi);
    }
    puts("========= END 3 章 Decorator ");
    return 0;
}