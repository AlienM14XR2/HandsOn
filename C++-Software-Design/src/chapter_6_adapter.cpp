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

class Page final : public virtual Document {
private:
    mutable OpenPages opages;
public:
    Page():opages{} {}
    Page(const Page& own) {*this = own;}
    ~Page() {}

    void exportToJSON() const override {
        puts("--- Page.exportToJSON()");
        exportToJSONFormat(opages);
    }
    void serialize() const override {
        puts("--- Page.serialize()");
        opages.convertToByte();
    }
};

int test_Page() {
    puts("--- test_Page");
    try {
        Page page;
        page.exportToJSON();
        page.serialize();
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
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_Page());
    }
    puts("=== Adapter パターン END");
    return 0;
}