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
 * 『クロージャクラス』
 * クロージャのインスタンス化に使用したクラスである。コンパイラはすべてのラムダに対し、一意なクロージャクラスをそれぞれ生成する。
 * ラムダ内の文はクロージャクラスのメンバ関数内の実行可能命令となる。
 * 
 * 項目 31 ：デフォルトのキャプチャモードは避ける
 * 
 * e.g. compile.
 * g++ -O3 -DDBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_6_lambda_31.cpp -o ../bin/main
 * 
*/
#include <iostream>
#include <vector>
#include <functional>

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](const auto message, const auto debug) -> void {
    std::cout << "DEBUG: " << message << '\t' << debug << std::endl;
};

template <class Error>
concept ErrReasonable = requires(Error& e) {
    e.what();
};
template <class Error>
requires ErrReasonable<Error>
void (*ptr_print_error)(Error) = [](const auto e) -> void {
    std::cout << "ERROR: " << e.what() << std::endl; 
};

int test_debug_and_error() {
    puts("=== test_debug_and_error");
    try {
        // ptr_print_now();
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

void sample() {
    puts("=== sample");
    int x = 3;

    auto c1 = [x](int y) {return x * y > 55;};
    auto c2 = c1;
    auto c3 = c2;

    auto ret3 = c3(100);
    ptr_lambda_debug<const char*, const decltype(ret3)&>("ret3 is ", ret3);
    auto ret4 = c2(6);
    ptr_lambda_debug<const char*, const decltype(ret4)&>("ret4 is ", ret4);
}

/**
 * c1、c2、c3 はすべて、ラムダから生成したクロージャのコピーです。
 * 正式とは言いませんが、ラムダ、クロージャ、クロージャクラスの境界線を曖昧にしてもまったく問題ありません。しかし、以降の
 * 項目では、コンパイル時に存在するもの（ラムダとクロージャクラス）、実行時に存在するもの（クロージャ）を区別し、その関係性
 * の把握が重要になる場面が多くあります。
 * 
 * 項目 31 ：デフォルトのキャプチャモードは避ける
 * 
 * C++11 にはデフォルトのキャプチャモードが 2 つあります。参照キャプチャと値キャプチャです。デフォルトの参照キャプチャでは
 * 不正参照が発生する恐れがあります。ディフォルトの値キャプチャを用いればそんな問題とは無縁になれると魅惑されてしまいそうで
 * すが、実は無縁ではありません。また、クロージャはそれ自身で完結しているとも思わされてしまいそうですが、必ずしもそうではあ
 * りません。本項目を管理職向けにまとめれば、これがすべてです。
 * 
 * 参照キャプチャでは、クロージャはローカル変数の参照、またはラムダを定義したスコープ内で使用可能な仮引数の参照を持つように
 * なります。もし、ラムダから作成したクロージャのライフタイムがローカル変数や仮引数よりも長くなければ、クロージャが持つ参照
 * は不正となります。例えば、フィルタ関数のコンテナがあるとしましょう。それぞれのフィルタ関数は int をとり、渡された値がフィ
 * ルタに一致するか否かを表す bool を返すとします。
*/


void sample2() {
    puts("=== sample2");
    using FilterContainer = std::vector<std::function<bool(int)>>;
    FilterContainer filters;

    // 5 の倍数のフィルタを追加してみます。
    filters.emplace_back(
        [](int value) { return value % 5 == 0; }
    );

    for(auto f: filters) {
        ptr_lambda_debug<const char*,const bool&>("f(3) ... ", f(3));
        ptr_lambda_debug<const char*,const bool&>("f(30) ... ", f(30));
    }
}

/**
 * 序数を実行時に算出する場合もあるとすれば、ラムダに 5 とハードコーディングできません。
 * フィルタは次のようになるかもしれません。
*/

void sample3() {
    puts("=== sample3");
    using FilterContainer = std::vector<std::function<bool(int)>>;
    FilterContainer filters;

    auto calc1 = 8;                     // computeSomeValue1();
    auto calc2 = 2;                     // computeSomeValue2();
    auto divisor = calc1 / calc2;       // computeDivisor(calc1, calc2);

    filters.emplace_back(
        [&](int value) { return value % divisor == 0; }
    );

    for(auto f: filters) {
        ptr_lambda_debug<const char*,const bool&>("f(2) ... ", f(2));
        ptr_lambda_debug<const char*,const bool&>("f(44) ... ", f(44));
    }
}

/**
 * 上例のコードは問題が発生するのを待っているようなものです。ラムダはローカル変数 divisor を参照していますが、ローカル変数は
 * sample3 が終了した時点で存在しなくなります。sample3 は filters.emplace_back の実行直後に終了するのですから、filters へ追加
 * した関数は実質的に初めから使用できない状態です。この関数を使用しても、作成時点からしてそもそも未定義動作となってしまいます。
 * 仮に divisor を明示的に参照キャプチャにしても問題は変わりません。
 * [](&divisor) { ... }
 * 
 * ※ 補足すると、filters をグローバル変数にして、sample3 を実行後、フィルタを利用する場合に問題になると著者は言っている。
 *   当然のような気がする。
*/

int main(void) {
    puts("START 項目 31 ：デフォルトのキャプチャモードは避ける ===");
    if(0.01) {
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_debug_and_error());
    }
    if(1.00) {
        sample();
        sample2();
        sample3();
    }
    puts("=== 項目 31 ：デフォルトのキャプチャモードは避ける END");
    return 0;
}
