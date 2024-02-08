/**
 * 4 章 スマートポインタ
 * 
 * 項目 19 ：共有するリソースの管理には std::shared_ptr を用いる
 * 
 * ガーベジコレクションのように自動的に動作し、デストラクタのようにすべての種類のリソースを対象とし、予測可能な
 * タイミングで動作するものは実現できないのでしょうか ？
 * 
 * std::shared_ptr は、双方をつなぎ合わせる、C++11 での方法です。std::shared_ptr を介し使用するオブジェクトのライフタイムは、
 * 『共同所有権』を備えたポインタにより管理されます。特定の std::shared_ptr がオブジェクトを排他的に所有する訳ではなく、その
 * オブジェクトを指す std::shared_ptr がすべて協調し、オブジェクトが不要になった時点での破棄を保証します。そのオブジェクトを
 * 指す、最後の std::shared_ptr がオブジェクトを指さなくなると（std::shared_ptr が破棄された、他のオブジェクトを指すように変更されたなど）、
 * その std::shared_ptr がオブジェクトを破棄します。ガベージコレクション同様に、コード利用者は自身で対象オブジェクトのライフ
 * タイムを管理する必要はありません。それでいてデストラクタ同様に、オブジェクトを破棄するタイミングは決定性を持つ動作となります。
 * 
 * std::shared_ptr は【レファレンスカウント】を使用して、自身がそのリソースを指す最後の std::shared_ptr か否かを判断できます。
 * レファレンスカウントの存在は性能に影響を与えます。
 * 
 * - 【std::shared_ptr のサイズが raw ポインタの 2 倍になる。】
 *   リソースを指す raw ポインタと、リソースのレファレンスカウントを指す raw ポインタを内部に持つためである。
 * - 【レファレンスカウントをダイナミックにメモリ割当しなければならない。】
 * - 【レファレンスカウントのインクリメント／デクリメントはアトミックに実行しなければならない。】
 * 
 * std::shared_ptr から別の std::shared_ptr をムーブコンストラクトすると、元の std::shared_ptr はヌルになります。
 * つまり、新 std::shared_ptr が誕生した時点から、元の std::shared_ptr はリソースを指さなくなるため、レファレンス
 * カウントの操作は不要になるのです。std::shared_ptr のムーブはコピーよりも高速です。コピーの場合ではレファレンス
 * カウントをインクリメントしなければなりませんが、ムーブの場合は不要です。
 * ムーブコンストラクタがコピーコンストラクタよりも高速であるのと同様に、ムーブ代入もコピー代入より高速です。
 * 
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_4_smart_pointer_shared_ptr.cpp -o ../bin/main
*/
#include <iostream>
#include <memory>
#include <vector>

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](const auto message, const auto debug) -> void {
    std::cout << "DEBUG: " << message << '\t' << debug << std::endl;
};

template <class Error>
concept Reasonable = requires(Error& e) {
    e.what();
};
template <class Error>
requires Reasonable<Error>
void (*ptr_print_error)(Error) = [](const auto e) -> void {
    std::cout << "ERROR: " << e.what() << std::endl;
};

int test_debug() {
    puts("=== test_debug");
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

/**
 * std::unique_ptr 同様に std::shared_ptr もカスタムデリータを利用できるがその扱い方が異なる。
 * std::unique_ptr ではデリータの型がスマートポインタの型の一部になるのに対し、std::shared_ptr ではそうなりません。
*/

class Widget {
public:
    void doSomething() noexcept {
        puts("------ doSomething");
        ptr_lambda_debug<const char*,Widget*>("this addr is ", this);
    }
};

void makeLogEntry(const Widget* const pw) {
    puts("--- makeLogEntry");
    ptr_lambda_debug<const char*,const Widget*>("widget addr is ", pw);
}

auto loggingDel = [](Widget* pw) {
    puts("--- loggingDel");
    makeLogEntry(pw);
    delete pw;
};

int test_custom_deleter() {
    puts("=== test_custom_deleter");
    try {
        // カスタムデリータは型の一部として宣言される。
        std::unique_ptr<Widget,decltype(loggingDel)> upw{new Widget(), loggingDel};
        ptr_lambda_debug<const char*,Widget*>("upw widget addr is ", upw.get());

        // カスタムデリータは型の一部ではない。
        std::shared_ptr<Widget> spw{new Widget(), loggingDel};
        ptr_lambda_debug<const char*,Widget*>("spw widget addr is ", spw.get());
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * std::shared_ptr の設計の方が柔軟性に優れています。std::shared_ptr<Widget> が 2 つあるとしましょう。
 * それぞれが異なる型のカスタムデリータを持つとします（ラムダ式を用いカスタムデリータを指定したなど）。
*/

auto customDeleter1 = [](Widget* pw) {
    puts("--- customDeleter1");
    ptr_lambda_debug<const char*,Widget*>("pw addr is ", pw);
    delete pw;
};

auto customDeleter2 = [](Widget* pw) {
    puts("--- customDeleter2");
    ptr_lambda_debug<const char*,Widget*>("pw addr is ", pw);
    delete pw;
};

int test_shared_ptr_and_custom_deleter() {
    puts("=== test_shared_ptr_and_custom_deleter");
    try {
        using SharedWidget = std::shared_ptr<Widget>;
        SharedWidget pw1{new Widget(),customDeleter1};
        SharedWidget pw2{new Widget(),customDeleter2};
        /**
         * pw1 と pw2 の型は同じため、同じコンテナに持たせられます。
        */
        std::vector<SharedWidget> vec{pw1, pw2};
        for(auto w: vec) {
            w.get()->doSomething();
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int main(void) {
    puts("START 項目 19 ：共有するリソースの管理には std::shared_ptr を用いる ===");
    if(0.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_debug());
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_custom_deleter());
    }
    if(1.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_shared_ptr_and_custom_deleter());
    }
    puts("=== 項目 19 ：共有するリソースの管理には std::shared_ptr を用いる END");
    return 0;
}