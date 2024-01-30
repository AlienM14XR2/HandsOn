/**
 * 3 章 現代の C++ への移行
 * 
 * 項目 8 ：0 や NULL よりも nullptr を優先する
 * 
 * ここで問題なのは、NULL の厳密な型ではなく、0 も NULL もポインタ型ではないということです。
 * ヌルポインタを表現したい場合は、0 や NULL ではなく nullptr を使用すること。
 * 
 * 重要ポイント
 * - 0 や NULL よりも nullptr を優先する。
 * - 汎整数型とポインタ型のオーバーロードは避ける。
 * 
 * e.g.) compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_3_modern_c++_nullptr.cpp -o ../bin/main
*/
#include <iostream>
#include <memory>
#include <mutex>

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
    puts("--- 次は実引数に nullptr を指定している");
    f(nullptr);

}

/**
 * テンプレートでは nullptr はさらに輝きを増す。
*/

class Widget;

int    f1( std::shared_ptr<Widget> spw ) {
    return 0;
}

double f2( std::unique_ptr<Widget> upw ) {
    return 3.33;
}

bool   f3( Widget* pw ) {
    return true;
}


template< class FuncType
          , class MutexType
          , class PtrType >
decltype(auto) lockAndCall( FuncType func, MutexType& mutex, PtrType ptr ) {
    using mutexGuard = std::lock_guard<MutexType>;
    mutexGuard g(mutex);
    return func(ptr);
}

void sample2() {
    puts("--- sample2");
    std::mutex f1m, f2m, f3m;

    // auto result1 = lockAndCall(f1, f1m, 0);         // これは int と判定され、コンパイルエラーになる

    // auto result2 = lockAndCall(f2, f2m, NULL);      // 私の環境では long int と判定され、コンパイルエラーになる

    auto result3 = lockAndCall(f3, f3m, nullptr);
    ptr_lambda_debug<const char*,const decltype(result3)&>("result3 is ", result3);
}


int main(void) {
    puts("START 項目 8 ：0 や NULL よりも nullptr を優先する ===");
    if(0.01) {
        auto pi = 3.141592;
        ptr_lambda_debug<const char*,const decltype(pi)&>("pi is ", pi);
    }
    if(1.00) {
        sample();
        sample2();
    }
    puts("=== 項目 8 ：0 や NULL よりも nullptr を優先する END");
    return 0;
}