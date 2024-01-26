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

/**
 * IDE のエディタ
 * 
 * IDE のエディタは、マウスカーソルを乗せるだけでプログラムエンティティ（変数、仮引数、関数など）の型を表示する機能を備えているのが一般的です。
 * 例えば次のようなコードがあるとしましょう。
*/

void sample() {
    const int theAnswer = 42;
    auto x = theAnswer;     // IDE のエディタは恐らく、x に推論した型を int と表示するはず。
    auto y = &theAnswer;    // y に推論した型は、const int* と表示するはず。

    ptr_lambda_debug<const char*,decltype(x)>("x is ", x);
    ptr_lambda_debug<const char*,decltype(y)>("y addr is ", y);     // 下記参照。
    ptr_lambda_debug<const char*,const int*>("y addr is ", y);      // 上記は、左記のように判断されているということ。
    ptr_lambda_debug<const char*,const int&>("y is ", *y);
}

/**
 * int のような単純な（組み込み）型では、IDE が表示する情報は通常問題ありません。しかし次の節
 * で述べるように複雑な型が登場してくると、IDE が表示する情報がそれほど有用とは言えなくなるでしょう。
 * 
 * コンパイラによる診断情報
 * 
 * コンパイラにその推論した型を表示させる上手な方法は、わざとコンパイルエラーを起こさせることです。
 * エラーメッセージにはその原因となった型の情報が、まず間違いなく含まれています。
*/

int main(void) {
    puts("START 項目 4 ：推論された型を確認する ===");
    if(0.01) {
        // 覚えたことは直ぐに試してみる。
        auto pi = 3.141592;
        ptr_lambda_debug<const char*,const decltype(pi)&>("pi is ",pi);
    }
    if(1.00) {
        sample();
    }
    puts("=== 項目 4 ：推論された型を確認する END");
    return 0;
}