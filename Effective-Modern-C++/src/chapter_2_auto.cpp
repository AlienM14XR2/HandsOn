/**
 * 2 章 auto
 * 
 * 推論規則に完全に準拠しているにも関わらず、プログラマからみて完全に誤っていることもあります。
 * この場合でも、従来の手続きによる型宣言に戻るなどは可能な限り避けるのが最善であり、auto が型
 * を正しく推論できるようにする方法を把握しておくことが重要です。
 * 
 * ```
 * 個人的感想、意見。
 * そこまで型を意識しない方がいいとする理由がほしい。
 * この章を進めていけば納得できるのだろうか。
 * ```
 * 
 * 項目 5 ：明示的型宣言よりも auto を優先する
 * 
 * e.g.) compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_2_auto.cpp -o ../bin/main
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall chapter_2_auto.cpp -o ../bin/main
*/
#include <iostream>
#include <memory>
#include <vector>
#include <functional>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * auto は初期化子がないとコンパイルエラーになる。
 * つまり、初期化忘れや未定義の状態をなくせる。
*/
void sample() {
    puts("--- sample");
    // int x;          // 未初期化のおそれ ... 私はコンパイルオプションで -Wall -Werror を指定しているのでこれもエラーになった。
                    // error: ‘x’ may be used uninitialized [-Werror=maybe-uninitialized]

    auto x2 = 3;    // 値 3 が確実に定義されている
    // auto x3;     // エラー、初期化子がない   error: declaration of ‘auto x3’ has no initializer

    // ptr_lambda_debug<const char*,const decltype(x)&>("x is ",x);
    ptr_lambda_debug<const char*,const decltype(x2)&>("x2 is ",x2);
    // ptr_lambda_debug<const char*,const decltype(x3)&>("x is ",x3);
}

/**
 * 個人的な演習
 * 
 * 読書に飽きたので、少しだけ Coffee Break。
 * 
 * GoF 古典からの第一歩を探して。
 * 第 2 節 Decorator パターンを考える。
 * 
 * - 継承は必要最小限する。
 * - 具体的な実装詳細は別クラスにする。
 * - 操作はインタフェースで行う。
 * - 必要な機能はコンポジションする。
 * - Decorator パターンを考える。（今回はここ）
*/

/**
 * 構文解析 Strategy クラステンプレート
 * Strategy パターンの基底クラス。
*/
template <class T>
class ParseStrategy {
public:
    virtual ~ParseStrategy() = default;
    // ...
    virtual void parse(T&) = 0;
    // virtual std::unique_ptr<ParseStrategy<T>> clone() const = 0;
};

/**
 * データアクセス（CRUD）に関する Strategy パターン クラステンプレート
 * Strategy パターンの基底クラス。
*/
template <class T>
class DataAccessStrategy {
public:
    virtual ~DataAccessStrategy() = default;
    virtual void access(T&) = 0;
    // virtual std::unique_ptr<DataAccessStrategy<T>> clone() const = 0;
};

/**
 * 共通インタフェース
 * 共通処理の定義を行う。
*/
class Repository {      // 言うなれば単なるタグ
public:
    virtual ~Repository() = default;
    virtual void parse() = 0;
    virtual void access() = 0;
};

/**
 * 登録処理クラス
*/
class Insert final : public Repository {
private:
    // std::unique_ptr<ParseStrategy<Insert>> parser;
    // std::unique_ptr<DataAccessStrategy<Insert>> accessor;
    ParseStrategy<Insert>* parser;
    DataAccessStrategy<Insert>* accessor;
    Insert() : parser{nullptr}, accessor{nullptr} {}
public:
    // 外部から好きな Parser や Accessor を 依存注入できる。
    Insert(ParseStrategy<Insert>& _parser, DataAccessStrategy<Insert>& _accessor) : parser{std::move(&_parser)}, accessor{std::move(&_accessor)}
    {}
    Insert(const Insert& own) {*this = own;}
    ~Insert() {}
    // ...
    void parse() override {
        parser->parse(*this);
    }
    void access() override {
        accessor->access(*this);
    }
};

