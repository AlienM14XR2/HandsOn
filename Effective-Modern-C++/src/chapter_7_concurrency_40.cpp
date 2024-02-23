/**
 * 7 章 並行 API
 * 
 * 項目 40 ：並行処理には std::atomic を、特殊メモリには volatile を用いる
 * 
 * 可哀想なことに volatile はとても誤解されています。そもそも並行プログラミングに関係しないのですから。本章に含めるべきでもありません。
 * しかし、他のプログラミング言語（Java や C# など）、volatile は並行プログラミングで重宝されるものです。
 * プログラマが volatile ー 紛れもなく本項で解説する機能 ー とたびたび混同する C++ 機能が std::atomic テンプレートです。std::atomic 
 * テンプレートをインスタンス化すると、他のスレッドから見てアトミックな動作を保証できます。
 * 
 * 重要ポイント
 * - std::atomic は、mutex を用いず複数スレッドからアクセスできるデータを表現する。並行ソフトウェアを開発するための道具である。
 * - volatile は、読み取り／書き込みを最適化するべきではないメモリを表現する。特殊なメモリを操作するための道具である。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_7_concurrency_40.cpp -o ../bin/main
*/
#include <iostream>
#include <cassert>
#include <atomic>

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](const auto message, const auto debug) -> void {
    std::cout << "DEBUG: " << message << '\t' << debug << std::endl;
};

template <class Err>
concept ErrReasonable = requires(Err& e) {
    e.what();
};

template <class Err>
requires ErrReasonable<Err>
void (*ptr_print_error)(Err) = [](const auto e) {
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

void sample_1() {
    puts("=== sample_1");
    std::atomic<int> ai(0);

    ai = 10;

    std::cout << ai << '\n';

    ++ai;

    --ai;

    std::cout << ai << std::endl;
}

/**
 * 上例の文を実行中は、他のスレッドが ai を読み取っても、値は 0, 10, 11 のいずれかにしかならず、他の値が見えることはありません
 * （もちろん、ai を変更するスレッドは上例のみとする）。
*/

int main(void) {
    puts("START 項目 40 ：並行処理には std::atomic を、特殊メモリには volatile を用いる ===");
    if(0.01) {
        auto ret = 0;
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", ret = test_debug_and_error());
        assert(ret == 1);
    }
    if(1.00) {
        sample_1();
    }
    puts("===   項目 40 ：並行処理には std::atomic を、特殊メモリには volatile を用いる END");
    return 0;
}
