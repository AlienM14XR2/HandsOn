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
 * 重要ポイント
 * - 推論された型は、通常は、IED のエディタ、コンパイラのエラーメッセージ、Boost の TypeIndex ライブラリを用いると確認できる。
 * - 一部の方法では、表示される型が正確でも有用でもない。そのため、C++ の型推論規則に対する理解が必要不可欠であることは変わらない。
 * 
 * e.g. ) compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_1_type_inference_typeid.cpp -o ../bin/main
*/
#include <iostream>
#include <vector>

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
    puts("--- sample");
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

/**
 * 実行時出力
 * 
 * printf を用いた方法では、実行時まで結果がわかりません（printf を推奨している訳ではないが）。
 * しかし、この方法は情報の書式を完全に制御できる利点があります。重要となるのは、目的の型の、
 * 表示に適したテクスト表現です。「そんなに大変じゃない」と読者は思うかもしれません。
 * 「typeid と std::type_info::name でいいんじゃないか」と。
*/

void sample2() {
    puts("--- sample2");
    const int theAnswer = 42;
    auto x = theAnswer;
    auto y = &theAnswer;

    cout << typeid(x).name() << endl;
    cout << typeid(y).name() << endl;
    /**
     * 処理系は std::type_info::name が意味がある内容を返すよう努めますが、その保証はありません。
     * GNU 及び Clang コンパイラでは、x の型を「i」、y の型を「PKi」と表示します。
     * 意味が分かればこの表示も納得できます。
     * 「i」は「int」を、「PKi」は「Pointer to （Konst）const 」を意味します。
     * （両コンパイラともこの種の「変形された」型を復元するツール c++filt に対応している）。
     * 次はもう少し複雑な例を考えてみましょう。
    */
}

class Widget {
private:
    double x, y;
public:
    explicit Widget() {}
    explicit Widget(double _x, double _y): x{_x}, y{_y}
    {}
    Widget(const Widget& own) {*this = own;}
    ~Widget() {}
    // ...
    double& getX() noexcept { return x; }
    double& getY() noexcept { return y; }
};

std::vector<Widget> createVec() {
    std::vector<Widget> vec{Widget{0.0, 0.0}, Widget{3.0,6.0}, Widget{9.9, 21.3}};
    return vec;
}

template <class T>
void f(const T& param) {
    puts("------ f");
    cout << "T \t= \t" << typeid(T).name() << endl; 
    cout << "param \t= \t" << typeid(param).name() << endl; 
}

int test_Widget() {
    puts("--- test_Widget");
    try {
        std::vector<Widget> vec = createVec();
        for(Widget w: vec) {
            ptr_lambda_debug<const char*,const double&>("x is ", w.getX());
            ptr_lambda_debug<const char*,const double&>("y is ", w.getY());
        }

        // ここからが書籍の本題。
        const auto vec2 = createVec();
        if(!vec2.empty()) {
            f(&vec2[0]);
            // ...
            /**
             * GNU および Clang コンパイラにより作成した実行ファイルは、次のように表示します。
             * T     = PK6Widget
             * param = PK6Widget
             * 
             * PK は「Pointer to const」を意味することは分かっていますので、残る謎は 6 という数字です。これは
             * 単に対象クラス名の文字数を表しています（Widget）。
             * 最終的に T も param も const Widget* という型であると言っています。
             * 
             * ここで問題があります。
             * f というテンプレート関数の param に宣言した型は const T& です。それなのに T と param が同じ型というのはおかしくありません？
             * T を int だとすると、param の型は const int& となるべきで、まったく異なる型のはずです。
             * 
             * 残念ながら std::type_info::name が返す型は信頼できません。（個人的にはこれで十分なんだけど：）
             * これは仕様で誤ることが本質的に『求められている』のです。
             * 項目 1 で述べたように、型が参照だった場合はその参照性は無視され、さらには const もその意味を失うのです（volatile も）。
             * const Widget* const & が、const Widget* と表示される理由です。
             * まず、型が持つ参照性が落とされ、次にポインタが持つ const 性が落とされるのです。
             * 
             * これは IDE でも同じです。
             * 
             * boost/type_index.hpp
             * が非常によくできていて、上記を解決している。
            */
        }
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

int main(void) {
    puts("START 項目 4 ：推論された型を確認する ===");
    if(0.01) {
        // 覚えたことは直ぐに試してみる。
        auto pi = 3.141592;
        ptr_lambda_debug<const char*,const decltype(pi)&>("pi is ",pi);
    }
    if(1.00) {
        sample();
        sample2();
    }
    if(1.50) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ",test_Widget());
    }
    puts("=== 項目 4 ：推論された型を確認する END");
    return 0;
}