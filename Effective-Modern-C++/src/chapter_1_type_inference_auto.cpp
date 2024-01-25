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
 * auto を用いた変数宣言では、型指定子が ParamType に相当し、やはり 3 種類に分けられます
 * - ケース 1 ：型指定子が参照もしくはポインタだが、ユニバーサル参照ではない。
 * - ケース 2 ：型指定子がユニバーサル参照である。
 * - ケース 3 ：型指定子がポインタでも参照でもない。
 * 
 * e.g. 関数テンプレートの場合。
 * 
 * template <class T>
 * void f(ParamType param)
 * 
 * f(expr)                  // call with some expression ... f に式を与え呼び出す。
*/

void sample_auto2() {
    puts("--- sample_auto2");
    auto x = 27;            // ケース 3 （x はポインタでも参照でもない）
    const auto cx = x;      // ケース 3 （cx はポインタでも参照でもない）
    const auto& rx = x;     // ケース 1 （rx は参照であり、ユニバーサル参照ではない）
    ptr_lambda_debug<const char*,const int&>("x is ",x);
    ptr_lambda_debug<const char*,const int&>("cx is ",cx);
    ptr_lambda_debug<const char*,const int&>("rx is ",rx);

    // 次はすべて ケース 2
    auto&& uref1 = x;       // ケース 2 （x は int かつ左辺値のため、 uref の型は int&）
    auto&& uref2 = cx;      // ケース 2 （cx は const int かつ左辺値のため、 uref2 の型は const int&）
    auto&& uref3 = 27;      // ケース 2 （27 は int かつ右辺値のため、uref3 の型は int&&）
    // 私の現在の理解では「ユニバーサル参照」は数値リテラルなどの、右辺値と通常の変数である左辺値を同様に扱えるというものなのだが、あってるのかな。
    ptr_lambda_debug<const char*,const int&>("uref1 is ",uref1);
    ptr_lambda_debug<const char*,const int&>("uref2 is ",uref2);
    ptr_lambda_debug<const char*,const int&>("uref3 is ",uref3);
    ptr_lambda_debug<const char*,const int*>("uref1 addr is ",&uref1);
    ptr_lambda_debug<const char*,const int*>("uref2 addr is ",&uref2);
    ptr_lambda_debug<const char*,const int*>("uref3 addr is ",&uref3);
}

int main(void) {
    puts("START 項目 2 ：auto 型推論を理解する ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const double&>("pi is ", pi);
    }
    if(1.00) {
        sample_auto();
        sample_auto2();
    }
    puts("=== 項目 2 ：auto 型推論を理解する END");
    return 0;
}