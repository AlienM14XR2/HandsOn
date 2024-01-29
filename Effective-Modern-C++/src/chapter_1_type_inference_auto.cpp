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
 * 重要ポイント
 * - auto の型推論は通常はテンプレートのそれと同様だが、auto では波括弧で囲んだ初期化子を std::initializer_list と想定する点が異なる。
 * - 関数の戻り値型やラムダ式の仮引数での auto はテンプレートの型推論と同じ動作となり、auto の型推論とは異なる。
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

/**
 * 項目 1 の最後では、配列および関数名が非参照型指定子のポインタに成り下がることを述べましたが、同様のことが
 * auto の型推論でも発生します。
*/

void someFunc(int lhs, double rhs) {    // someFunc は関数、型は void(int,double)
    puts("------ someFunc");
    ptr_lambda_debug<const char*, const int&>("lhs + rhs = ", lhs + rhs);
}

void sample_auto3() {
    puts("--- sample_auto3");
    const char name[] = "R. N. Briggs";     // name の型は const char[13]

    auto arr1 = name;       // arr1 の型は const char*
    auto& arr2 = name;      // arr2 の型は const char(&)[13]
    ptr_lambda_debug<const char*,const char*>("arr1 is ", arr1);
    ptr_lambda_debug<const char*,const char*>("arr2 is ", arr2);

    auto func1 = someFunc;  // func1 の型は void (*)(int,double)
    func1(3,6.6);

    auto& func2 = someFunc; // func2 の型は void (&)(int,double)
    func2(3,9.9);
}

/**
 * 上例が示す通り、auto の型推論はテンプレートの場合と同様に動作します。
 * いわば一枚のコインの表裏です。
 * 
 * しかし、異なる動作を示す場面が 1 つだけあります。初期値を 27 とする int を宣言する場合を
 * 考えてみましょう。C++98 では 2 通りの記述があります。
*/

template <class T>
void f_list(std::initializer_list<T> initList) {
    for(T t: initList) {
        ptr_lambda_debug<const char*,const T&>("t is ",t);
    }
}

void sample_auto4() {
    puts("--- sample_auto4");
    // C++98 では 2 通りの記述があります。
    int x1 = 27;
    int x2(27);
    ptr_lambda_debug<const char*,const int&>("x1 is ", x1);
    ptr_lambda_debug<const char*,const int&>("x2 is ", x2);

    // C++11 では初期化の統一記法を採用したため、次の記述も可能です。
    int x3 = {27};
    int x4{27};
    ptr_lambda_debug<const char*,const int&>("x3 is ", x3);
    ptr_lambda_debug<const char*,const int&>("x4 is ", x4);
    // 記述は 4 通りでも結果は 1 つしかありません。値を 27 とする int です。

    // 項目 5 で述べるように auto は型を明示した変数宣言より利点が多く、上例の変数宣言 int は
    // auto に置き換えるのが良いでしょう。単純に字面を置き換えれば次のようになります。

    auto x10 = 27;          // 型は int、値は 27
    auto x20{27};           // 型は int、値は 27
    auto x30 = {27};        // これは、配列や array の初期化記法ではなかったかな？ ... 型は std::initializer_list<int> 値は {27}
    auto x40{27};           // 型は int、値は 27
    ptr_lambda_debug<const char*,const int&>("x10 is ", x10);
    ptr_lambda_debug<const char*,const int&>("x20 is ", x20);
    // ptr_lambda_debug<const char*,const int&>("x30 is ", x30);       // error: invalid initialization of reference of type ‘const int&’ from expression of type ‘std::initializer_list<int>’
    ptr_lambda_debug<const char*,const int*>("x30 is ", x30.begin());
    for(int n: x30) {
        printf("n is %d\n",n);
    }
    ptr_lambda_debug<const char*,const int&>("x40 is ", x40);

    auto x50 = {11, 23, 9}; // 型は std::initializer_list<int>
    for(int n: x50) {
        printf("n is %d\n",n);
    }

    f_list({11,23,9});      // T は int と initList は std::initializer_list<int> と推察される。

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
        sample_auto3();
        sample_auto4();
    }
    puts("=== 項目 2 ：auto 型推論を理解する END");
    return 0;
}