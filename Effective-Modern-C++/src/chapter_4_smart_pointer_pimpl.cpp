/**
 * 4 章 スマートポインタ
 * 
 * 項目 22 ：Pimpl イディオムを用いる際は特殊メンバ関数を定義する
 * 
 * 巨大なビルド時間と格闘した経験があるならば、『Pimpl イディオム』（Pointer to Implementation、実装を指すポインタ）もとっくに承知でしょう。
 * Pimpl イディオムとはクラスのメンバ変数を実装クラス（または、struct）に置き換える技法です。元クラスにあるメンバ変数を実装クラスに移動し、
 * このメンバ変数にはポインタを介して間接的にアクセスします。 
 * 
 * これ別の書籍「C++ ソフトウェア設計」でも語られてたな、忘れた：）
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_4_smart_pointer_pimpl.cpp -o ../bin/main
*/
#include <iostream>
#include <vector>

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](const auto message, const auto debug) -> void {
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
 * 例えば、次のような Widget があるとします。
 * 
 * // in header "widget.h"
 * 
 * class Widget {
 * public:
 *      Widget();
 * private:
 *      std::string name;
 *      std::vector<double> data;
 *      Gadget g1, g2, g3;          // Gadget は何らかのユーザ定義クラス
 * };
 * 
 * Widget のメンバ変数の型は std::string, std::vector<double>, Gadget であるため、Widget をコンパイルするには
 * これらの型の存在がみえていなければなりません。すなわち、Widget を使用するコードでは <string> <vector> gadget.h
 * を #include する必要があり、このヘッダに起因し Widget 利用側のコンパイル時間は延び、また Widget 利用側のコード
 * はヘッダ内容に依存することになります。ヘッダの内容が変更されれば、Widget 利用側のコードはリコンパイルしなければ
 * なりません。標準ヘッダでは頻繁に変更されませんが、gadget.h の変更は頻繁になる恐れがあります。
 * 
 * この Widget に C++98 の Pimpl イディオムを適用してみましょう。メンバ変数は宣言したけれど定義はしていない struct 
 * を指す raw ポインタに置き換えます。
*/

class Gadget {

};

/**
 * 本来はヘッダとソースが分かれるので次のようになると考えてください。
 * 
 * #include "widget.h"
 * #include "gadget.h"
 * #include <string>
 * #include <vector>
*/

// Widget 宣言 widget.h に記述すべきこと
class Widget {
private:
    struct Impl;
    Impl* pImpl;
public:
    Widget();
    ~Widget();
};

// Widget 定義 widget.cpp
struct Widget::Impl {
    std::string name;
    std::vector<double> data;
    Gadget g1, g2 ,g3;
};

Widget::Widget() : pImpl(new Impl)
{}
Widget::~Widget() {
    delete pImpl;
}


/**
 * ソースファイルとヘッダファイルを分けた書き方では次の定義になる。
 * 
struct Widget::Impl {
    std::string name;
    std::vector<double> data;
    Gadget g1, g2 ,g3;
};
 * 
*/


int main(void) {
    puts("START 項目 22 ：Pimpl イディオムを用いる際は特殊メンバ関数を定義する ===");
    if(0.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_debug());
    }
    puts("=== 項目 22 ：Pimpl イディオムを用いる際は特殊メンバ関数を定義する END");
    return 0;
}
