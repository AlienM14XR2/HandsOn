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
 * std::shared_ptr を採用する余裕があるかどうか疑問に感じるならば、共同所有権が本当に必要か否かを見直すのが良いでしょう。
 * 独占所有権でも良い、または良いだろうと見込めるならば、std::unique_ptr の方が適切です。
 * また、std::shared_ptr は std::unique_ptr からも作成できるため、「アップグレード」は容易です。
 * しかし、 その逆は真ではありません。
 * 
 * std::shared_ptr ではできないことは他にもあります。配列の場合がそうです。 std::unique_ptr とのもうひとつの差異になりますが、
 * std::shared_ptr には単一オブジェクトを指すポインタ用に設計された API しかなく、std::shared_ptr<T[]> などはありません。
 * 最も重要な点は、組み込み配列ライクなさまざまな C++11 オブジェクトがありながら（std::array, std::vector, std::string）、単純配列
 * を指すスマートポインタを宣言するなどは、まず間違いなく悪い設計の証だということです。
 * 
 * 重要ポイント
 * - std::shared_ptr は、任意の共有リソースのライフタイム管理をガーベジコレクションに近付ける、有用なものである。
 * - std::shared_ptr オブジェクトにサイズは、std::unique_ptr の 2 倍になるのが通例である。また、コントロールブロックに由来する
 *   オーバーヘッドが発生し、レファレンスカウントのアトミック演算も必要になる。
 * - リソース破棄時にデフォルトで実行されるのは delete だが、カスタムデリータも指定可能である。デリータの型は std::shared_ptr の型に影響しない。
 * - raw ポインタ型の変数から std::shared_ptr を作成するのは避けるべきである。
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

/**
 * std::shared_ptr オブジェクトは、対象オブジェクトのレファレンスカウントを指すポインタを保持します。
 * 厳密に言えば、std::shared_ptr が管理するオブジェクトそれぞれにつき『コントロールブロック』というデータがあり、
 * レファレンスカウントはコントロールブロックの一部です、コントロールブロックには他にも、『weak カウント』『カスタムデリータ』
 * 『カスタムアロケータ』などがあります。
*/

class WidgetV2;
std::vector<std::shared_ptr<WidgetV2>> processedWidgets;

class WidgetV2 {
public:
    void process() {
        processedWidgets.emplace_back(this);    // 処理済みの WidgetV2 をリストへ追加。誤り！！
    }
};

int test_WidgetV2_Bad_Coding() {
    puts("=== test_WidgetV2_Bad_Coding");
    WidgetV2* wv2 = nullptr;
    try {
        /**
         * 上例の、誤りというコメントがすべてです。
         * emplace_back の使用ではなく、this を与えている点です。上例はコンパイル可能ですが、std::shared_ptr のコンテナに raw ポインタ（this）
         * を渡しているため、std::shared_ptr をコンストラクトすることにより、WidgetV2 を指すポインタ（*this）用のコントロールブロックが新規に
         * 作成されてしまいます。無害に見えるコードかもしれませんが、そう思うのも同じ WidgetV2 を指す std::shared_ptr がメンバ関数外にすでに
         * 存在しているかもしれないと気づくまでです。これで試合終了。勝者は未定義動作です。
        */
        wv2 = new WidgetV2();
        wv2->process();
        for(auto w: processedWidgets) {
            ptr_lambda_debug<const char*,WidgetV2*>("w addr is ", w.get());
        }
        if(wv2) {
            delete wv2;
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        if(wv2) {
            delete wv2;
        }
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * std::shared_ptr にはまさにこの種の状況に対応するものがあります。恐らく、標準 C++ ライブラリの中でも最も変わった名前だと思いますが、
 * std::enable_shared_from_this です。これは、this ポインタから安全に std::shared_ptr を作成可能とし、std::shared_ptr が管理するクラス
 * が必要な場合に使用する、継承用クラステンプレートです。
 * 
 * class WidgetV2 : public std::enable_shared_from_this<WidgetV2> {
 * public:
 *      void proceed();
 * };
 * 
 * これが『奇妙に再帰したテンプレートパターン（Curiously Recuring Template Pattern, CRTP）』という標準名も付けられています。
*/

class WidgetV3;
std::vector<std::shared_ptr<WidgetV3>> processedWidgetV3s;

class WidgetV3 : public std::enable_shared_from_this<WidgetV3>{
public:
    void process() {
        processedWidgetV3s.emplace_back(shared_from_this());
    }  
};

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
    if(0) {
        // 二重解放によるコアダンプで終了する。
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_WidgetV2_Bad_Coding());
    }
    puts("=== 項目 19 ：共有するリソースの管理には std::shared_ptr を用いる END");
    return 0;
}