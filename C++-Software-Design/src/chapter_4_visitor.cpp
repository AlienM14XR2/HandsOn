/**
 * 4 章 Visitor パターン
 * 
 * 確かになぜこれが最初に紹介すべきパターンなのか理解できない。
 * 
 * ガイドライン 15: 型または処理の追加に備え設計する。
 * 
 * 図形描画をサンプルに説明するようだ。
 * オブジェクト指向の解は型（円、正方形、その他）の追加という点ではOCP に従っているが、処理の追加という点では
 * 逸脱してしまっている（Serializa のような Shape をバイト列に変換する必要が生じた場合）。
 * 
 * 動的多態性を用いると設計上の判断を迫られる。
 * 型の追加は容易だが、処理の追加は困難になる（手続き型では逆、型の追加が困難で、処理の追加は容易）。
 * 静的多態性は上記の範疇ではない、しかし適切に利用することは当然だと「本」は言っている。
 * 
 * ガイドライン 15 の要約
 * - 異なるプログラミングパラダイムそれぞれの長所と短所を把握する。
 * - プログラミングパラダイムの長所を活かし、短所を回避する。
 * - 動的多態性では型を追加するか、もしくは処理を追加するかの選択があることを理解する。
 * - 主に型を追加するならば、オブジェクト指向の解を優先する。
 * - 主に処理を追加するならば、手続き型／関数型プログラミングの解を優先する。
 * 
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_4_visitor.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

int main() {
    puts("START 4 章 Visitor パターン ===");
    if(0.01) {
        double pi = 3.14159;
        ptr_lambda_debug<const char*,const double&>("pi is ", pi);
    }
    puts("=== 4 章 Visitor パターン END");
}