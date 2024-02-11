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
#include <memory>

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

// Widget 宣言 widget.h に記述すべきこと
class Widget {
private:
    struct Impl;
    Impl* pImpl;
public:
    Widget();
    ~Widget();
};

/**
 * 本来はヘッダとソースが分かれるので次のようになると考えてください。
 * 
 * #include "widget.h"
 * #include "gadget.h"
 * #include <string>
 * #include <vector>
*/

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
 * これで、Widget は std::string std::vecotor Gadget を直接使用しなくなったため、これらのヘッダを Widget の利用者が #include する
 * 必要もなくなりました。コンパイル時間を短縮できるばかりか、これらの型のヘッダに何らかの変更が加えられても、Widget 利用者は影響を
 * 受けません。
 * 宣言はしたけど定義をしない型を指し、『不完全型』と言います。上例の Widget::Impl はまさに不完全型です。 不完全型に対し直接できる
 * ことはほどんどありませんが、そのポインタは宣言できます。Pimpl イディオムではこの点を活用しています。
 * Pimpl イディオムの第一部は、不完全型を指すポインタをメンバ変数として宣言することです。第二部は、元クラスにあったメンバ変数を保持
 * するオブジェクトの、ダイナミックな割当／解放です。割当／解放コードはヘッダファイルではなく、実装を記述するソースコードに記述します。
 * 
 * 上例では std::string std::vector Gadget の各ヘッダには変わらず依存していることを示すために、敢えて #include ディレクティブを残して
 * あります。しかし、この依存性は widget.h から widget.cpp へ移動しています（Widget を利用するものからは見えない）。Pimpl オブジェクト
 * の割当／解放は忘れてはいけません。
 * 
 * 本章の主眼は raw ポインタよりスマートポインタの方が望ましいと示すことにあります。したがって、次のようになります。
*/

// Widget 宣言 widget.h に記述すべきこと
class WidgetV2 {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
public:
    WidgetV2();
    WidgetV2(const std::string& _name);
    // 私の環境では次の定義は不要だった。そもそもソースにすべて記述しているため。
    ~WidgetV2();
    // デストラクタを明示した際は、ムーブの特殊メンバ関数の自動生成が抑制される、利用する場合は明示しないといけない。
    WidgetV2(WidgetV2&& rhs);
    WidgetV2& operator=(WidgetV2&& rhs);
    // スマートポインタはコピーではなくムーブを利用する、ムーブ専用型。

    /**
     * 必要があれば、コピー演算もヘッダで宣言し、ソースで定義する
    */
    WidgetV2(const WidgetV2& own);
    WidgetV2 operator=(const WidgetV2& own);

    // ...
    std::unique_ptr<Impl>& getResource() {
        return pImpl;
    }
};

/**
 * 本来はヘッダとソースが分かれるので次のようになると考えてください。
 * 
 * #include "widget.h"
 * #include "gadget.h"
 * #include <string>
 * #include <vector>
*/

// Widget 定義 widget.cpp
struct WidgetV2::Impl {
    std::string name;
    std::vector<double> data;
    Gadget g1, g2 ,g3;
};

WidgetV2::WidgetV2() : pImpl(std::make_unique<Impl>())
{
    ptr_lambda_debug<const char*,const char*>("pImpl type is ", typeid(pImpl).name());
    ptr_lambda_debug<const char*,const char*>("*pImpl type is ", typeid(*pImpl).name());
    ptr_lambda_debug<const char*,Impl*>("*pImpl addr is ", pImpl.get());
    ptr_lambda_debug<const char*,Impl*>("*pImpl addr is ", &(*pImpl));
    // pImpl.get()->data;
    // (*pImpl).data;
}
WidgetV2::WidgetV2(const std::string& _name) : pImpl{std::make_unique<Impl>()}
{
    pImpl.get()->name = _name;
    ptr_lambda_debug<const char*,const char*>("pImpl type is ", typeid(pImpl).name());
    ptr_lambda_debug<const char*,const char*>("*pImpl type is ", typeid(*pImpl).name());
    ptr_lambda_debug<const char*,Impl*>("*pImpl addr is ", pImpl.get());
    ptr_lambda_debug<const char*,Impl*>("*pImpl addr is ", &(*pImpl));
}
// 解放は std::uniqu_ptr が行う。
// WidgetV2::~Widget() {
//     delete pImpl;
// }

// 私の環境では次の定義は不要だった。そもそもソースにすべて記述しているため。
WidgetV2::~WidgetV2() = default;                            // デストラクタ
WidgetV2::WidgetV2(WidgetV2&& rhs) = default;               // ムーブコンストラクタ
WidgetV2& WidgetV2::operator=(WidgetV2&& rhs) = default;    // ムーブ代入演算子
WidgetV2::WidgetV2(const WidgetV2& own) //: pImpl(std::make_unique<Impl>(*own.pImpl))   // コピーコンストラクタ
{
    if(own.pImpl.get()) {
        pImpl = std::make_unique<Impl>(*own.pImpl);
    }
}
WidgetV2 WidgetV2::operator=(const WidgetV2& own)           // コピー代入演算子
{
    if(own.pImpl.get()) {
        *pImpl = *own.pImpl;
    }
    return *this;
}