/**
 * 登録構文解析クラス A
*/
class InsertParser final : public ParseStrategy<Insert> {
private:
    ParseStrategy<Insert>* parser = nullptr;
public:
    InsertParser(): parser{nullptr} {}
    InsertParser(ParseStrategy<Insert>& _parser): parser{std::move(&_parser)} 
    {}
    InsertParser(ParseStrategy<Insert>* _parser): parser{_parser}
    {}
    InsertParser(const InsertParser& own) {*this = own;}
    ~InsertParser() {}
    // ...
    void parse(Insert& insert) override {
        if(parser) {
            parser->parse(insert);
        }
        puts("------ TODO Insert parse");
    }
};

/**
 * 登録データアクセスクラス
*/
class InsertAccessor final : public DataAccessStrategy<Insert> {
public:
    void access(Insert& insert) override {
        puts("------ TODO Insert access");
    }
};

int test_Insert() {
    puts("--- test_Insert");
    try {
        InsertParser parser;
        InsertAccessor accessor;
        std::unique_ptr<Repository> insert = std::make_unique<Insert>(Insert{
            parser
            , accessor
        });
        insert->parse();
        insert->access();
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what()  << endl;
        return EXIT_FAILURE;
    }
}

/**
 * ここまでが前回で実装したこと。
 * 次はこれに Decorator パターンを組み込みたい。
 * 私の理解では、Decorator パターンは平たく言えば、関数の再帰呼び出しだ。
*/

/**
 * 登録構文解析クラス B
*/
class InsertSyntaxParser final : public ParseStrategy<Insert> {
private:
    ParseStrategy<Insert>* parser = nullptr;
public:
    InsertSyntaxParser(): parser{nullptr} {}
    InsertSyntaxParser(ParseStrategy<Insert>& _parser): parser{std::move(&_parser)}
    {}
    InsertSyntaxParser(ParseStrategy<Insert>* _parser): parser{_parser}
    {}
    InsertSyntaxParser(const InsertSyntaxParser& own) {*this = own;}
    ~InsertSyntaxParser() {}
    // ...
    void parse(Insert& insert) override {
        if(parser) {
            parser->parse(insert);
        }
        puts("--------- TODO Insert Syntax parse");
    }
};

