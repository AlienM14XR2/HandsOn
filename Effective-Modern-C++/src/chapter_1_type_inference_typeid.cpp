/**
 * 1 章 型推論
 * 
 * 項目 4 ：推論された型を確認する
 * 
 * 推論した型を表示、確認する方法は複数あり、開発プロセスの段階に応じて変化します。ここでは 3 種類の
 * 方法を取り上げます。
 * - コーディング時に型推論情報を得る。
 * - コンパイル時に得る。
 * - 実行時に得る。
 * 
 * e.g. ) compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_1_type_inference_typeid.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template <class Message, class Debug>
void (*ptr_lambda_debug)(Message, Debug) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

int main(void) {
    puts("START 項目 4 ：推論された型を確認する ===");
    if(0.01) {
        // 覚えたことは直ぐに試してみる。
        auto pi = 3.141592;
        ptr_lambda_debug<const char*,const decltype(pi)&>("pi is ",pi);
    }
    puts("=== 項目 4 ：推論された型を確認する END");
    return 0;
}