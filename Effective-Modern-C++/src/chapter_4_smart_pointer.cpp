/**
 * 4 章 スマートポインタ
 * 
 * 著者が考える raw ポインタを愛せない理由。
 * 
 * 1. その宣言からでは、単一オブジェクトを指すのか、配列を指すのか判断できない。
 * 
 * 2. その宣言からでは、使用を終えた時に対象オブジェクトを破棄すべきか否か、すなわちポインタ
 *    がその指しているオブジェクトを『所有している』か否かを判断できない。
 * 
 * 3. 対象オブジェクトを破棄すべきと判断したとしても、delete するか、もしくは他の破棄方法を
 *    実行すべきか（そのポインタ専用の破棄関数など）、破棄方法を通知する手段がない。
 * 
 * 4. 破棄方法が delete だと分かったとしても、上記の理由 1 のように単一オブジェクトとして扱う
 *    のか（「delete」）、それとも配列として扱うのかを知る術がない（「delete []」）。 形態が
 *    一致しない破棄を実行するとその結果は未定義となってしまう。
 * 
 * 5. ポインタが対象オブジェクトを所有しており、破棄方法も判明したとしても、コードの全実行パス
 *    中で『正確に一度だけ』破棄するのは容易ではない（例外処理の実行パスも含め）。 実行パスを
 *    すべて把握できなければリソースの解放もれになり、逆に複数回破棄しては未定義動作となってしまう。
 * 
 * 6. ポインタが不正か否かを通知する術が、一般に存在しない。不正ポインタとは、オブジェクトを保存
 *    していないメモリを指すポインタであり、ポインタが指しているオブジェクトを破棄すると発生する。
 * 
 * C++11 には 4 種類のスマートポインタがあります。
 * std::auto_ptr
 * std::unique_ptr
 * std::shared_ptr
 * std::weak_ptr
 * いずれもダイナミックに割り当てたオブジェクトのライフサイクルの管理を支援するべく設計されたものです。
 * すなわち、適切な時に、（例外発生時も含め）、適切な方法でオブジェクトを確実に破棄し、リソースの解放
 * 漏れを防止します。
 * 
 * 項目 18 ：独占するリソースの管理には std::unique_ptr を用いる
 * 
 * std::unique_ptr は独占所有（排他的所有権）セマンティクスを備えます。非ヌルの std::unique_ptr はその
 * 指す対象を常に所有し、std::unique_ptr をムーブすると、所有権も元のポインタからムーブ先のポインタへ
 * 転送されます（元のポインタはヌルになる）。std::unique_ptr はコピーできません。std::unique_ptr は
 * 『ムーブ専用型』です。非ヌルの std::unique_ptr を破棄すると、std::unique_ptr が対象のリソースを破棄
 * します。デフォルトのリソース破棄では、std::unique_ptr が持つ raw ポインタに対する delete が実行されます。
 * 
 * 一般的な使われ方は、factory 関数の戻り値型です。基底クラスのポインタを利用する場合です（実質は各派生クラスのインスタンス）。
 * デフォルトでは、delete により破棄されますが、カスタムデリータを用いるよう、std::unique_ptr 作成時に指定可能です。
 * 
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_4_smart_pointer.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](const auto message, const auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

template<class Error>
concept Reasonable = requires(Error& e) {
    e.what();
};
template<class Error>
requires Reasonable <Error>
void (*ptr_print_error)(Error) = [](const auto e) -> void {
    cout << "ERROR: " << e.what() << endl;
};

int test_debug() {
    puts("--- test_debug");
    try {
        auto pi = 3.141592;
        ptr_lambda_debug<const char*,const decltype(pi)&>("pi is ", pi);
        ptr_lambda_debug<const char*,const decltype(pi)*>("pi addr is ", &pi);
        throw runtime_error("It's test error.");
        return EXIT_SUCCESS;
    } catch(exception& e) {
        printf("e type is ... %s\n", typeid(e).name());
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * factory 関数の例を具体的に実装する演習でもやってみる。
 * Investment 投資を基底クラスとした次の派生クラスを作るものとする。
 * Stock 株式、Bond 債権、RealEstate 不動産。
*/

int main(void) {
    puts("START 項目 18 ：独占するリソースの管理には std::unique_ptr を用いる ===");
    if(0.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_debug());
    }
    puts("=== 4 章 項目 18 ：独占するリソースの管理には std::unique_ptr を用いる END");
    return 0;
}
