/**
 * 5 章 右辺値参照、ムーブセマンティクス、完全転送
 * 
 * 項目 24 ：ユニバーサル参照と右辺値参照の違い
 * ※ユニバーサル参照とは著者の造語と聞いたことがあるがそんなことないのかな。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_5_move_forward_24.cpp -o ../bin/main
*/
#include <iostream>
#include <chrono>
#include <ctime>
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

void (*ptr_print_now)(void) = [](void) -> void {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::cout << std::ctime(&t) << std::endl;
};

int test_debug_and_error() {
    puts("=== test_debug_and_error");
    try {
        ptr_print_now();
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
 * ある型 T の右辺値参照を宣言するには T&& と記述します。このためソースコードに「T&&」と記述されているのを見れば、これは
 * 右辺値参照だと受け取れるのは当然に思えます。しかし、あぁなんとしたことか、事はそう簡単ではありません。
*/

class Widget {
public:
    Widget(): name{""}
    {}
    Widget(const std::string& _name): name{_name}
    {}
    // ...
    std::string getName() noexcept {
        return name;
    }
private:
    std::string name;
};

void foo(Widget&& param) {              // 右辺値参照
    puts("--- foo");
}

template<class T>
void bar(std::vector<T>&& param) {      // 右辺値参照
    puts("--- bar");
}

template <class T>
void buzz(T&& param) {                  // 右辺値参照ではない
    puts("--- buzz");
}

void sample() {
    puts("=== sample");
    Widget&& w1 = Widget("Chasire");    // 右辺値参照

    auto&& w2 = w1;                     // 右辺値参照ではない
    // Widget&& w3 = w1;                   // これだとコンパイルエラーになる：）
    ptr_lambda_debug<const char*,const std::string&>("(w2) name is ", w2.getName());
}

/**
 * 実際には「T&&」には 2 つの意味があります。1 つはもちろん右辺値参照です。この参照は期待通りに動作し、バインドするのは
 * 右辺値のみであり、その『レゾンデトル（存在理由）』はムーブ元になれるオブジェクトの表現にあります。
 * 「T&&」が持つもう 1 つの意味は、右辺値参照か左辺値参照の『どちらか』一方を表すことです。ソースコード上では右辺値参照
 * と見分けが付きませんが（どちらも「T&&」）、あたかもあたかも左辺値参照であるかのように振る舞えます（すなわち「T&」）。
 * この 2 つの性質を併せ持つため、（右辺値参照のように）右辺値にバインド可能であり、また（左辺値参照のように）左辺値にも
 * バインド可能です。
 * さらに、const オブジェクト、非 const オブジェクト、volatile オブジェクト、非 volatile オブジェクトにも、また、const 
 * かつ volatile なオブジェクトにもバインド可能です。実質的に『どんなもの』にもバインド可能です。このような前例のない
 * 柔軟性を備えた参照には専用の名前が相応しいでしょう。著者は『ユニバーサル参照』と呼んでいます。
 * ※『転送参照（forwarding reference）』と一般には呼ばれる。
*/

int main(void) {
    puts("START 項目 24 ：ユニバーサル参照と右辺値参照の違い ===");
    if(0.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_debug_and_error());
    }
    if(1.00) {
        sample();
    }
    puts("=== 項目 24 ：ユニバーサル参照と右辺値参照の違い END");
}