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
 * 項目 39 ：イベントが 1 度切りならば void な future を検討する
 * 
 * ある特定のイベントが発生した場合に、タスクに第 2 のタスクを非同期に実行させると有用な場面があります。イベントが発生するまで 第 2 のタスクは
 * 処理を開始できない場合です。例えば、データの初期化が完了した、計算の準備が整った、センサが異常な値を検知したなどが考えられます。このような
 * 場面でのスレッド間通信の最適な方法とは何でしょうか？
 * 分かりやすい方法は『条件変数』でしょう。条件を検知するタスクを『検知タスク』、条件に反応し処理を開始するタスクを『反応タスク』とそれぞれ呼ぶ
 * とします。方式は単純です。反応タスクは条件変数でウェイトし、検知タスクはイベント発生を条件変数に通知します。
 * 
 * std::condition_variable cv;      // イベント用の条件変数
 * 
 * std::mutex m;                    // cv に使用する mutex
 * 
 * // ... イベントを検知
 * 
 * cv.notify_one();                 // 反応タスクへ通知
 * 
 * // ... 反応処理の準備
 * {                                // クリティカルセクション開始
 * 
 * std::unique_lock<std::mutex> lk(m);      // mutex をロック
 * 
 * cv.wait(lk, []{ return flag; });   // イベントが発生したか否か                    
 * }
 * 
 * std:atomic<bool> flag(false);    // 共有フラグ
 * 
 * // ... イベントを検知
 * {
 * std::lock_guard<std::mutex> g(m) // g のコンストラクタから m をロック
 * 
 * flag = true;
 * }
 * // ... 反応処理の準備
 * {
 * std::lock_guard<std::mutex> g(m) // g のコンストラクタから m をロック 
 * 
 * while(!flag);                    // イベントをウェイト
 * }
 * // ... イベント反応処理
 * 
 * ※ 本項もコーディングはしないだろうな。
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_7_concurrency_38.cpp -o ../bin/main
*/
#include <iostream>
#include <thread>
#include <future>

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

/**
 * 本項の始めにコメントで書いたものは『条件変数』を用いたものだが、より簡潔に表現する仕組みが
 * std::promise を用いたものである。
*/

void react() {              // 反応タスクの関数
    puts("--- react");
}

void detect(std::promise<void>&& p) {             // 検知タスクの関数
    puts("--- detect");
    std::thread t(
        [&p]{
            p.get_future().wait();
            react();
        });
    p.set_value();          // t の実行再開（react を呼び出す）
    // ...                  // 何らかの処理
    t.join();               // 項目 37 参照（join 不可にする）この言い方がよくない、他の処理で join させないということだな。
}

int sample_1() {
    puts("=== sample_1");
    try {
        detect(std::move(std::promise<void>()));
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * std::thread std::promise std::future を利用したサンプル
 * ポケットリファレンスから引用した。 
*/

int computeSomething(const double& x) {
    return x * x;
}

void worker(std::promise<int> p, const double& x) {
    try {
        p.set_value(computeSomething(x));
    } catch(...) {
        p.set_exception(std::current_exception());
    }
}

int sample_2() {
    puts("=== sample_2");
    int ret = 0;
    std::promise<int> p;
    std::future<int> f = p.get_future();
    double x = 3.0;
    std::thread th(worker, std::move(p), std::ref(x));
    try {
        std::cout << "value is " << f.get() << std::endl;
        ret = EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        ret = EXIT_FAILURE;
    }
    th.join();
    return ret;
}

int main(void) {
    puts("START 項目 38 ：スレッドハンドルのデストラクト動作の差異には注意する ===");
    if(0.01) {
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_debug_and_error());
    }
    if(1.00) {
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", sample_1());
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", sample_2());
    }
    puts("===  項目 38 ：スレッドハンドルのデストラクト動作の差異には注意する  END");
    return 0;
}
