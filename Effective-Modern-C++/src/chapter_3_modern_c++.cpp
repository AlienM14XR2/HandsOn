/**
 * 3 章 現代の C++ への移行
 * 
 * 項目 7 ：オブジェクト作成時の () と {} の違い
 * 
 * e.g. ) compile. 
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_3_modern_c++.cpp -o ../bin/main
*/
#include <iostream>

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

int main(void) {
    puts("START 項目 7 ：オブジェクト作成時の () と {} の違い ===");
    if(0.01) {
        auto pi = 3.141592;
        ptr_lambda_debug<const char*,const decltype(pi)&>("pi is ",pi);
    }
    if(1.00) {
        sample();
        sample2();
    }
    puts("=== 項目 7 ：オブジェクト作成時の () と {} の違い END");
    return 0;
}