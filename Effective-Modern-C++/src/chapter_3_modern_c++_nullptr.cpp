/**
 * 3 章 現代の C++ への移行
 * 
 * 項目 8 ：0 や NULL よりも nullptr を優先する
 * 
 * ここで問題なのは、NULL の厳密な型ではなく、0 も NULL もポインタ型ではないということです。
 * 
 * e.g.) compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_3_modern_c++_nullptr.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](const auto message, const auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

void f(int x) {
    puts("------ f(int)");
    printf("x is %d\n",x);
}
void f(bool b) {
    puts("------ f(bool)");
    printf("b is %d\n",b);
}
void f(void* p) {
    puts("------ f(void*)");
    printf("p is %p\n",p);
    ptr_lambda_debug<const char*,const void*>("p is ",p);
}

void sample() {
    puts("--- sample");
    f(0);
    f(true);
    // f(NULL);    // 複数の Overload された関数とヒットして、コンパイルできない。
    int a = 3;
    int* pa = &a;
    ptr_lambda_debug<const char*,const int*>("pa is ",pa);
    f((void*)pa);
}

int main(void) {
    puts("START 項目 8 ：0 や NULL よりも nullptr を優先する ===");
    if(0.01) {
        auto pi = 3.141592;
        ptr_lambda_debug<const char*,const decltype(pi)&>("pi is ", pi);
    }
    if(1.00) {
        sample();
    }
    puts("=== 項目 8 ：0 や NULL よりも nullptr を優先する END");
    return 0;
}