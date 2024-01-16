/**
 * 9 章 Decorator パターン
 * 
 * ガイドライン 35 ：カスタマイズを階層化するには Decorator パターン
 * 
 * 解明 Decorator パターン
 * Decorator パターンもやはり GoF 本に収録されているもので、異なる種類の機能をコンポジションにより
 * 柔軟に組み合わせることを主眼に置いています。
 * 
 * Decorator パターン
 * 目的：オブジェクトに責任を動的に追加する。Decorator パターンは、サブクラス化よりも
 *       柔軟な機能拡張方法を提供する。
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

/**
 * 商品売買管理システムをリファクタリングする形で、書籍は説明するようだ。
 * まずは、Decorator パターンの古典的実装から行う。
*/

int main(void) {
    puts("START カスタマイズを階層化するには Decorator パターン ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    puts("=== カスタマイズを階層化するには Decorator パターン END");
    return 0;
}