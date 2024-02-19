/**
 * 7 章 並行 API
 * 
 * 項目 35 ：スレッドベースよりもタスクベースプログラミングを優先する
 * 
 *   C++11 の偉業の 1 つに言語とライブラリへの並行性の導入があります。C++ 以外のスレッド API に慣れたプログラマが（pthread や Windows スレッド）、
 * スパルタ式とも言える比較的質実剛健な C++ の並行機能セットに驚くことがありますが、これは C++ 並行対応作業の大部分がコンパイラ開発側が負担
 * する形で実現されたおかげです。その結果、標準規格として C++ の歴史上初めて、どのプラットフォームでも動作が変わらないマルチスレッドプログラムの
 * 開発が保証されました。ここから表現力豊富なライブラリを築けられ、堅牢な基礎が出来上がったのです。標準ライブラリの並行部品は（タスク、future、
 * thread、mutex、条件変数、atomic、その他）、成長する C++ 並行ソフトウェア開発の豊富なツールセットの第一歩に過ぎません。
 *   以降の項目では、標準ライブラリには 2 つのテンプレート、std::future と std::shared_future があることを忘れずに覚えておいてください。ほとんど
 * の場面でその差異は重要ではないため、本書ではその両方を指し、単に 『future』と表記します。
 * 
 * ※ thread, promise, future 
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_7_concurrency_35.cpp -o ../bin/main
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

/**
 * 関数 doAsyncWork を非同期に実行する場合、基礎となる選択肢が 2 つあります。1 つは std::thread を作成し、そのスレッド上で doAsyncWork を実行
 * する方法は、『スレッドベース』の方式です。
 * 
 * int doAsyncWork();
 * 
 * std::thread t(doAsyncWork);
 * 
 * もう 1 つは std::async に doAsyncWork を渡す方法で
 * 『タスクベース』という方式です。
 * ※ std::async を利用することだけを取って、タスクベースという訳ではないことを肝に銘じること。
 * 
 * auto future_ = std::async(doAsyncWork);
 * 「タスクベースプログラミング」とは次の 3 つを土台とする設計手法です。
 * (1) 実行する仕事の単位「タスク」を定義する。
 * (2) タスク間のデータ依存性と時間依存性を明確にする。
 * (3) 前項の依存性を維持しつつ、タスクを実行するよう実行環境を構築、設定する。
*/

int main(void) {
    puts("START 項目 35 ：スレッドベースよりもタスクベースプログラミングを優先する ===");
    if(0.01) {
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_debug_and_error());
    }
    puts("=== 項目 35 ：スレッドベースよりもタスクベースプログラミングを優先する END");
    return 0;
}
