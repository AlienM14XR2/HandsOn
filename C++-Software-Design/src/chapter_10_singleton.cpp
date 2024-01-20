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
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_10_singleton.cpp -o ../bin/main
*/
#include <iostream>

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
        return true;
    }
    bool read(/* some arguments */) {
        return true;
    }
    // ... その他データベース特有の機能
};

int test_Database() {
    puts("--- test_Database");
    try {
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