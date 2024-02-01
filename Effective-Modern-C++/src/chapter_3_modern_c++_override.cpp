/**
 * 3 章 現代の C++ への移行
 * 
 * 項目 12 ：オーバーライドする関数は override と宣言する
 * 
 * e.g. compile
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_3_modern_c++_override.cpp -o ../bin/main
*/
#include <iostream>
#include <cassert>
#include <memory>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](const auto message, const auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

int test_debug() {
    puts("--- test_debug");
    try {
        auto pi = 3.141592;
        ptr_lambda_debug<const char*,const decltype(pi)&>("pi is ", pi);
        ptr_lambda_debug<const char*,const decltype(pi)*>("pi addr is ", &pi);
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << "ERROR: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}

/**
 * 仮想関数のオーバーライドとは、基底クラスのインタフェースを介し、派生クラスの関数の実行を実現するものです。
 * 
*/

class Base {
public:
    virtual ~Base() = default;
    virtual void doWork() const = 0;    // 関数の修飾 const は必須ではない。
    virtual void doSomething() = 0;
};

class Derived final : public Base {
public:
    virtual void doWork() const override {      // Base::doWork をオーバーライド、「virtual」は必須ではない。
        puts("------ Derived::doWork");         // override あり
    }
    virtual void doSomething() {
        puts("------ Derived::doSomthing");     // override なし
    }
};

class Widget {
public:
    void doWork() &;        // このバージョンの doWork は *this が左辺値の場合にのみ使用可能、一般的なオブジェクトでの使用が可能。
    void doWork() &&;       // このバージョンの doWork は *this が右辺値の場合にのみ使用可能、factory などによる特殊なオブジェクト生成が必要。
};
void Widget::doWork() & {
    puts("------ Widget::doWork &");
}
void Widget::doWork() && {
    puts("------ Widget::doWork &&");
}

Widget makeWidget() {
    Widget&& w{};           // これを実際に利用したい場面がよくわからない。
    return w;
}


/**
 * オーバーライドには要件がいくつかあります。
 * 
 * - 基底クラスの関数は仮想関数でなければならない
 * - 基底クラス、派生クラスの関数名は一致していなければならない（デストラクタ）は例外
 * - 基底クラス、派生クラスそれぞれの関数の仮引数型は一致していなければならない。
 * - 基底クラス、派生クラスそれぞれの関数の const 性は一致していなければならない。
 * - 基底クラス、派生クラスそれぞれの関数の戻り型及び例外指定は互換でなければならない。
 * 
 * 上記要件は C++98 にもそのまま適用されますが、C++11 にはもう 1 つあります。
 * 
 * - 関数の参照修飾子も一致していなければならない。
 * 
 * メンバ関数の参照修飾子は C++11 のあまり宣伝されていない機能の 1 つです。
 * メンバ関数の参照修飾子とは、その関数を左辺値のみに使用する。または右辺値のみに使用するよう制限を課すものです。
 * 参照修飾子は仮想関数か否かとは独立に指定できます。
 * 
*/

int test_Derived() {
    puts("--- test_Derived");
    try {
        std::unique_ptr<Base> base = std::make_unique<Derived>(Derived{});
        base.get()->doWork();
        base.get()->doSomething();
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << "ERROR: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}

int test_Widget() {
    puts("--- test_Widget");
    try {
        Widget w;
        w.doWork();             // 左辺値の Widget::doWork（Widget::doWork &） を呼び出す
        makeWidget().doWork();  // 右辺値の Widget::doWork（Widget::doWork &&）を呼び出す
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << "ERROR: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}

int main(void) {
    puts("START 項目 12 ：オーバーライドする関数は override と宣言する ===");
    if(0.01) {
        auto ret = -1;
        ptr_lambda_debug<const char*,const decltype(ret)&>("Play and Result ... ", ret = test_debug());
        assert(ret == 0);
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_Derived());
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_Widget());
    }
    puts("=== 項目 12 ：オーバーライドする関数は override と宣言する END");
    return 0;
}