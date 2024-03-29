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
 * ... ここでやっとサンプルの出番かな。
 * 
 * ガイドライン 38 の要約
 * - Singleton パターンはその短所もすべて含み、グローバルな状態を表現する。
 * - グローバルな状態変数は可能な限り避ける。
 * - Singleton パターンを用いるときは工夫を凝らし、限られた数のグローバルアスペクトにのみ用いる。
 * - Singleton パターンでの双方向のデータの流れはできるだけ避ける。
 * - Singleton パターンに付随する変更しやすさとテスト可用性の阻害を排除するには、Singleton パターンを用い依存関係を逆転する。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_10_singleton.cpp -o ../bin/main
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall chapter_10_singleton.cpp -o ../bin/main
*/
#include <iostream>
#include <cassert>
#include <array>
#include <cstddef>
#include <cstdlib>
#include <memory_resource>
#include <string>
#include <vector>

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

/**
 * Singleton パターンの依存関係逆転
 * C++17 pmr ( polymorphic memory resources ) の世界。
*/

int sample_pmr() {
    puts("--- sample_pmr");
    try {
        std::array<std::byte, 1000> raw;        // 注意：未初期化
        std::pmr::monotonic_buffer_resource buffer{raw.data(), raw.size(), std::pmr::null_memory_resource()};
        /**
         * std::pmr::null_memory_resource() を用いメモリを追加しようとします。この上流アロケータは何らメモリを返さず、常に失敗したら
         * std::bad_alloc() 例外をスローします。すなわち、buffer が使えるメモリは raw が持つ 1000 バイトに制限されます。
         * これは Decorator であり Singleton です。
         * std::pmr::null_memory_resource() はコールされる度に同じアロケータを指すポインタを返すため、1 だけ存在する std::pmr::null_memory_resource 
         * インスタンスを使用するシングルポイントかのように振る舞います。結果的に、返されたアロケータは Singleton パターンとして機能します。
         * メモリという一種のグローバルな状態を表現する妥当な選択です。
         * 
         * std::pmr::null_memory_resource() コールを std::pmr::get_default_resouce() に変更すると話が変化します。
         * std::pmr::get_default_resouce() も std::pmr::memory_resource を指すポインタを返しますが、これはシステムデフォルトアロケータです。
         * デフォルトでは std::pmr::new_delete_resource() 関数が返すアロケータに一致します。意外なことにこのデフォルトアロケータは std::pmr::set_default_resource() を
         * 用いるとカスタマイズできるのです。
        */
        std::pmr::vector<std::pmr::string> strings {&buffer};
        // ...
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

/**
 * Strategy パターンの適用
 * 依存オブジェクト注入ポイントを設ける。
 * サンプルとして Database クラスを修正する。
*/

class PersistenceInterface {
private:
    virtual bool do_read() const = 0;
    virtual bool do_write() = 0;
public:
    virtual ~PersistenceInterface() = default;

    bool read() const {
        return do_read();
    }
    bool write() {
        return do_write();
    }
};
PersistenceInterface* get_persistence_interface();          // 本来はヘッダファイルに記述する
void set_persistence_interface(PersistenceInterface*);      // 本来はヘッダファイルに記述する
extern PersistenceInterface* instance;                      // 本来はヘッダファイルに記述する

class DatabaseV2 final : public PersistenceInterface {
private:
    bool do_read() const override {
        puts("------ do_read");
        return true;
    }
    bool do_write() override {
        puts("------ do_write");
        return true;
    }
public:
    DatabaseV2() = default;
    DatabaseV2(const DatabaseV2&) = delete;
    DatabaseV2& operator=(const DatabaseV2&) = delete;
    DatabaseV2(DatabaseV2&&) = delete;
    DatabaseV2& operator=(DatabaseV2&&) = delete;
};

/**
 * PersistenceInterface.cpp
 * 本来であれば上記ソースファイルに記述するべき内容になる。
*/
namespace picpp {
    PersistenceInterface* instance = nullptr;

    PersistenceInterface* get_persistence_interface() {
        static bool init = []() {
            if( !instance ) {
                static DatabaseV2 db;
                instance = &db;
            }
            return true;    // 戻り値は重要ではないため false でも構わない。
        }();    // ラムダ式直後の () により、ラムダ式を実行する。
        return instance;
    }
    
    void set_persistence_interface(PersistenceInterface* _instance) {
        instance = _instance;
    }

}   // namespace picpp

int test_PersistenceInterface() {
    puts("--- test_PersistenceInterface");
    try {
        PersistenceInterface* persistence = picpp::get_persistence_interface();
        persistence->read();
        persistence->write();

        PersistenceInterface* persistence2 = picpp::get_persistence_interface();
        ptr_lambda_debug<const char*,PersistenceInterface*>("persistence addr is ",persistence);
        ptr_lambda_debug<const char*,PersistenceInterface*>("persistence2 addr is ",persistence2);
        assert(persistence == persistence2);

        // delete persistence;      // これは必要なかった、ラムダ式のスタティックローカル変数を参照しているだけだから（二重開放になる）。

        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

/**
 * PersistenceInterface の問題点、それはスレッドセーフではないことです。
 * どの時点でも、set_persistence_interface() によりグローバル変数の状態を変更できてしまいます。
 * したがって set_persistence_interface() で設定したものを get_persistence_interface() が返却する
 * 保証は一般にはありません。
 * この点に関しては対処は不要です。
 * set_persistence_interface() での設定を任意ではなく、プログラム開始直後か、あるいは 1 テスト項目開始時
 * とすべきです。
 * 
 * 依存注入という考えにすることで、アーキテクチャの境界が変化したことがまず重要と考える。
*/

int main(void) {
    puts("START Singleton パターン ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const double&>("pi is ", pi);
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_Database());
    }
    if(2.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",sample_pmr());
    }
    if(3.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_PersistenceInterface());
    }
    puts("=== Singleton パターン END");
    return 0;
}