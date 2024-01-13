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
 * 解明 Type Erasure パターン
 * Type Erasure パターンの核となる 3 つのデザインパターンとは、External Polymorphism パターン（分離及び既存コードに干渉しないようにする。）、
 * Bridge パターン（構造の分離、値セマンティクスベースの実装を担う。）、Prototype パターン（用いない場合もある。作成した値型のコピーセマンティクスを実現する。）です。
 * しかし、この 3 つに限定されているわけではなく、組み合わせは文脈や意味付け、実装次第で変わることもあります。
 * この 3 つの組み合わせの重要な点は、既存コードに干渉せず、関係性を希薄にする抽象化のラッパクラス作成にあります。
 * 
 * Type Erasure パターン
 * 目的：次の性質を持つ拡張可能な型セットを対象に、既存コードに干渉せず値ベースに抽象化する。
 *       - 潜在的に非多態
 *       - 意味的に同一動作
 *       - 互いに関係を持たない
 * 
 * この目的は、正確さを損なわずにできるだけ簡潔に定義しましたが、内容を補足します。
 * 
 * 値ベース
 *      Type Erasure パターンの目的は、コピー可能、ムーブ可能、推論可能な値型の作成にある。
 *      しかしこの値型は正則な値型の要件を厳密には満たしておらず、二項演算では一部に制限がある、
 *      単項演算では最適に動作する。
 * 
 * 非干渉的
 *      
 * 
 * 拡張可能で互いに関係を持たない型セット
 *      
 * 潜在的に非多態
 *      
 * 意味的に同一動作
 *      
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_8_type_erasure.cpp -o ../bin/main
*/
#include <iostream>
#include <any>
#include <cstdlib>
#include <string>
#include <memory>
#include <cassert>

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

int sample_2() {
    /**
     * その他には、std::shared_ptr があります。与えられたデリータを内部に保持する際に型消去技術を使用しています。
    */
    puts("--- sample_2");
    try {
        // 専用デリータを指定し std::shared_ptr を作成する。デリータは型の一部ではない点に注意！
        std::shared_ptr<int> shared{
            new int(42)
            , [](int* ptr){
                delete ptr;
            }
        };

        // スコープから抜ければ std::shared_ptr は破棄され、int は専用デリータが delete する。
        int result = -1;
        ptr_lambda_debug<const char*,const int&>("*shared.get() is ",result = *shared.get()); 
        assert(result == 42);
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
    if(1.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",sample_2());
    }
    puts("=== Type Erasure パターンでの継承階層の置換を検討する END");
    return 0;
}