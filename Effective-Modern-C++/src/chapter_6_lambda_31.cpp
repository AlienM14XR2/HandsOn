/**
 * 6 章 ラムダ式
 * 
 * 『ラムダ式』
 * その名の通り、式である。ソースコードの一部である。
 * 
 * std::find_if(container.begin(), container.end(),
 *      [](int val) { return 0 < val && val 10 });
 * 
 * 『クロージャ』
 * ラムダが作成する実行時オブジェクトである。キャプチャモードに従い、キャプチャデータの参照もしくはコピーを保持する。
 * 上例の 第 3 実引数に渡しているオブジェクトがクロージャである。
 * 
 * 項目 31 ：デフォルトのキャプチャモードは避ける
 * 
 * e.g. compile.
 * g++ -O3 -DDBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_6_lambda_31.cpp -o ../bin/main
 * 
*/
#include <iostream>

int main(void) {
    puts("START 項目 31 ：デフォルトのキャプチャモードは避ける ===");
    puts("=== 項目 31 ：デフォルトのキャプチャモードは避ける END");
    return 0;
}