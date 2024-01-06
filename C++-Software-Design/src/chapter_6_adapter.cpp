/**
 * 6 章 Adapter パターン、Observer パターン、CRTP パターン
 * 
 * CRTP（Curiously Recurring Template Pattern）... 直訳 ... 奇妙なことに繰り返し発生するテンプレート パターン。
 * 
 * ガイドライン 24：インタフェースを統一するには Adapter パターン
 * 
 * 解明 Adapter パターン
 * Adapter パターンも古典的な GoF デザインパターンの 1 つです。インタフェースを統一し、既存
 * の階層構造に干渉せず機能を追加することを目的としています。
 * 
 * Adapter パターン
 * 目的：あるクラスのインタフェースを、クライアントが求める他のインタフェースへ変換する。
 *      Adapter パターンは、インタフェースに互換性のないクラス同士を組み合わせることができるようにする。
 * 
 * e.g. compile
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_6_adapter.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * Adapter パターンのサンプルは Document というクラスを例にする
 * @see 「ガイドライン 3」
*/

class JSONExportable {
public:
    virtual ~JSONExportable() = default;
    virtual void exportToJSON(/*...*/) const = 0; 
};

class Serializable {
public:
    virtual ~Serializable() = default;
    virtual void serialize(/*ByteStream& bs,*//*...*/) const = 0;   // 書籍では何らかのサードパーティ製のライブラリを利用していることを仮定している。
};

class Document : public JSONExportable, public Serializable {
public:
    virtual ~Document() = default;
    // ...
};

/**
 * ドキュメントを Pages 型式にする必要がでてきた。
 * その際オープンソースの Pages 型式実装が存在することを知り、利用するとした場合。
 * 次のクラスはサードパーティ製のものと仮定する。
*/

class OpenPages {
public:
    OpenPages() {}
    OpenPages(const OpenPages& own) {*this = own;}
    virtual ~OpenPages() {};

    void convertToByte() const {
        puts("--- OpenPages.convertToByte()");
    }
    // ...
};
void exportToJSONFormat(const OpenPages& op /*...*/) {
    puts("--- exportToJSONFormat()");
    op.convertToByte();
}

class Pages final : public virtual Document {
private:
    mutable OpenPages opages;
public:
    Pages():opages{} {}
    Pages(const Pages& own) {*this = own;}
    ~Pages() {}

    void exportToJSON() const override {
        puts("--- Page.exportToJSON()");
        exportToJSONFormat(opages);
    }
    void serialize() const override {
        puts("--- Page.serialize()");
        opages.convertToByte();
    }
};

int test_Pages() {
    puts("--- test_Pages");
    try {
        Pages pages;
        pages.exportToJSON();
        pages.serialize();
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

/**
 * オブジェクトアダプタとクラスアダプタ
 * Pages のようなクラスをオブジェクトアダプタとも言う。
 * ラッピングした型のインスタンスをを内部に保持するという意味です。
 * 最上位の基底クラスを指すポインタを保持する形もあります。この場合は、
 * 階層構造内にあるすべての型にオブジェクトアダプタを使用できるようになり、
 * 柔軟性が飛躍的に高まります。
 * 
 * 別の形として、クラスアダプタというものもあります。
 * 次にその実装を例に挙げます。
*/

class PagesV2 final : public Document, private OpenPages {  // Example of a class adapter.
public:
    void exportToJSON() const override {
        puts("--- PagesV2.exportToJSON()");
        exportToJSONFormat(*this);
    }
    void serialize() const override {
        puts("--- PagesV2.serialize()");
        this->convertToByte();
    }
};

int test_PagesV2() {
    puts("--- test_PagesV2");
    try {
        PagesV2 pages{};
        pages.exportToJSON();
        pages.serialize();
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

int main(void) {
    puts("START Adapter パターン ===");
    if(0.01) {
        double pi = 3.14159;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    if(1.00) {
        // オブジェクトアダプタ
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_Pages());
    }
    if(1.01) {
        // クラスアダプタ
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_PagesV2());
    }
    puts("=== Adapter パターン END");
    return 0;
}