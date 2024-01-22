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

/**
 * ケース 1 ：ParamType が参照もしくはポインタだが、ユニバーサル参照ではない
 * 
 * e.g. 関数テンプレートの場合。
 * 
 * template <class T>
 * void f(ParamType param)
 * 
 * f(expr)                  // call with some expression ... f に式を与え呼び出す。
*/

template <class T>
void f(T& param) {      // const なし
    puts("------ f");
    // param += 1;      // コンパイルエラー、 param is read-only reference
    ptr_lambda_debug<const char*,const T&>("param is ",param);
}

int test_f() {
    puts("--- test_f");
    try {
        int x = 27;             // x  は int
        const int cx = x;       // cx は const int
        const int& rx = x;      // rx は const int としての x の参照

        f(x);                   // T は int, param の型は int&
        f(cx);                  // T は const int, param の型は int&
        f(rx);                  // T は const int, param の型は int&
        /**
         * 正誤表より補足だと：）
         * 上段落はまること削除だと（オイ：）
        */
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

template <class T>
void f2(const T& param) {       // const あり
    puts("------ f2");
    // param += 1;      // コンパイルエラー、 param is read-only reference
    ptr_lambda_debug<const char*,const T&>("param is ",param);
}

int test_f2() {
    puts("--- test_f2");
    try {
        int x = 27;             // x  は int
        const int cx = x;       // cx は const int
        const int& rx = x;      // rx は const int としての x の参照

        f2(x);                  // T は int, param の型は const int&
        f2(cx);                 // T は int, param の型は int&
        f2(rx);                 // T は int, param の型は int&

        /**
         * 先の例と同様に、型推論では rx の参照性は無視されます。
         * 仮に param が参照ではなくポインタだったとしても（または const を指すポインタ）、基本的には
         * 同様に推論されます。
        */

        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

template <class T>
void fp(T* param) {
    puts("------ fp");
    ptr_lambda_debug<const char*,const T&>("param is ",*param);
}

int test_fp() {
    puts("--- fp");
    try {
        int x = 27;             // x  は int
        const int* px = &x;     // 先の例と変わらず、px は const int としての x を指す

        fp(&x);                 // T は int, param の型は int*
        fp(px);                 // T は const int, param の型は const int*
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

/**
 * C++ の型推論は、参照やポインタの仮引数に対してごく自然に動作するため、改めて言葉で
 * 説明されても退屈で居眠りをしてしまったかもしれません。すべて自明で分かりきったことです！
 * 型推論システムは期待通りに動作します。
*/

int main(void) {
    puts("START 1 章 型推論 ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const double>("pi is ",pi);
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_f());
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_f2());
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_fp());
    }
    puts("=== 1 章 型推論 END");
    return 0;
}