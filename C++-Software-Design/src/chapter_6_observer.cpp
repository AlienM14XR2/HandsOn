/**
 * 6 章 Adapter パターン、Observer パターン、CRTP パターン
 * 
 * CRTP（Curiously Recurring Template Pattern）... 直訳 ... 奇妙なことに繰り返し発生するテンプレート パターン。
 * 
 * ガイドライン 25：通知を抽象化するには Observer パターン
 * 
 * サブジェクト（観察対象または単に対象、観察されるソフトウェアエンティティ、状態変化が発生する箇所）と
 * 複数存在するかもしれない、オブザーバ（状態変化に応じた通知を受け取るコールバック）との分離こそ、
 * Observer パターンの目的です。
 * 
 * Observer パターン
 * 目的：あるオブジェクトが状態を変えたときに、それに依存するすべてのオブジェクトに自動的にそのことが知らされ、また、
 *      それらが更新されるように、オブジェクト間に一対多の依存関係を定義する。
 * 
 * すべてのデザインパターンがそうですが、Observer パターンも、変更もしくはその予定がある部分をパリエーションポイント（Variation Point）
 * として切り出し、なんらかの形で抽象化します。
 * 
 * e.g. compile
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_6_observer.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * Observer パターンの古典的実装
*/

class Observer {
public:
    virtual ~Observer() = default;
    virtual void update() = 0;
    /**
     * 次のようにも定義できる。
     * 
     * virtual void update1( arguments representing the updated state ) = 0
     * virtual void update2( arguments representing the updated state ) = 0
    */

};

int main(void) {
    puts("START Observer パターン ===");
    if(0.01) {
        double pi = 3.14159;
        ptr_lambda_debug<const char*,const int&>("pi is ",pi);
    }
    puts("=== Observer パターン END");
    return 0;
}