int test_Insert_V2() {
    puts("--- test_Insert_V2");
    try {
        std::unique_ptr<InsertSyntaxParser> systaxParserA = std::make_unique<InsertSyntaxParser>(InsertSyntaxParser{});
        std::unique_ptr<InsertSyntaxParser> systaxParserB = std::make_unique<InsertSyntaxParser>(InsertSyntaxParser{systaxParserA.get()});
        std::unique_ptr<ParseStrategy<Insert>> parserA = std::make_unique<InsertParser>(InsertParser{systaxParserB.get()});

        InsertParser parser{parserA.get()};
        InsertAccessor accessor;
        std::unique_ptr<Repository> insert = std::make_unique<Insert>(Insert{
            parser
            , accessor
        });
        insert->parse();
        /**
         * Decorator パターンの基本的な考え方としてはあってると思う。
         * 実際には異なる処理単位のラッピングにはなる。
        */
        insert->access();
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

template <class Ite>
void dwim(Ite* b) {       // dwim ( "do what i mean" ) こちらの意図通りに実行せよ ... 数値型であればすべて 0 で初期化する。
    puts("------ dwim");
    size_t i = 0;
    for( auto curValue: *b) {   // ここで auto を利用している。
       curValue *= 0;
        (*b)[i] = curValue;
        i++;
    }
}

template <class Ite>
void dwimRef(Ite& b) {       // dwim ( "do what i mean" ) こちらの意図通りに実行せよ ... 数値型であればすべて 0 で初期化する。
    puts("------ dwimRef");
    size_t i = 0;
    for( auto curValue: b) {    // ここで auto を利用している。
       curValue *= 0;
        (b)[i] = curValue;
        i++;
    }
}

int test_dwim() {
    puts("--- test_dwim");
    try {
        vector<int> vec = {1,2,3,4,5};
        dwim(&vec);
        for(int value: vec) {
            ptr_lambda_debug<const char*,const int&>("value is ",value);
        }
        vector<int> vec2 = {10,20,30,40,50};
        dwimRef(vec2);
        for(int value: vec2) {
            ptr_lambda_debug<const char*,const int&>("value is ",value);
        }
        // 以下は個人的興味だね ... これはコンパイルエラーになった。error: no match for ‘operator*=’ (operand types are ‘std::__cxx11::basic_string<char>’ and ‘int’)
        // vector<string> vec3 = {"Jack", "Derek", "Alice"};
        // dwimRef(vec3);
        // for(string value: vec3) {
        //     ptr_lambda_debug<const char*,const string&>("value is ",value);
        // }
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

/**
 * auto は型を推論するため（項目 2）、コンパイラにしか分からない型でも表現できます。
*/

template <class T>
auto (*ptr_lambda_upless)(T& lhs,T& rhs) = [](const auto& p1, const auto& p2) -> auto {
    // ポインタライクならば、どんなものでもその指す値を比較する C++14 関数
    return *p1 < *p2;
};

template<class T>
auto f_upless(T* lhs, T* rhs) {
    return *lhs < *rhs;     // 真偽判定は bool を返す
}

int test_ptr_lambda_upless() {
    puts("--- test_ptr_lambda_upless");
    try {
        int x = 30;
        int y = 60;
        // auto ret = ptr_lambda_upless<const int>(&x,&y);    // これがうまく動作しない。
        auto ret2 = f_upless(&y, &x);
        ptr_lambda_debug<const char*,decltype(ret2)&>("ret2 is ", ret2);
        ptr_lambda_debug<const char*,const char*>("ret2 type is ", typeid(ret2).name());

        auto upless = [](const auto& p1, const auto& p2) {
            // ポインタライクならば、どんなものでもその指す値を比較する C++14 関数
            return *p1 < *p2;
        };
        auto ret3 = upless(&x,&y);
        ptr_lambda_debug<const char*,decltype(ret3)&>("ret3 is ", ret3);
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

/**
 * std::function とは、関数ポインタの概念を一般化した、C++11 標準ライブラリのテンプレートです。
 * 関数ポインタが関数しか指せないのに対し、std::function オブジェクトは呼び出し可能なオブジェクト、
 * すなわち関数のように実行できるものならば何でも参照できます。関数ポインタの作成時にはその関数の型
 * を指定しなければならないのと同様に（対象関数に共通するシグネチャ）、std::function オブジェクト作
 * 成時にも関数の型を指定しなければなりません。これは std::function のテンプレート仮引数を用います。
*/

class Widget {
private:
    int x;
public:
    Widget():x{0} {}
    Widget(const int& _x): x{_x} {}
    Widget(const Widget& own) {*this = own;}
    ~Widget() {}
    // ...
    int getX() noexcept {return x;}
    bool operator<(const Widget& rhs) {
        return x < rhs.x;
    }
};

/**
 * 案外不親切な本だから、上記と下記は著者の意図を汲むかたちで独自に実装した。
 * つまり、サンプルはなかった。
*/

std::function<bool(const std::unique_ptr<Widget>&, const std::unique_ptr<Widget>&)> funcComp;

int test_funcComp() {
    puts("--- test_funcComp");
    try {
        // ラムダ式を std::function オブジェクトの funcComp に設定している。
        funcComp = [](const std::unique_ptr<Widget>& lhs, const std::unique_ptr<Widget>& rhs)-> bool{
            return *(lhs.get()) < *(rhs.get());
        };

        std::unique_ptr<Widget> w1 = std::make_unique<Widget>(Widget{});
        std::unique_ptr<Widget> w2 = std::make_unique<Widget>(Widget{30});
        bool ret = funcComp(w1,w2);
        ptr_lambda_debug<const char*,const bool&>("ret is ",ret);
        ret = funcComp(w2,w1);
        ptr_lambda_debug<const char*,const bool&>("ret is ",ret);
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

/**
 * 著者の意見を簡潔にまとめると、std::function は実行速度、メモリ使用量の観点から、ラムダ式や auto に劣る。
*/

int main(void) {
    puts("START 2 章 auto ===");
    if(0.01) {
        auto pi = 3.141592;
        ptr_lambda_debug<const char*,const decltype(pi)&>("pi is ",pi);
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_Insert());
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_Insert_V2());
    }
    if(1.00) {
        sample();
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_dwim());
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_ptr_lambda_upless());
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_funcComp());
    }
    puts("=== 2 章 auto END");
    return 0;
}
