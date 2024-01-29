/**
 * 2 章 auto
 * 
 * 項目 6 auto が期待値とは異なる型を推論する場面では ETII を用いる
 * 
 * ETII（Explicitly Typed Initializer Idiom）明示的型付け初期化子イディオム
 * 
 * e.g.)
 * auto highPriority = static_cast<bool>(feature(w)[5]);
 * 
 * 上記に至る経緯。
 * std::vector<bool>::reference はプロクシクラス（Proxy Class）の一例です。プロクシクラスとは、他の型
 * の動作の模倣、拡張を目的としたクラスで、さまざまな場面で使用されます。std::vector<bool>::reference は
 * std::vector<bool> に対する operator[] がヒットの参照を返すように見せかけるものですし、また標準ライブラリ
 * のスマートポインタ型は raw ポインタを用いたリソース管理を実現するものです。プロクシクラスの使用については
 * すでに充分確率されており、実際のデザインパターンの「Proxy」は、もう長いことデザインパターンの殿堂にその名
 * を連ねています。
 * 一般に、「ユーザに意識させない（invisible）」プロクシクラスは、auto とは上手には協調しません。
 * 
 * 重要ポイント
 * - 「ユーザに意識させない」プロクシ型は、auto に、初期化式の「誤った」を推論させる。
 * - 明示的型付け初期化子イディオムを用いれば、auto に目的の型を推論させられる。
 * 
 * e.g.) compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_2_auto_ETII.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](const auto message, const auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

int main(void) {
    puts("START 項目 6 auto が期待値とは異なる型を推論する場面では ETII を用いる ===");
    if(0.01) {
        auto pi = 3.141592;
        ptr_lambda_debug<const char*,const decltype(pi)&>("pi is ",pi);
    }
    puts("=== 項目 6 auto が期待値とは異なる型を推論する場面では ETII を用いる END");
    return 0;
}