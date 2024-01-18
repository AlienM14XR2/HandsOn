/**
 * 9 章 Decorator パターン
 * 
 * ガイドライン 36 ：実行時抽象化とコンパイル時抽象化間のトレードオフを理解する
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_9_decorator2.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * 値ベースのコンパイル時 Decorator パターン
 * 
 * 抽象化の主要部分にはテンプレートを使います。また　C++20　のコンセプトも転送参照も使います。
 * ですが、そんなことよりも Decorator パターンを用いた設計という重要な目的を見失ってはいけません。
 * 通常商品の種類が増えた場合や、新たな価格変更要因に対応する decorator を容易に追加できるように
 * することが目的です。
 * ConferenceTicket クラスを例に進めます。
*/

int main(void) {
    puts("START Decorator パターン ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    puts("=== Decorator パターン END");
    return 0;
}