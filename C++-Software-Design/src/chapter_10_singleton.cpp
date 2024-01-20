/**
 * 10 章 Singleton パターン
 * 
 * 読者がすでに Singleton パターンに精通しており、自分なりの確固たる意見を持っているだろうとは承知しています。
 * Singleton パターンをアンチパターンとみなしており、本書が敢えて取り上げたのはどうしてだろうと疑問を感じる
 * かもしれません。
 * 
 * C++ 標準ライブラリに Singleton パターンに似たインスタンスがいつくかあると分かると大いに驚くかもしれません。
 * Singleton パターンとは何か、いつ動作するか、正しく付き合うにはどうするのが良いかについて解説しましょう。
 * 
 * ガイドライン 37 ：Singleton パターンはデザインパターンではなく実装パターンとみなす
 * Singleton パターンはデザインパターンではない。
 * 
 * Singleton パターン
 * 目的：あるクラスに対してインスタンスが 1 つしか存在しないことを保証し、
 *       それにアクセスするためのグローバルな方法を提供する。
 * 
 * 著者の指摘、デザインパターンの性質の再掲、ガイドライン 11
 * - 名前を持つ
 * - 明確な目的を持つ
 * - 抽象化する
 * - 検証済み
 * 
 * Singleton パターンは依存関係を管理も軽減もしない
 * Singleton パターンは抽象化を導入も表現もしていない。
 * Singleton パターンが目的にしているのはインスタンス数を 1 つに制限することです。このため Singleton はデザインパターンではなく
 * 実装パターンに過ぎません。
 * 
 * ガイドライン 37 の要約
 * - Singleton パターンは依存関係の軽減も管理も目的としておらず、デザインパターンに期待される内容を満たしていない。
 * - 特定クラスのインスタンス数を厳密に 1 つに制限する場合に用いるのが Sigleton パターンである。
 * 
 * ガイドライン 38 ：Singleton パターンに変更しやすさとテスト可用性を設計する
 * 
 * Singleton パターンが嫌われる主な理由は、多くの場合で依存関係を新たに発生させ、テスト可用性を
 * 阻害することにあります。そこで、Singleton パターンを邪悪なものだと断罪する前に、プログラムのグローバルアスペクトを
 * 表現する必要があり、その適切な表現方法を議論しており、変更しやすさとテスト可用性も失わないように設計するという、滅多に
 * ない場面を考えます：）
 * 
 * Singleton パターンはグローバルな状態を表現する
 * 
 * Singleton パターンはグローバル変数を使用する開発者が用いる仕組みである。一般に
 * グローバル変数は良くないとされているが、その理由は複数ある。その 1 つにはその
 * 不透明性にある。
 * 
 * Singleton パターンは変更しやすさとテスト可用性を阻害する
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_10_singleton.cpp -o ../bin/main
*/
#include <iostream>
#include <cassert>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * 次に挙げる Database クラスが Meyers の Singleton を実装したものです。
*/

class Database final {
private:
    Database() {}
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    Database(Database&&) = delete;
    Database& operator=(Database&&) = delete;
    // ... 他にもデータベースでメンバ変数を使うものがあるだろう
public:
    static Database& instance() {
        static Database db;     // 唯一のインスタンス
        return db;
    }

    bool write(/* some arguments */) {
        puts("------ write");
        return true;
    }
    bool read(/* some arguments */) {
        puts("------ read");
        return true;
    }
    // ... その他データベース特有の機能
};

int test_Database() {
    puts("--- test_Database");
    try {
        Database& db1 = Database::instance();
        Database& db2 = Database::instance();
        db1.write();
        db2.read();
        assert(&db1 == &db2);
        ptr_lambda_debug<const char*,Database*>("db1 addr is ",&db1);
        ptr_lambda_debug<const char*,Database*>("db2 addr is ",&db2);
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

int main(void) {
    puts("START Singleton パターン ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const double&>("pi is ", pi);
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_Database());
    }
    puts("=== Singleton パターン END");
    return 0;
}