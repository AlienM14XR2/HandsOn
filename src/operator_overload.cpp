/**
 * 演算子のオーバーロード 
 * 
 * これは少し興味があるので実践してみたい。
   適当な struct でデータを管理、そのオブジェクトの演算子のオーバーロード。
   Foo foo1,foo2;
   foo1 + foo2
   が実現できると思ってます。
   なんか面白そ。

   リファレンス先生の情報（「基本文法」の章ね）が薄いから
   ひとまず、実践してみる。
*/
// assertマクロの置換結果を制御する。
// <assert.h> をインクルードするよりも前で、このマクロを定義することによって、
// assertマクロの置換結果に影響を与える。
// #define NDEBUG
#include <iostream>
#include <cassert>

// 以下は関係ないよ。
// # if !defined(NDEBUG)
//   #define assert(expr) implementation-defined
// # else
//   #define assert(ignore) ((void)0)
// # endif

using namespace std;

struct Person {
    int level;
    string name;
    Person():level{-1},name{""} {}
    Person(const int& lv, const char* n) {
        level = lv;
        name = n;
    }
    void toString() {
        cout << "level is " << level << endl;
        cout << "name is " << name << endl;
    }
    // int operator+(const Person& p, const int& i); 
};
// 学んだこと、これはクラスのメンバ関数としては定義できない。
// 宣言だけならいいのか、今から試す。
// OK、宣言だけもコンパイラに叱られた。
int operator+(const Person& p, const int& i) {
    return p.level + i;
}
int operator-(const Person& p, const int& i) {
    return p.level - i;
}

// cpprefjp - C++日本語リファレンス
// 良くできたリファレンスなのだよ、うちのリファレンス先生とは桁違いなのだ。
// その情報量がね、うちの先生とは趣旨が違うことは理解している。
// https://cpprefjp.github.io/reference/cassert/assert.html
void test_operator_plus(){
    cout << "test_operator_plus ------------------- " << endl;
    Person alice(17,"Alice");
    alice.toString();
    alice.level = alice + 2;
    alice.toString();
    assert(alice.level == 19);
}

void test_operator_minus() {
    cout << "test_operator_minus ------------------- " << endl;
    Person alice(17,"Alice");
    alice.toString();
    alice.level = alice - 2;
    assert(alice.level == 16);
    // Assertion `alice.level == 16' failed.
    // 中止 (コアダンプ)
    // しっかりと機能しているのだ。
    // 注意、おそらくこのままでは、DEBUG BUILD になると思われる。
    // 本ソースの 17 行目あたりを見るのだ、未来のオレ。
    // あれを有効にすれば、RELEASE BUILD にできるぞ。
    alice.toString();   // これが出力されたら、リリースビルドってことになる。
}

int main() {
    cout << "START 演算子のオーバーロード ========== " << endl;
    test_operator_plus();
    test_operator_minus();
    cout << "演算子のオーバーロード ========== END " << endl;
    // できた。認識のズレ、あるいは、知識不足のため、class（struct）内での
    // 演算子のオーバーロードは、現状ではできなかった。
    return 0;
}