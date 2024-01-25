/**
 * 1 章 型推論
 * 
 * 項目 2 ：auto 型推論を理解する
 * 
 * 項目 1 のテンプレートの型推論を読んでいれば、auto の型推論についてもすでにほぼすべてを把握していることになります。
 * やや奇異にも見える一点だけを除き、auto の型推論はテンプレートの型推論と『同一』です。
 * しかしそんなことが可能でしょうか？テンプレートの型推論では、テンプレート、関数、仮引数が対象ですが、auto ではそん
 * なものは対象としません。
 * 
 * 対象としないのはその通りですが、問題にはなりません。テンプレートの型推論と auto の型推論は直接的に対応しており、
 * 機械的に字面を置き換えるだけのことです。
 * 
 * e.g. ) compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_1_type_inference_auto.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

void sample_auto() {
    puts("--- sample_auto");
    auto x = 27;            // x の型指定子は auto のみです。
    const auto cx = x;      // cx の型指定子は const auto です。
    const auto& rx = x;     // rx の型指定子は const auto& です。

    /**
     * コンパイラはそれぞれの宣言につきテンプレートが存在し、さらにテンプレートに初期化式を与えるコード
     * も存在するものとして処理します。
    */

    ptr_lambda_debug<const char*,const int*>("x addr is ", &x);
    ptr_lambda_debug<const char*,const int*>("cx addr is ", &cx);
    ptr_lambda_debug<const char*,const int*>("rx addr is ", &rx);
}

/**
 * 
*/

int main(void) {
    puts("START 項目 2 ：auto 型推論を理解する ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const double&>("pi is ", pi);
    }
    if(1.00) {
        sample_auto();
    }
    puts("=== 項目 2 ：auto 型推論を理解する END");
    return 0;
}