/**
 * 上記コピー演算を眺め（観察し）て、正して置かなければいけない問題が私にはある。
 * スマートポインタは ポインタ型なのか オプジェクト型なのか、何を言っているかわからないだろうが
 * 私の今の疑問だ、これをはっきりさせないと、スマートポインタを正しく扱えない。
 * 
 * アスタリスクを付けるとスマートポインタに指定したオブジェクトが取り出せる。
 * なしでは、スマートポインタ型。
 * 納得した、上記のコピー演算では中身（Impl）を取り出している。
*/

int test_WidgetV2() {
    puts("=== test_WidgetV2");
    try {
        WidgetV2 w;
        auto pImpl = std::move(w.getResource());
        ptr_lambda_debug<const char*,const char*>("pImpl type is ", typeid(pImpl).name());
        ptr_lambda_debug<const char*,const char*>("*pImpl type is ", typeid(*pImpl).name());
        ptr_lambda_debug<const char*,const decltype(pImpl.get())>("(w) WidgetV2::Impl addr is ", pImpl.get());

        puts("--- Before construct w2");
        WidgetV2 w2;
        puts("--- Before move construct w3");
        WidgetV2 w3 = std::move(w2);
        auto pImpl3 = std::move(w3.getResource());
        ptr_lambda_debug<const char*,const decltype(pImpl3.get())>("(w3) WidgetV2::Impl addr is ", pImpl3.get());
        auto pImpl2 = std::move(w2.getResource());
        ptr_lambda_debug<const char*,const decltype(pImpl2.get())>("(w2) WidgetV2::Impl addr is ", pImpl2.get());
        puts("--- Before construct w4");
        WidgetV2 w4;
        puts("--- Before move operator=() w2");
        w2 = std::move(w4);
        auto pImpl4 = std::move(w2.getResource());
        ptr_lambda_debug<const char*,const decltype(pImpl4.get())>("(w2) WidgetV2::Impl addr is ", pImpl4.get());

        puts("--- Before construct w5");
        WidgetV2 w5("Derek");
        puts("--- Before copy construct w6");
        WidgetV2 w6 = w5;
        auto pImpl5 = std::move(w6.getResource());
        ptr_lambda_debug<const char*,const decltype(pImpl5.get())>("(w6) WidgetV2::Impl addr is ", pImpl5.get());
        ptr_lambda_debug<const char*,const decltype(pImpl5.get()->name)&>("(w6) WidgetV2::Impl name is ", pImpl5.get()->name);
        puts("--- Before construct w7");
        WidgetV2 w7("Alice");
        puts("--- Before construct w8");
        WidgetV2 w8;
        puts("--- Before copy operator=() w8");
        w8 = w7;
        auto pImpl6 = std::move(w8.getResource());
        ptr_lambda_debug<const char*,const decltype(pImpl6.get())>("(w8) WidgetV2::Impl addr is ", pImpl6.get());
        ptr_lambda_debug<const char*,const decltype(pImpl6.get()->name)&>("(w8) WidgetV2::Impl name is ", pImpl6.get()->name);

        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * 上記サンプルの WidgetV2 は Pimpl イディオムを std::unique_ptr で宣言しているため、本来コンパイラが自動生成する
 * 特殊メンバ関数を宣言、定義してあげる必要があった。WidgetV2::Impl はメンバ変数なので専有目的の std::unique_ptr 
 * を用いることは正しい、しかし、次のパターンでは、敢えて、std::shared_ptr で Pimpl イディオムを宣言し、コンパイラ
 * の自動生成の利用を可能にしている。プログラマのコード量を減らすことができる。
*/


class WidgetV3 {
private:
    struct Impl;
    std::shared_ptr<Impl> pImpl;
public:
    WidgetV3();
    WidgetV3(const std::string& _name);
    // ...
    std::string getName() noexcept;
};

struct WidgetV3::Impl {
    std::string name;
    std::vector<double> data;
    Gadget g1, g2 ,g3;
};

WidgetV3::WidgetV3(): pImpl{std::make_shared<Impl>()}
{}
WidgetV3::WidgetV3(const std::string& _name): pImpl{std::make_shared<Impl>()}
{
    pImpl.get()->name = _name;
}
std::string WidgetV3::getName() noexcept {
    return pImpl.get()->name;
}

int test_WidgetV3() {
    puts("=== test_WidgetV3");
    try {
        WidgetV3 w1("Jack");
        WidgetV3 w2 = std::move(w1);
        ptr_lambda_debug<const char*,const std::string&>("(w2) name is ", w2.getName());
        // ptr_lambda_debug<const char*,const std::string&>("(w1) name is ", w1.getName());     // 未定義の動作でコアダンプになった。
        /**
         * std::shared_ptr は 専有しないので、コピー演算もコンパイラが自動生成してくれる。
        */
        w1 = w2;
        ptr_lambda_debug<const char*,const std::string&>("(w1) name is ", w1.getName());
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;        
    }
}

int main(void) {
    puts("START 項目 22 ：Pimpl イディオムを用いる際は特殊メンバ関数を定義する ===");
    if(0.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_debug());
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_WidgetV2());
    }
    if(1.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_WidgetV3());
    }
    puts("=== 項目 22 ：Pimpl イディオムを用いる際は特殊メンバ関数を定義する END");
    return 0;
}
