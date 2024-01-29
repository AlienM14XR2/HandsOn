/**
 * 3 章 現代の C++ への移行
 * 
 * 項目 7 ：オブジェクト作成時の () と {} の違い
 * 
 * 重要ポイント
 * - もっとも広範囲に使用可能な初期化構文は波括弧による初期化である。精度が落ちる変換を防ぎ、C++ の最も厄介な構文解析を回避できる。
 * - 波括弧で囲んだ初期化子は、コンストラクタのオーバーロード解決時に、見た目ではより高く一致するコンストラクタが他にあっても、仮引数が
 *   一致すれば std::initializer_list を取るコンストラクタに解決される。
 * - 丸括弧か波括弧かの選択が大きな差異を生む例としては、実引数を 2 つ与えた std::vector<numeric type> の作成がある。
 * - テンプレート内でのオブジェクト作成に、丸括弧、波括弧のどちらかを使用するかを選択するのは、楽しいながらも大変な挑戦になるだろう。
 * 
 * e.g. ) compile. 
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_3_modern_c++.cpp -o ../bin/main
*/
#include <iostream>
#include <vector>
#include <string>
#include <atomic>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](const auto message, const auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

void sample() {
    puts("--- sample");
    int x(3);
    ptr_lambda_debug<const char*,const decltype(x)&>("x is ",x);

    int y = 6;
    ptr_lambda_debug<const char*,const decltype(y)&>("y is ",y);

    int z{9};
    ptr_lambda_debug<const char*,const decltype(z)&>("z is ",z);

    // 多くの場合、等号と波括弧は併用できます。
    int zz = {99};
    ptr_lambda_debug<const char*,const decltype(zz)&>("zz is ",zz);
}

/**
 * 「見苦しい混乱」を主張する人々からは、初期化時の等号を代入と勘違いする C++ 初心者がとても多いと指摘されています。
 * もちろん代入ではありません。 int などの組み込み型でこの違いが意味を持つのは学術論くらいしか考えられませんが、
 * ユーザ定義型の場合は初期化と代入の区別は大きな意味を持ちます。呼び出される関数が異なるのです。
*/

class Widget {
public:
    Widget() {
        puts("------ Default Constructor");
    }

    Widget(const Widget& own) {
        puts("------ Copy Constructor");
        *this = own;
    }
    ~Widget() {
        puts("------ Destructor");
    }
};

void sample2() {
    puts("--- sample2");
    Widget w1;          // デフォルトコンストラクタを呼び出す。
    Widget w2 = w1;     // コピーコンストラクタを呼び出す。代入ではない。
    Widget w3;
    w3 = w1;            // 代入である。コピー演算 operator= を呼び出す。
}

/**
 * C++11 では『初期化の統一記法』を導入しました。
 * どんな初期化でも表現できる唯一の初期化構文です、少なくとも概念としては、
 * 初期化の統一記法では『波括弧』を用います。
*/

template <class T, std::size_t N>
constexpr std::size_t arraySize(T(&)[N]) noexcept {
    return N;
}

void sample3() {
    puts("--- sample3");
    std::vector<int> v{1,3,5};
    for(auto val: v) {
        ptr_lambda_debug<const char*,const decltype(val)&>("val is ",val);
    }

    Widget w{};
    ptr_lambda_debug<const char*,const char*>("w type is ",typeid(w).name());

    std::string names[] {{"Jack"},{"Derek"},{"Alice"}};
    size_t namesSize = sizeof(names)/sizeof(names[0]);
    ptr_lambda_debug<const char*,const size_t&>("namesSize is ",namesSize);
    size_t namesSize2 = arraySize(names);
    ptr_lambda_debug<const char*,const size_t&>("namesSize2 is ",namesSize2);
    for(size_t i = 0; i < namesSize; i++) {
        ptr_lambda_debug<const char*,const string&>("name is ",names[i]);
    }
}

/**
 * コピー不可能なオブジェクトは（std::atomic など。項目 40 を参照）、波括弧でも丸括弧でも
 * 初期化できますが、「=」では初期化できません。
*/

void sample4() {
    puts("--- sample4");
    std::atomic<int> ai1{3};        // 問題なし
    std::atomic<int> ai2(6);        // 問題なし
    std::atomic<int> ai3 = 99;      // エラー ... えっ、エラーにならないね。

    ptr_lambda_debug<const char*,const int&>("ai1 is ",ai1);
    ptr_lambda_debug<const char*,const int&>("ai2 is ",ai2);
    ptr_lambda_debug<const char*,const int&>("ai3 is ",ai3);
}

class WidgetV2 {
public:
    WidgetV2() {
        puts("------ Default Constructor");
    }
    WidgetV2(std::initializer_list<int> il) {
        puts("------ std::initializer_list<int> Constructor");
    }
    WidgetV2(const Widget& own) {*this = own;}
    ~WidgetV2() {}
};

void sample5() {
    puts("--- sample5");
    WidgetV2 w1;
    WidgetV2 w2{};
    // WidgetV2 w3();       // もっとも厄介な構文解析！ 関数宣言となる。これは、コンパイルエラーになった。

    WidgetV2 w11({});
    WidgetV2 w12{{}};
}

void sample6() {
    puts("--- sample6");
    std::vector<int> v1(10,20);     // 要素数 10 個 の 初期値 20 の std::vector を作成する。
    for(auto elm: v1) {
        ptr_lambda_debug<const char*,const decltype(elm)&>("elm is ",elm);
    }
    puts("これは確かに、混乱させるかもね。");
    std::vector<int> v2{10,20};     // 要素数 2 個の 初期値がそれぞれ、10 と 20 の std::vector を作成する。
    for(auto elm: v2) {
        ptr_lambda_debug<const char*,const decltype(elm)&>("elm is ",elm);
    }
}

int main(void) {
    puts("START 項目 7 ：オブジェクト作成時の () と {} の違い ===");
    if(0.01) {
        auto pi = 3.141592;
        ptr_lambda_debug<const char*,const decltype(pi)&>("pi is ",pi);
    }
    if(1.00) {
        sample();
        sample2();
        sample3();
        sample4();
        sample5();
        sample6();
    }
    puts("=== 項目 7 ：オブジェクト作成時の () と {} の違い END");
    return 0;
}