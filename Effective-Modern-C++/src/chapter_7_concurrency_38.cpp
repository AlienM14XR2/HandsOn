/**
 * 7 章 並行 API
 * 
 * 項目 38 ：スレッドハンドルのデストラクト動作の差異には注意する
 * 
 * join 可能な std::thread を破棄するとプログラムは終了します。他の 2 つの選択肢、暗黙の join と暗黙の detach よりもましなものを選択した結果で
 * す。future のデストラクタは、暗黙に join したかのように振る舞うこともあれば、暗黙に detach したかのうように振る舞うこともあります。また、そ
 * のどちらにも該当しない場合もあります。しかし、それでいてプログラムを終了させることは一切ありません。
 * 
 * future を、呼び出された側が呼び出した側へ実行結果を送信する通信チャネルの一端と考えるところ始めます。呼び出された側（通常は非同期に実行される）
 * はその実行結果を通信チャネルへ書き込み（通常は std::promise オブジェクトを介す）、呼び出した側は書き込まれた実行結果を future から読み取ります。
 * 
 * 『共有ステート』
 * 共有ステートは一般にヒープ上のオブジェクトとして実装されますが、標準仕様ではその型、インタフェース、実装詳細を明記していません。標準ライブラリ
 * 実装者が自由に実装できます。
 * 共有ステートの存在は、特に次の点で大きな意味を持ちます。
 * 
 * 遅延させられなかったタスクの共有ステートを参照する、最後の future のデストラクタが std::async 経由で実行されても、タスクが完了するまでブロック
 * する。実質的にこの種の future のデストラクタが、タスクを非同期に実行するスレッドを暗黙に join する。
 * 
 * 他の future のデストラクタは、単純に future オブジェクトを破棄する。
 * タスクが非同期に実行中の場合、この破棄する動作はスレッドの暗黙の detach に相当する。
 * タスクの実行が遅延させられ、かつ、この future が最後の future の場合、遅延させられたタスクは実行されないままとなる。
 * 
 * 重要ポイント
 * - future のデストラクタは通常、future のメンバ変数を破棄するのみである。
 * - std::async により開始され、遅延させられないタスクの共有ステートを参照する最後の future は、タスクが完了するまでブロックする。
 * 
 * ※ 本項もコーディングはしないだろうな。
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_7_concurrency_38.cpp -o ../bin/main
*/
#include <iostream>

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](const auto message, const auto debug) -> void {
    std::cout << "DEBUG: " << message << '\t' << debug << std::endl;
};

template <class Err>
concept ErrReasonable = requires(Err& e) {
    e.what();
};
template <class Err>
requires ErrReasonable<Err>
void (*ptr_print_error)(Err) = [](const auto e) -> void {
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

int main(void) {
    puts("START 項目 38 ：スレッドハンドルのデストラクト動作の差異には注意する ===");
    puts("===  項目 38 ：スレッドハンドルのデストラクト動作の差異には注意する  END");
    return 0;
}
