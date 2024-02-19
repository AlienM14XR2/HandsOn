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
 * 重要ポイント
 * - std::thread API では、非同期に実行する関数の戻り値を得る方法が存在しない。また、非同期に実行する関数が例外を発生させるとプログラムは終了する。
 * - スレッドベースのプログラミングでは、スレッド数、オーバーサブスクリプション、ロードバランスの手動管理が必要になる。また、新規プラットフォーム
 *   への移行時にも作業が発生する。
 * - デフォルトのローンチポリシで動作する std::async を用いたタスクベースのプログラミングでは、上記問題の大部分に自動的に対応する。
 * 
 * 項目 36 ：非同期性が不可欠の場面では std::launch::async を指定する
 * 
 * 並行 API に関して：）=== 
 * 
 * ここに来て、「C++ ポケットリファレンス」の情報の方が「Effective Modern C++」を上回っていることに
 * 少し笑える：）まぁ、後者を擁護するなら、後者は C++11 と C++14 にフォーカスを当てているのに対して
 * 前者は C++20 までを網羅的に扱っているということが言えるかな。また、後者は知識、情報の深さが違うと
 * も言える。
 * 
 * これは、もう実践、Try and Errors あるのみと腹をくくった：）
 * 一度使ったことがある、std::promise、std::future、std::thread と今回学んだ std::async、std::launch::async
 * に絞って、復習してみるかな。
 * 
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
 * 
 * 上例のような場合では、std::async へ渡した関数オプジェクト doAsyncWork を『タスク』と考えます。
 * 
 * ここで、C++ 並行ソフトウェアの「スレッド」が持つ、3 つの意味をまとめておきます。
 * 
 * 『ハードウェアスレッド』
 *   実際に演算を実行するスレッドである。現代のマシンアーキテクチャは CPU コア 1 つにつき、 1 つまたは複数のハードウェアスレッドを持つ。
 * 
 * 『ソフトウェアスレッド』
 *   OS スレッド、システムスレッドともいう。オペレーティングシステムが全プロセスに渡り管理し、ハードウェアスレッド上で実行するようスケジューリング
 * するスレッドである。ソフトウェアスレッドがブロックする場合（I/O の完了や mutex、条件変数でのウェイトなど）、他のブロックしていないスレッドを実行
 * すればスループットを向上させられる。このため、通常はハードウェアスレッド数よりも多くのソフトウェアスレッドが作成できる。
 * 
 * 『std::thread』
 *   C++ プロセス内で、下位に位置するソフトウェアスレッドのハンドルとして振る舞うオブジェクトである。
 * 
 * プログラムから直接 std::thread を使用すると、スレッド数の枯渇、オーバーサブスクリプション（ソフトウェアスレッドがハードウェアスレッドを超える）、
 * ロードバランスに自身で対応することになります。
 * 
 * スレッドベースプログラミングと比較すると、タスクベースの設計ではスレッドの手動管理の労苦から解放され、非同期に実行した関数の結果も普通の方法で
 * 得られます（戻り値や例外）。しかし、それでもスレッドの直接使用が適切な場面もあります。
 * 
 * ・下位に位置するスレッド実装の API を使用する必要がある
 * 
 * ・開発者がアプリケーションのスレッド使用を最適化でき、またそうする必要がある
 * 
 * ・C++ 並行 API を越えたスレッド技術を実装する必要がある
*/

int main(void) {
    puts("START 項目 35 ：スレッドベースよりもタスクベースプログラミングを優先する ===");
    if(0.01) {
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_debug_and_error());
    }
    puts("=== 項目 35 ：スレッドベースよりもタスクベースプログラミングを優先する END");
    return 0;
}
