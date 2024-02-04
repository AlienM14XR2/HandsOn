/**
 * 3 章 現代の C++ への移行
 * 
 * 項目 17 ：自動的に生成される特殊メンバ関数を理解する
 * 
 * C++ の正式用語で特殊メンバ関数とは、C++ が自ら生成するメンバ関数を意味します。C++98 には特殊メンバ関数が 4 つあります。
 * デフォルトコンストラクタ、デストラクタ、コピーコンストラクタ、コピー代入演算子です。もちろん生成される条件があり、クラス
 * が上記関数を明示的に宣言していないけれど、コードでは使用している場合です。デフォルトコンストラクタが生成されるのは、クラス
 * が 1 つも宣言していない場合です（コンストラクタには実引数が必須と指定すると、コンパイラはデフォルトコンストラクタを生成しない）。
 * 生成された特殊メンバ関数は暗黙に public かつ inline になり、また、仮想デストラクタを持つ基底クラスを継承した派生クラスの
 * デストラクタ以外は、非仮想関数になります。基底クラスが仮想デストラクタを持っていれば、コンパイラが生成する派生クラスのデストラクタ
 * も仮想デストラクタになります。
 * C++11 から、ムーブコンストラクタとムーブ代入演算子の 2 つが追加されました。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_3_modern_c++_special_member_function.cpp -o ../bin/main
*/
#include <iostream>
#include <cassert>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](const auto message, const auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * このコンセプトがどれほどの価値があるのかは正直よく分からない、なくてもいいかな。
 * あるいは、ptr_print_error を constexpr 宣言してしまうとか。
*/

template <class Error>
concept Reasonable = requires (Error& e) {
    e.what();
};
template <class Error>
         requires Reasonable <Error>
void (*ptr_print_error)(Error) = [](const auto e) -> void {
    cout << "ERROR: " << e.what() << endl;
};

