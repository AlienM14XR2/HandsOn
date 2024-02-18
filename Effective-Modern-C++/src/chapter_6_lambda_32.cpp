/**
 * 6 章 ラムダ式
 * 
 * 項目 32 ：クロージャ内にオブジェクトをムーブする場面では初期化キャプチャを用いる
 * 
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_6_lambda_32.cpp -o ../bin/main
*/
#include <iostream>

/**
 * Coffee Break
 * 
 * External Polymorphism について復習したくなった。
 * 
 * - 概念として、何らかのコンセプトを表現する基底クラスがある、Model の基底クラス。
 * - DefaultStrategy と operator 関数
 * - Model と Strategy の分離、Model に Strategy の依存注入を行う。
 * - 具体的な処理を担う Strategy クラス。operator() 関数の実装、この関数は仮引数の型（Model）によりオーバーロードする。
*/

/**
 * Coffee Break C 言語
 * 
 * 以前、考えた簡易メモリ管理を再実装してみる。
 * 
 * - {} 任意のブロックの上下でそれぞれ、メモリの取得と解放を行う仕組み。
*/

int main(void) {
    puts("START 項目 32 ：クロージャ内にオブジェクトをムーブする場面では初期化キャプチャを用いる ===");
    puts("=== 項目 32 ：クロージャ内にオブジェクトをムーブする場面では初期化キャプチャを用いる END");
}