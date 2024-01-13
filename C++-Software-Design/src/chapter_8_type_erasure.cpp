/**
 * 8 章 Type Erasure パターン
 * 
 * ガイドライン 32 Type Erasure パターンでの継承階層の置換を検討する
 * 
 * 本書において繰り返し提言していること。
 * - 依存関係を最小化せよ。
 * - 関心を分離せよ。
 * - 継承よりコンポジションを優先せよ。
 * - 既存コードに干渉しない解を優先せよ。
 * - 参照セマンティクスより値セマンティクスを優先せよ。
 * 
 * External Polymorphism パターンの値セマンティクスベースの実装モデル。
 * この技術を一般に Type Erasure （型消去）技術と言います。
 * 
 * 著者が知る限り、この技術が最初に考案されたのは 2000 年の Kevin Henney の論文です。
 * これは標準ライブラリにも採用されており、すでに紹介した、std::function や std::any
 * がそれにあたります。
 * std::function は、callable を値ベースに抽象化します。また、std::any は仮想的にどんな
 * ものでも（原語は anything。名前の由来）、その内容／機能を隠蔽したまま、コンテナライク
 * な値に抽象化します。 
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_8_type_erasure.cpp -o ../bin/main
*/
#include <iostream>
#include <any>
#include <cstdlib>
#include <string>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

int sample_1() {
    puts("--- sample_1");
    try {
        std::any a;         // create an empty 'any'
        a = 3;              // Storing an 'int' inside the 'any'
        a = "some string"s; // Replacing the 'int' with a 'std::string'

        // There is nothing we can do with the 'any' except for getting the value back.（any は値を取り出すことぐらいしか出来ない。）
        std::string s = std::any_cast<std::string>(a);
        ptr_lambda_debug<const char*,const string&>("s is ",s);
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

int main(void) {
    puts("START Type Erasure パターンでの継承階層の置換を検討する ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const int&>("pi is ", pi);
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",sample_1());
    }
    puts("=== Type Erasure パターンでの継承階層の置換を検討する END");
    return 0;
}