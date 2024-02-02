/**
 * 3 章 現代の C++ への移行
 * 
 * 項目 12 ：オーバーライドする関数は override と宣言する
 * 
 * 重要ポイント
 * - オーバーライド関数は override と宣言する。
 * - メンバ関数の参照修飾子を用いると、左辺値オブジェクト、右辺値オブジェクト（*this）を区別できる。
 * 
 * e.g. compile
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_3_modern_c++_override.cpp -o ../bin/main
*/
#include <iostream>
#include <cassert>
#include <memory>
#include <vector>

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
 * 意図しない関数定義にならないように、コンパイラに正しく警告やエラーメッセージを出力させるためにも派生クラスのオーバーライド関数には
 * override を付けること。
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

/**
 * メンバ関数の参照修飾子についてはまだあります。
 * 実引数に左辺値のみをとる関数を記述する場合、非 const な左辺値参照仮引数とするのが通例です。
 * 
 * void doSomething(Widget& w);     // 左辺値の Widget のみをとる
 * 
 * また、右辺値のみをとる関数を記述する場合、右辺値参照仮引数とするのが通例です。
 * 
 * void doSomething(Widget&& w);    // 右辺値の Widget のみをとる
 * 
 * メンバ関数の参照修飾子は、そのメンバ関数を実行するオブジェクト、すなわち *this を同様に区別
 * するためのものです。メンバ関数宣言の末尾に記述し、そのメンバ関数を実行するオブジェクト（*this）
 * が const であることを表す、const とよく似ています。
 * 参照修飾子が必要になるメンバ関数は多くありませんが、出会うこともあるでしょう。例えば、Widget 
 * クラスが std::vector のメンバ変数をもっており、コード利用者が直接アクセスできるようアクセサ関数
 * を提供する場合を考えます。
*/

class WidgetV2 {
public:
    using DataType = std::vector<double>;
    DataType& data() & noexcept {       // WidgetV2 が左辺値ならば、左辺値を返す 
        puts("------ WidgetV2::data &");
        return values; 
    }                 
    DataType data() && noexcept {       // WidgetV2 が右辺値ならば、右辺値を返す 
        puts("------ WidgetV2::data &&");
        return std::move(values); 
    }
private:
    DataType values;
};

WidgetV2 makeWidgetV2() {
    return WidgetV2{};
}

/**
 * 上例はカプセル化が高度に図られておらず、まず陽の目を見ることがない設計ですが、それは
 * 置いといて、コード利用側はどうなるか考えてみましょう。
*/

int test_WidgetV2() {
    puts("--- test_WidgetV2");
    try {
        WidgetV2 w;
        auto vals1 = w.data();      // w.values を vals1 へコピー
        /**
         * WidgetV2::data の戻り型は左辺値参照であり（厳密に言えば std::vector<double>&）、左辺値参照は
         * 左辺値となるように定義されているため、vals1 を左辺値から初期化することになります。そのため、
         * コメントにもあるように、vals1 は w.values からコピーコンストラクトされます。
         * ここで WidgetV2 を作成する factory 関数があるとしましょう。
        */

        auto vals2 = makeWidgetV2().data();     // Widget 内の値を vals2 へコピー

        /**
         * 上例でも WidgetV2::data は左辺値参照を返し、左辺値参照は同様に左辺値になるため、やはり
         * 新規オブジェクト（vals2）はWidgetV2 内の values からコピーコンストラクトされます。しかし、
         * 今度の WidgetV2 は makeWidgetV2 が返した一時オブジェクトのため（すなわち右辺値）、内部で
         * 実行される std::vector のコピーは時間の無駄です。ムーブの方が好ましい動作ですが、data は
         * 左辺値参照を返すため、C++ の規則ではコンパイラがコピーするコードを生成することとしています
         * （「as if rule」による最適化の余地はありますが、コンパイラにそこまで期待してはいけません ）。
         * ここで必要なのは、右辺値 WidgetV2 の data 呼び出しでは、結果も右辺値となることを明示する
         * 方法です。参照修飾子を用いることで、左辺値、右辺値どちらものオーバーロードも実現できます。
         * 
         * 一時オブジェクトのようなコピーの内部的な無駄を無くす、高速化に貢献する可能性がある場合に
         * 右辺値参照を用いる、私には今はそのような解釈だな。
        */

        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << "ERROR: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}

/**
 * 個人的興味と演習
 * 独自定義しない場合のコピーコンストラクタの動作確認。
*/

class Foo {
private:
    double point;
    std::vector<string> names;
    const Derived* const derived = nullptr;
public:
    Foo() = delete;
    Foo(const double& _point, const std::vector<string>& _names, const Derived* const _derived): point{_point}, names{std::move(_names)}, derived{_derived}
    {}
    // ... コピーコンストラクタは独自定義しない。
    double& getPoint()              noexcept { return point; }
    std::vector<string>& getNames() noexcept { return names; }
    const Derived* const getDerived()              noexcept { return derived; }
};

void f_foo(Foo foo) {       // 値渡し
    puts("------ f_foo");
    double point = foo.getPoint();
    ptr_lambda_debug<const char*,const double&>("point is ",point);
    point += point;
    ptr_lambda_debug<const char*,const double&>("foo.getPoint() is ",foo.getPoint());
    ptr_lambda_debug<const char*,const double&>("point is ",point);
    puts("------");
    auto names1 = foo.getNames();
    names1.emplace_back("Dante");
    for(auto name: names1) {
        ptr_lambda_debug<const char*,const decltype(name)&>("name is ", name);
    }
    puts("------");
    auto names2 = foo.getNames();
    for(auto name: names2) {
        ptr_lambda_debug<const char*,const decltype(name)&>("name is ", name);
    }
    puts("---------");
    auto derived = foo.getDerived();
    ptr_lambda_debug<const char*,const decltype(derived)>("derived                addr is ", derived);
    ptr_lambda_debug<const char*,const Derived* const>("foo.getDerived()       addr is ", foo.getDerived());
}

/**
 * デフォルトコピーコンストラクタで充分だね。
*/

int test_Foo() {
    puts("--- test_Foo");
    try {
        Derived derived;
        std::vector<string> names = {"Jack", "Derek", "Alice"};
        Foo foo{3.00, names, &derived};
        f_foo(foo);
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
    if(1.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_WidgetV2());
    }
    if(2.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_Foo());
    }
    puts("=== 項目 12 ：オーバーライドする関数は override と宣言する END");
    return 0;
}