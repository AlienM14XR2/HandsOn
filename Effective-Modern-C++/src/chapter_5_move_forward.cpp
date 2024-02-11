/**
 * 5 章
 * 右辺値参照、ムーブセマンティクス、完全転送
 * 
 * ・ムーブセマンティクス
 *   コンパイラによる、コスト高なコピー演算のコスト安なムーブ演算への置き換えを可能にする。
 *   コピーコンストラクタ、コピー代入演算子がオブジェクトのコピーを制御するのと同様に、
 *   ムーブコンストラクタ、ムーブ代入演算子はオブジェクトのムーブセマンティクスを制御する。
 *   また、ムーブセマンティクスからは std::unique_ptr、std::future、std::thread などの
 *   ムーブ専用型も生み出される。
 * 
 * ・完全転送
 *   任意の実引数をとり、他の関数へ転送する関数テンプレートの記述を可能にする。転送先関数は
 *   完全に同じ実引数を直接受け取ったかのように動作する。
 * 
 * 右辺値参照は、独立した機能と言うよりも、上記 2 つの機能をつなぎ合わせる糊の役割を果たす、ムーブセマンティクスと
 * 完全転送を実現するための言語機能です。
 * この 3 つの機能の経験を積むにつれ、当初抱いた印象は氷山の一角に過ぎなかったと実感するでしょう。右辺値参照、
 * ムーブセマンティクス、完全転送の世界には、見た目以上に微妙な差異が隠れています。例えば、std::move は実際には何
 * もムーブしませんし、完全転送は不完全なものです。ムーブ演算が常にコピー演算よりもコスト安という訳でもなく、
 * コスト安になる場合でも常に期待するほどの差が得られる訳でもありません。また、呼び出される場面が常にムーブ可能と
 * いう訳でもありませんし、「type&&」というコードが常に右辺値参照を表す訳でもありません。
 * 
 * 現時点では不透明な部分が残っていても構いません。
 * 本項を読み進める際には、たとえその型が右辺値参照とされていても、仮引数とは常に左辺値であるという点を心に刻み込
 * んでおくことが特に重要です。
 * 
 * void f(Widget&& w);
 * 
 * 上例の仮引数は左辺値です。Widget の右辺値参照と記述されているにも関わらずです。
 * 
 * 項目 23 ：std::move と std::forward を理解する
 * 
 * std::move と std::forward については、こう動作すると解説するよりも、『こう動作しない』と解説を進めた方が良いでしょう。
 * std::move は何もムーブしませんし、std::forward は何も転送しません。実行時には、どちらも何もしないのです。
 * std::move と std::forward はキャストを実行する関数に過ぎません（実際にはカンスウテンプレート）。std::move は実引数を
 * 無条件に右辺値へキャストし、std::forward は特定の条件が満たされた場合にのみ、同様のキャストを実行します。これだけです。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_5_move_forward.cpp -o ../bin/main
*/
#include <iostream>

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](const auto message, const auto debug) -> void {
    std::cout << "DEBUG: " << message << '\t' << debug << std::endl;
};

template <class Error>
concept Reasonable = requires(Error& e) {
    e.what();
};
template <class Error>
void (*ptr_print_error)(Error) = [](const auto e) -> void {
    std::cout << "ERROR: " << e.what() << std::endl;
};

int test_debug_and_error() {
    puts("=== test_debug_and_error");
    try {
        auto pi = 3.141592;
        ptr_lambda_debug<const char*,const decltype(pi)&>("pi is ", pi);
        ptr_lambda_debug<const char*,const decltype(pi)*>("pi addr is ", &pi);
        throw std::runtime_error("It's test error.");
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        printf("e type is ... %s\n", typeid(e).name());
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int main(void) {
    puts("START 項目 23 ：std::move と std::forward を理解する ===");
    if(0.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_debug_and_error());
    }
    puts("=== 項目 23 ：std::move と std::forward を理解する END");
    return 0;
}