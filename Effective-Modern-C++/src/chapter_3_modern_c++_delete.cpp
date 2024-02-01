/**
 * 3 章 現代の C++ への移行
 * 
 * 項目 11 ：未定義 private 関数よりも =delete を優先する
 * 
 * 重要ポイント
 * - 関数を private な未定義とするよりも delete の使用を優先する。
 * - 非メンバ関数、テンプレートのインスタンス化も含め、どんな関数でも delete できる。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_3_modern_c++_delete.cpp -o ../bin/main
*/
#include <iostream>
#include <cassert>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](const auto message, const auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

int test_debug() {
    puts("--- test_debug");
    try {
        auto pi = 3.141592;
        ptr_lambda_debug<const char*,const decltype(pi)&>("pi is ", pi);
        ptr_lambda_debug<const char*,const decltype(pi)*>("pi addr is ", &pi);
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << "ERROR: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}

/**
 * 慣習として delete された関数は private ではなく public と宣言することとされていますが、
 * これには理由があります。コード利用者がメンバ関数を使用しようとする際に、C++ は delete 
 * 状態より先にアクセスの可否をチェックします。コード利用者が private かつ delete された
 * 関数を使用しようとした場合に、関数が private であることだけをメッセージするコンパイラ
 * があります。この場合のアクセス可否は実際問題ではないにも関わらずです。古いコードの 
 * private で未定義のメンバ関数を、delete された関数へ変更する際には、この点を肝に銘じて
 * おくのが良いでしょう。新規関数は public とした方が通常は分かりやすいエラーメッセージ
 * になります。
 * private にできるのはメンバ関数のみなのに対し、delete が持つ大きな利点は、『どんな関数』
 * でも delete できることです。
 * 
 * 次のようなそれが幸運の数字か否かを返す、非メンバ関数があるとしましょう。
 * 
 * bool isLucky(int number);
 * 
 * C++ は C から受け継いだ遺産のおかげで、漠然と数値型とみなせるきわめて多くの型が暗黙に
 * int へ変換されます。しかし、コンパイル可能な呼び出しでもその一部は意味をなさない恐れが
 * あります。
 * 
 * if (isLucy('a')) ...
 * if (isLucy(true)) ...
 * if (isLucy(3.5)) ...
 * 
 * ラッキーナンバが絶対に整数でなければいけないとしたら、上例のような呼び出しはそもそも
 * コンパイル不可とすべきです。
 * これを実現する方法の 1つに、排除したい型用にオーバーロードし、delete する方法があります。
*/

bool isLuch(int number);
bool isLuch(char)    = delete;
bool isLuch(bool)    = delete;
bool isLuch(double)  = delete;

/**
 * delete された関数には他にも（private なメンバ関数ではできない）利点があります。無効なテンプレート
 * のインスタンス化を防ぐ効果があります。例えば、組み込みポインタを処理するテンプレートが必要になった
 * とします。
 * 
 * template <class T>
 * void processPointer(T* ptr);
 * 
 * ポインタの世界には特例が 2 つあります。 1 つは void* ポインタです。void* ポインタは参照先を辿ることも
 * 、インクリメント、デクリメントなどもできません。もう 1 つは char* ポインタです。C スタイルの文字列を
 * 表現するのが通例で、個々の文字を指すものではありません。この 2 つの特例には特別扱いが必要になることが
 * 多く、上例のテンプレートでは、拒否するのが正常な動作としてみましょう。すなわち、void* や char* ポインタ
 * を与えて processPointer 呼び出しは不可とします。
 * この強制は難しくありません。単にそのインスタンス化を delete すれば良いのです。
*/

template <class T>
void processPointer(T* ptr);

template<>
void processPointer<void>(void*) = delete;

template<>
void processPointer<char>(char*) = delete;


int main() {
    puts("START 項目 11 ：未定義 private 関数よりも =delete を優先する ===");
    if(0.01) {
        int ret = -1;
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",ret = test_debug());
        assert(ret == 0);
    }
    puts("=== 項目 11 ：未定義 private 関数よりも =delete を優先する END");
    return 0;
}