int test_debug() {
    puts("--- test_debug");
    try {
        auto pi = 3.141592;
        ptr_lambda_debug<const char*,const decltype(pi)&>("pi is ", pi);
        ptr_lambda_debug<const char*,const decltype(pi)*>("pi addr is ", &pi);
        throw runtime_error("It's test error.");
        return EXIT_SUCCESS;
    } catch(exception& e) {
        printf("e type is ... %s\n", typeid(e).name());
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * C++11 から、ムーブコンストラクタとムーブ代入演算子の 2 つが追加されました。
 * それぞれのシグネチャを挙げます。
*/

class Point {
private:
    double x,y;
public:
    Point() = delete;
    Point(double _x, double _y): x{_x}, y{_y}
    {}
    Point(const Point& own) = default;
    Point& operator=(const Point& lhs) = default;
    // ~Point() {}
    // ...
    double getx() noexcept { return x; }
    double gety() noexcept { return y; }

};
class Widget {
private:
    Point top{-10.0, 0.0}, bottom{-5.0, 5.0};
public:
    Widget(Point _top, Point _bottom): top{std::move(_top)}, bottom{std::move(_bottom)}                      // デフォルトコンストラクタ
    {}
    Widget(const Widget&) = default;        // デフォルトコピーコンストラクタ
    Widget(Widget&& rhs) {                  // ムーブコンストラクタ
        puts("------ move constructor");
        *this = rhs;
    }
    Widget& operator=(Widget&& rhs) {       // ムーブ代入演算子
        puts("------ move assignment operator");
        *this = rhs; 
        return *this; 
    }
    Widget& operator=(const Widget&) = default;
    // ...
    Point& getTop()    noexcept { return top; }
    Point& getBottom() noexcept { return bottom; }
};

int test_Widget() {
    puts("--- test_Widget");
    try {
        Widget widget{Point{0.0, 0.0}, Point{30.0, 60.0}};
        Widget w1 = std::move(widget);      // ムーブコンストラクタが呼ばれる。
        Widget w2 = Widget{Point{0.0, 0.0}, Point{33.3, 66.6}};
        w2 = std::move(widget);             // ムーブ代入演算子が呼ばれる。

        printf("w1 top(x,y) : bottom(x,y) is top(%.2f,%.2f): bottom(%.2f,%.2f)\n", w1.getTop().getx(), w1.getTop().gety(), w1.getBottom().getx(), w1.getBottom().gety());
        printf("w2 top(x,y) : bottom(x,y) is top(%.2f,%.2f): bottom(%.2f,%.2f)\n", w2.getTop().getx(), w2.getTop().gety(), w2.getBottom().getx(), w2.getBottom().gety());

        ptr_lambda_debug<const char*,const Widget*>("widget addr is ", &widget);
        ptr_lambda_debug<const char*,const Widget*>("w1     addr is ", &w1);
        ptr_lambda_debug<const char*,const Widget*>("w2     addr is ", &w2);
        return EXIT_SUCCESS;
    } catch(exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * 生成規則の動作は、対応するコピー関数と同様です。必要な場合にのみ生成され、その動作は非 static メンバ変数「メンバ単位ムーブ」です。
 * すなわち、ムーブコンストラクタは仮引数 rhs からクラスの 非 static メンバ変数をそれぞれムーブコンストラクトし、ムーブ代入演算子は
 * 同様にムーブ代入します。ムーブコンストラクタは基底クラス部分もムーブコンストラクタし（もしあれば）、ムーブ代入演算子も同様に基底
 * クラス部分を含めムーブ代入します。
 * 
 * コピーコンストラクタとコピー代入演算子は個々に独立しており、ユーザがどちらかを個別に宣言しても、問題ありません（コンパイラ
 * は必要があれば、特殊メンバ関数のもう一方を自動生成する）。しかし、ムーブに関してははこの規則は当てはまらず、ムーブコンストラクタ
 * とムーブ代入演算子は個別で宣言しても、どちらかが自動生成されることもなく、また、コピー代入演算子を無効（= delete）にします。
 * 
 * さっきこれに引っかかったから、明示的に 「Widget& operator=(const Widget&) = default;」を Widget クラスに宣言しなければいけなかったのか。
 * ここまで読んで納得した。
*/

/**
 * 読者は恐らく『三関数同時ルール』というガイドラインを耳にしたことがあるでしょう。三関数同時ルールとは、コピーコンストラクタ、コピー代入演算子、
 * デストラクタのいずれか 1 でも宣言する場合は、この 3 つをすべて宣言しなさいというもので、何らかのリソース管理を行うクラスでは、ほとんどの場合で
 * コピー演算の動作を引き継ぐ必要性があるという観測から導かれたルールです。ほぼ次のことが該当します。
 * 
 * （1）コピー演算の一方が何らかのリソース管理を伴うならば、もう一方のコピー演算でも同じリソース管理が恐らく必要になるだろう。
 * （2）恐らくクラスデストラクタでも同じリソースを管理することになるだろう（通常はリソースを解放する）。
 * 
 * C++11 では三関数同時ルールという考え方が効力を発揮し、コピー演算を宣言すれば、ムーブ演算の暗黙の生成を抑制すべきという観測結果も組み合わさり、
 * ユーザ宣言のデストラクタを持つクラスでは『ムーブ演算を生成しない』こととされました。
 * 最終的に、クラスムーブ演算が生成されるのは（必要に応じ）次の 3 つの条件がすべて真の場合のみです。
 * 
 * （1）クラスがコピー演算を宣言していない。
 * （2）クラスがムーブ演算を宣言していない。
 * （3）クラスがデストラクタを宣言していない。
 * 
 * つまり、余計なことは極力避けた方が実装が楽になると思った次第なのだが。
 * 別な言い方をするなら、リソース管理（クラス内でのメモリ取得 new と解放 delete）をする場合は上記の実装はプログラマが責任を持たないといけないこと
 * を意味する。
*/

int main(void) {
    puts("START 項目 17 ：自動的に生成される特殊メンバ関数を理解する ===");
    if(0.01) {
        int ret1 = -1;
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", ret1 = test_debug());
        assert(ret1 == 1);      // 意図的に Error を発生させているため
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_Widget());
    }

    puts("=== 項目 17 ：自動的に生成される特殊メンバ関数を理解する END");
}