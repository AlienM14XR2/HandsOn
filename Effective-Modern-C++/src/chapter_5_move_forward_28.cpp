/**
 * 5 章 右辺値参照、ムーブセマンティクス、完全転送
 * 
 * 項目 28 ：参照の圧縮を理解する
 * 
 * 項目 23 ではテンプレート関数に実引数を渡す際に、仮引数に推論した型に、実引数が左辺値か右辺値かを表す情報が埋め込まれると述べましたが、
 * 埋め込まれるのは転送参照仮引数を実引数で初期化する場合のみである点には触れませんでした。転送参照は、本書ではその次の項目 24 まで登場
 * しないのがその理由です。
 * 
 * template <typename T>
 * void func(T&& param)
 * 
 * この埋め込みはの仕組みは単純です。実引数に左辺値が渡された場合 T は左辺値参照とと推論され、右辺値が渡されれば非参照と推論されます（
 * この非対称には注意。左辺値には左辺値参照という情報が埋め込まれますが、右辺値には『参照ではない』という情報が埋め込まれます）
 * // ... 
 * ここからは、サンプルを実装して確認していく。
 * 
 * 重要ポイント
 * - 参照の圧縮が発生する場面は 4 つある。テンプレートのインスタンス化、auto の型生成、typedef と using エイリアス宣言の使用と作成、
 *   decltype である。
 * - 参照の圧縮（これも著者の造語）が発生する場面でコンパイラが参照の参照を生成すると、単一の参照となる。それが、転送参照のこと。
 *   ※著者の言い回しが回りくどく説明も遠回しすぎて、本質から遠ざかっていくように感じる。
 * - 転送参照とは、左辺値と右辺値と区別する型推論および参照の圧縮場面での右辺値参照である。
 * 
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_5_move_forward_28.cpp -o ../bin/main
*/
#include <iostream>

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](const auto message, const auto debug) -> void {
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

class Widget {
};

Widget widgetFactory() {        // 右辺値を返す関数
    puts("--- widgetFactory");
    Widget w;
    // 次の書き方の違いでは、説明の解釈が異なるのかな？ いや変わらないと思う。使われ方は同じだから。
    return w;   // return Widget();
}

template <class T>
void func(T&& param) {
    puts("--- func");
    ptr_lambda_debug<const char*, const char*>("type is ", typeid(param).name());
}

void sample() {
    puts("=== sample");
    Widget w;
    func(w);                    // 左辺値を渡す、T は Widget& と推論
    func(widgetFactory());      // 右辺値を渡す、T は Widget と推論
}

/**
 * 上例では、コンパイラは func(T&& param) を次の 2 つに型推論を行って使い分けている。
 * func(Widget&& param)、右辺値参照と func(Widget& param) 左辺値参照 である。
 * ※ コンパイラでは、参照を表す参照は認められていない、次は NG、コンパイルエラーになる。
 * ```
 * int x
 * // ... 
 * auto& & rx = x;  // コンパイルエラー
 * ```
 * 次は私（読者）の解釈。
 * auto&& rx = x; // このようにコンパイラは解釈するためだと思う、これは、右辺値参照型 rx に 左辺値である x を指定しているため。
 * 書籍の解説に戻る。
 * 
 * その答えは『参照の圧縮』です。『開発者』は参照の参照を宣言できませんが、コンパイラはある特定の場面に限り使用できるのです。
 * テンプレートのインスタンス化はまさに該当します。
 * 
 * 参照の圧縮は std::forward を実現する重要な要素です。項目 25 でも述べたように、std::forward<T>() は 転送参照に対し用いるため、
 * 一般的な使用は次の例のようになります。
*/

template <class T>
void someFunc(T&& param) {
    puts("--- someFunc");
}

template <class T>
void func_v2(T&& param) {
    puts("--- func_v2");
    ptr_lambda_debug<const char*, const char*>("type is ", typeid(param).name());
    someFunc(std::forward<T>(param));
}

void sample2() {
    puts("=== sample2");
    func_v2(widgetFactory());
}

int main(void) {
    puts("START 項目 28 ：参照の圧縮を理解する ===");
    if(0.01) {
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_debug_and_error());
    }
    if(1.00) {
        sample();
        sample2();
    }
    puts("=== 項目 28 ：参照の圧縮を理解する END");
    return 0;
}