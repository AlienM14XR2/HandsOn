/**
 * Lost Chapter Concurrency
 * 
 * 個人的な疑問、不明点等を明らかにする。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror coffee_break_concurrency.cpp -o ../bin/main
*/
#include <iostream>
#include <future>
#include <memory>
#include <thread>
#include <chrono>
#include <mutex>

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
 * オブジェクトを複数のスレッドで利用しなければ、メンバ変数の const 修飾は必要ないはず。
 * オブジェクトとスレッドを 1 : 1 で管理する。
 * 
 * スレッドの同期はどのように行うのか、あるいはスレッドの生存期間の制御を考えてみる。
*/

class Widget {
public:
    Widget():alive(true)
    {}
    bool getAlive() const        { return alive; }
    void setAlive(const bool& b) const {
        std::lock_guard<std::mutex> guard(m);   // lock mutex
        alive = b; 
    }                                           // unlock mutex
private:
    mutable std::mutex m;
    mutable bool alive;
};

void liveWidget(std::shared_ptr<Widget>& pw) {
    puts("--- START liveWideget");
    ptr_lambda_debug<const char*, const Widget*>("pw addr is ", pw.get());
    ptr_lambda_debug<const char*, const bool&>("lived ? ", pw->getAlive());
    while(pw->getAlive()) {
        // 生存中に何らかの処理が可能
        // printf("live ...\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    puts("END --- liveWideget");
}

void dieWidget(std::shared_ptr<Widget>& pw) {
    puts("--- START dieWidget");
    ptr_lambda_debug<const char*, const Widget*>("pw addr is ", pw.get());
    for(int i = 0; i<100; i++) {
        // printf("wait ... \n");
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    pw->setAlive(false);
    printf("... DIE\n");
    puts("END --- dieWidget");
}

int sample_1() {
    puts("=== sample_1");
    try {
        std::shared_ptr<Widget> pw = std::make_shared<Widget>();
        ptr_lambda_debug<const char*, const Widget*>("pw addr is ", pw.get());
        auto f_live = std::async(std::launch::async, liveWidget, std::ref( pw ));
        auto f_die  = std::async(std::launch::async,  dieWidget, std::ref( pw ));
        f_live.get();
        f_die.get();
        /**
         * これがいいやり方なのか、よくあるコーディング技法なのかは判らない。
         * しかし、これで任意のオブジェクトの生存と消滅を管理できる。
         * スレッド管理されたオブジェクトは生存期間中は自由に振る舞える。
         * 例えば、何らかのシューティング・ゲームなら、移動と攻撃を繰り返す MOB に相当する。
         * 消滅スレッドは、プレーヤーの攻撃に対する当たり判定の結果というイメージだ。
        */
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int main(void) {
    puts("START Lost Chapter Concurrency ===");
    if(0.01) {
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_debug_and_error());
    }
    if(1.00) {
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", sample_1());
    }
    puts("=== Lost Chapter Concurrency END");
    return 0;
}