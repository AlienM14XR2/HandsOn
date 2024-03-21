/**
 * Lost Chapter O/R Mapping
 * 
 * Makefile が必要だな。
 * 
 * 私は、C++ のビルドに関しては素人以下だ、テンプレートを利用したクラスの分割（ヘッダとソース）に
 * 関して、注意喚起している書籍を持っていないし（見落としている可能性は否めない）、Google に尋ねな
 * ければいけないとは。次のことは、今日費やした時間のためにも肝に銘じる。
 * 『テンプレートの実装はヘッダに書かなければならない』これは少し乱暴な手段であり、別な方法もある。
 * しかし、現状はこれでよしとする、あぁ、安らかに眠れるし、次こそは Makefile に専念できるだろう。
 * 
 * 以前もどこかで記述したが、改めて自分の開発環境を再掲しておく。
 * ```
 * No LSB modules are available.
 * Distributor ID:	Ubuntu
 * Description:	Ubuntu 22.04.4 LTS
 * Release:	22.04
 * Codename:	jammy
 * ```
 * 
 * MySQL サーバのインストール
 * ```
 * sudo apt install mysql-server
 * ```
 * MySQL サーバの Version 確認
 * ```
 * $ sudo mysqladmin -p -u [your_mysql_user] version
 * ```
 * mysqladmin  Ver 8.0.36-0ubuntu0.22.04.1 for Linux on x86_64 ((Ubuntu))
 * Copyright (c) 2000, 2024, Oracle and/or its affiliates.
 * 
 * Oracle is a registered trademark of Oracle Corporation and/or its
 * affiliates. Other names may be trademarks of their respective
 * owners.
 * 
 * Server version		8.0.36-0ubuntu0.22.04.1
 * Protocol version	10
 * Connection		Localhost via UNIX socket
 * UNIX socket		/var/run/mysqld/mysqld.sock
 * Uptime:			1 hour 1 min 10 sec
 * 
 * コンパイル例を実行する前に次を実行して、MySQL を利用するにあたり、必要なヘッダファイルとライブラリを用意した。
 * ```
 * sudo apt-get install  libmysqlcppconn-dev
 * ```
 * 
 * e.g. compile A.
   g++ -O3 -DDEBUG -std=c++20 -I../inc/ -I/usr/local/include/ -I/usr/include/mysql-cppconn-8/ -L/usr/local/lib/ -L/usr/lib/x86_64-linux-gnu/ -pedantic-errors -Wall -Werror main.cpp -lmysqlcppconn -lmysqlcppconn8 -lpqxx -lpq ./repository/PersonRepository.cpp ./sql_generator.cpp ./model/PersonStrategy.cpp ./data/PersonData.cpp ./driver/MySQLDriver.cpp ./test/test_1.cpp ./connection/MySQLConnection.cpp -o ../bin/main
 * 
 * e.g. compile B. 
 * 分割した方が少しだけコンパイル時間が短縮できるかな、体感値で申し訳ないが。
 * 以下の行を毎回実行する必要はなく、必要に応じて適宜、上の行は実行すれば良い、main.cpp と比較した場合、アーキテクチャ上の
 * 上位と見ることができる。 main.cpp ほど修正が入らないため。どこかのタイミングで、Makefile にはしたい。
 * 
   g++ -O3 -DNDEBUG -std=c++20 -I../inc/ -pedantic-errors -Wall -Werror -c ./data/PersonData.cpp -o ../bin/PersonData.o
   g++ -O3 -DNDEBUG -std=c++20 -I../inc/ -pedantic-errors -Wall -Werror -c ./model/PersonStrategy.cpp -o ../bin/PersonStrategy.o
   g++ -O3 -DNDEBUG -std=c++20 -I../inc/ -pedantic-errors -Wall -Werror -c ./driver/MySQLDriver.cpp -o ../bin/MySQLDriver.o
   g++ -O3 -DNDEBUG -std=c++20 -I../inc/ -pedantic-errors -Wall -Werror -c ./connection/MySQLConnection.cpp -o ../bin/MySQLConnection.o
   g++ -O3 -DNDEBUG -std=c++20 -I../inc/ -pedantic-errors -Wall -Werror -c ./sql_generator.cpp -o ../bin/sql_generator.o
   g++ -O3 -DNDEBUG -std=c++20 -I../inc/ -pedantic-errors -Wall -Werror -c ./repository/PersonRepository.cpp -o ../bin/PersonRepository.o
NG 今この中間ファイルでは正しく動作しない、このコンパイルはいらない、main に含めた記述が必要、下記参照。   g++ -O3 -DNDEBUG -std=c++20 -I../inc/ -pedantic-errors -Wall -Werror -c ./test/test_1.cpp -o ../bin/test_1.o
   g++ -O3 -DDEBUG -std=c++20 -I../inc/ -I/usr/include/mysql-cppconn-8/ -L/usr/lib/x86_64-linux-gnu/ -pedantic-errors -Wall -Werror  ./test/test_1.cpp main.cpp -lmysqlcppconn -lmysqlcppconn8 -lpqxx -lpq ../bin/PersonRepository.o ../bin/sql_generator.o ../bin/PersonStrategy.o ../bin/PersonData.o ../bin/MySQLDriver.o ../bin/MySQLConnection.o -o ../bin/main
*/

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <vector>
#include <memory>
#include <optional>
#include <set>
#include <chrono>
#include <array>
#include <nlohmann/json.hpp>
#include "Debug.hpp"
#include "DataField.hpp"
#include "RdbDataStrategy.hpp"
#include "PersonStrategy.hpp"
#include "PersonData.hpp"
#include "MySQLDriver.hpp"
#include "ConnectionPool.hpp"
#include "test_1.hpp"
#include "MySQLConnection.hpp"
#include "Repository.hpp"
#include "RdbTransaction.hpp"
#include "RdbProcStrategy.hpp"
#include "MySQLCreateStrategy.hpp"
#include "MySQLReadStrategy.hpp"
#include "MySQLUpdateStrategy.hpp"
#include "MySQLDeleteStrategy.hpp"
#include "MySQLTx.hpp"
#include "PersonRepository.hpp"
#include "MySQLXTx.hpp"
#include "MySQLXCreateStrategy.hpp"
#include "AppProp.hpp"
#include "mysql/jdbc.h"
#include "mysqlx/xdevapi.h"
#include <pqxx/pqxx>

int test_debug_and_error() {
    puts("=== test_debug_and_error");
    try {
        // ptr_print_now();
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
 * 設計及び実装はここから。
*/







ConnectionPool<sql::Connection> app_cp;
AppProp appProp;

bool read_app_prop() {
    try {
        std::string line;
        std::string s;
        // TODO これ環境変数にしたい。
        std::ifstream appPropJson("/home/jack/dev/c++/HandsOn/ORM-Cheshire/src/appProp.json");
        if(appPropJson.is_open()) {
            while(std::getline(appPropJson, line)) {
                std::cout << line << '\n';
                s.append(line);
            }
            appPropJson.close();
            nlohmann::json j = nlohmann::json::parse(s);;
            std::cout << j << std::endl;
            for(auto& el: j) {
                auto mysql = el.at("/mysql"_json_pointer);
                std::cout << mysql << '\n';
                appProp.my.uri = mysql.at("uri");
                appProp.my.port = mysql.at("port");
                appProp.my.user = mysql.at("user");
                appProp.my.password = mysql.at("password");
                std::cout << "mysql is "<< appProp.my.uri << ": " << appProp.my.port << ": " << appProp.my.user << ": " << appProp.my.password << '\n';

                auto mysqlx = el.at("/mysqlx"_json_pointer);
                std::cout << mysqlx << '\n';
                appProp.myx.uri = mysqlx.at("uri");
                appProp.myx.port = mysqlx.at("port");
                appProp.myx.user = mysqlx.at("user");
                appProp.myx.password = mysqlx.at("password");
                std::cout << "mysqlx is "<< appProp.myx.uri << ": " << appProp.myx.port << ": " << appProp.myx.user << ": " << appProp.myx.password << '\n';

                auto pqxx = el.at("/pqxx"_json_pointer);
                std::cout << pqxx << '\n';
                appProp.pqx.uri = pqxx.at("uri");
                appProp.pqx.port = pqxx.at("port");
                appProp.pqx.dbname = pqxx.at("dbname");
                appProp.pqx.user = pqxx.at("user");
                appProp.pqx.password = pqxx.at("password");
                std::cout << "pqxx is "<< appProp.pqx.uri << ": " << appProp.pqx.port << ": " << appProp.pqx.dbname << ": " << appProp.pqx.user << ": " << appProp.pqx.password << '\n';
                std::cout << "pqxx :" << appProp.pqx.toString() << std::endl;
            }
        } else {
            std::cout << "Unable to open file." << std::endl;
            return false;
        }
        return true;
    } catch(std::exception& e) {
        throw std::runtime_error(e.what());
    }
}

int test_read_appProp() {
    puts("=== test_read_appProp");
    try {
        bool ret = read_app_prop();
        assert(ret == true);
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}



/**
 * トランザクションについて考えてみる。
 * 
 * con->setAutoCommit(false);
 * try {
 *   // ... Prepared Statement の発行等の具体的な SQL 文の構築を行う。
 *   std::unique_ptr<sql::PreparedStatement> prep_stmt(con->prepareStatement(sql));     // MySQL を例にすれば、ステートメントはコネクションと SQL に依存している。
 *   con->commit();
 * } catch(...) {
 *   con->rollback();
 * }
 * 
 * リポジトリとトランザクションは別、これを如何に綺麗に設計できるのか。
 * - CRUD 単位でトランザクションのインタフェースを用意する。
 * - トランザクション内で扱うオブジェクトは、リポジトリのテンプレートにする（できるかな？）。
 * - おそらく、コネクションとステートメントは抽象化しないとダメかな。
 * - これも Step 0 として、別ソースファイルで確認する必要がある。
 * 
 * @see inc/RdbTransaction.hpp
 * 
*/

/**
 * 再びこっちに戻ってきた。FastCGI を利用した REST-API の実装は CRUD の基本的なものを実装して
 * 一応の終了とした。ここで、再度考えてみたいこと、それは DB アクセスの高速化、それを組み込ん
 * だ C++ での設計だ。結論から言えば、"mysql/jdbc.h" を利用した場合、REST-API 経由では秒間 800
 * スループットは困難だと思ったからだ。"mysqlx/xdevapi.h" は DB アクセス（INSERT）で既に 1 ミリ
 * 秒を切る、jdbc.h を利用したものと比較すると、約 3 倍速い（シャアザク：）C/C++ を利用するから
 * にはやはり、実行速度こそが、正義だと思う。
 * 
 * まずはソースを整理するために、main.cpp 上のテスト関数を test_1.cpp に移行する
 * （動かなくなりそうだな：）。
 * 
 * 具体的に、mysqlx を利用した際の設計、実装の理想を考えてみる。良い機会だから、namespace を使っ
 * てみたい。
 * 
 * サンプルを実装した時に感じた疑問点をまず解決すべきかもしれない。
 * - Q．トランザクション（begin commit rollback）はどうなっているのか。
 * - A．mysqlx::Session に startTransaction()、commit()、rollback() があるね。
 * 
 * sql::Connection ではなく mysqlx::Session をプールするものが必要。
*/

ConnectionPool<mysqlx::Session> app_sp("mysqlx::Session.");     // アプリケーションのセッションプール

void mysqlx_session_pool(const std::string& server, const int& port, const std::string& user, const std::string& passwd, const int& sum) {
    puts("=== mysqlx_session_pool");
    for(int i=0; i<sum; i++) {
        puts("connected ... ");
        app_sp.push(new mysqlx::Session(server, port, user, passwd));
    }
}

int test_mysqlx_session_pool() {
    puts("=== test_mysqlx_session_pool");
    try {
        std::string server(appProp.myx.uri);
        int port = appProp.myx.port;
        std::string user(appProp.myx.user);
        std::string passwd(appProp.myx.password);
        mysqlx_session_pool(server, port, user, passwd, 3);
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_mysqlx_insert() {
    puts("=== test_mysqlx_insert");
    std::clock_t start_1 = clock();
    try {
        // mysqlx::Session sess("localhost", 33060, "derek", "derek1234");
        mysqlx::Session sess(appProp.myx.uri, appProp.myx.port, appProp.myx.user, appProp.myx.password);
        std::clock_t start_2 = clock();
        mysqlx::Schema db = sess.getSchema("cheshire");
        mysqlx::Table person = db.getTable("person");
        
        std::array<std::string, 3> cols{"name","email","age"};
        // mysqlx::Result res = person.insert("name", "email", "age")
        mysqlx::Result res = person.insert(cols)
                .values("Jabberwocky", "Jabberwocky@loki.org", nullptr)     // TODO 異なる型の値の保持、受け渡し方法、これを実装できないとダメだ。
                .values("Rabbit Foot", "rabbit@loki.org", 1)
                .values(";DELETE FROM person;", "ace@loki.org", nullptr)    // SQL Injection はできなかった
                .execute();
        // ptr_lambda_debug<const char*,const char*>("ret type is ", typeid(ret).name());
        std::cout <<  res.getAutoIncrementValue() << std::endl;     // 最初に Insert したレコードの pkey (AUTO INCREMENT) の値
        std::cout <<  res.getAffectedItemsCount() << std::endl;     // 上記で Insert されたレコード件数
        // auto ids = res.getGeneratedIds();        // Document のみかな？ Table ではダメ？
        // for(auto id: ids) {
        //     std::cout <<  id << std::endl;
        // }
        std::clock_t end = clock();
        std::cout << "passed " << (double)(end-start_1)/CLOCKS_PER_SEC << " sec." << std::endl;
        std::cout << "passed " << (double)(end-start_2)/CLOCKS_PER_SEC << " sec." << std::endl;

        // 次の書き分けが std::optional を利用している際のヒントになるかな。
        mysqlx::TableInsert tblIns = person.insert(cols);
        tblIns.values("Alpha", "alpha@loki.org", nullptr).execute();

        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_mysqlx_update() {
    puts("=== test_mysqlx_update");
    try {
        // std::optional<int> age = std::nullopt;
        // // やっぱり次のはエラーになるのか。
        // std::cout <<  age.value() << std::endl;
        
        mysqlx::Session sess(appProp.myx.uri, appProp.myx.port, appProp.myx.user, appProp.myx.password);
        mysqlx::Schema db = sess.getSchema("cheshire");
        mysqlx::Table person = db.getTable("person");
        person.update()
                .set("name", "FOO").set("email", "foo@loki.org").set("age", 3)
                .where("id = 8")
                .execute();
        person.update()
                .set("name", "FOO").set("email", "foo@loki.org").set("age", 3)
                .where("id = ;DELETE FROM person;")
                .execute();
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_mysqlx_select() {
    puts("=== test_mysqlx_select");
    try {
        mysqlx::Session sess(appProp.myx.uri, appProp.myx.port, appProp.myx.user, appProp.myx.password);
        mysqlx::Schema db = sess.getSchema("cheshire");
        mysqlx::Table person = db.getTable("person");

        mysqlx::RowResult row = person.select("name","email","age").where("id = 8").execute();
        for(auto d: row) {
            std::cout << d.get(0) << '\t' << d.get(1) << '\t' << d.get(2) << std::endl;
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_mysqlx_delete() {
    puts("=== test_mysqlx_delete");
    try {
        mysqlx::Session sess(appProp.myx.uri, appProp.myx.port, appProp.myx.user, appProp.myx.password);
        mysqlx::Schema db = sess.getSchema("cheshire");
        mysqlx::Table person = db.getTable("person");

        person.remove().where("id = 8").execute();
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * C++ でどれくらい自分が設計、コーディングができるのかという点に注力してきたので
 * 実際の mysql/jdbc.h や mysqlx/xdevapi.h の違いを深く考えずにきた。問題にした点
 * は実行速度であり、その内部の作りや、使い方ではなかった。ただ mysqlx は ビルダ
 * パターン？ で SQL を構築するので、もし、素の SQL 文がそのまま利用することができ
 * ないと、複雑なSQL の構築には手を焼くのではないのかという疑念がある。私は DBA で
 * はないので、SQL に関しては大して知見はないが、SQL の肝は SELECT 文にあると思う。
 * タスクの大半はその実装になることもある、そう考えると、更新系は xdevapi.h 検索系
 * は jdbc.h という使い方もあり得るのか、それとも xdevapi.h ですべて賄えるのか。
 * この点は、よくよく検証する必要があるだろう。まぁ本気で利用するなら。
*/


/**
 * 細かな疑問がある、Tx は スキーマやテーブルの取得前に startTransaction() できるのか？
 * これをハッキリさせないと、Tx の流用が可能なのか分からない。
 * 
 * mysql> use information_schema;
 * mysql> select table_schema, table_name, engine from tables where table_name = 'person';
 * 
 * | cheshire     | person     | InnoDB |
 * 
 * 勿論上記を確認した。
*/

int test_mysqlx_tx_insert() {
    puts("=== test_mysqlx_tx_insert");
    mysqlx::Session sess(appProp.myx.uri, appProp.myx.port, appProp.myx.user, appProp.myx.password);
    try {
        sess.startTransaction();
        mysqlx::Schema db = sess.getSchema("cheshire");
        mysqlx::Table person = db.getTable("person");
        
        mysqlx::Result res = person.insert("name", "email", "age")
                .values("MajorX", "majorx@loki.org", 24)
                .execute();
throw std::runtime_error("It's rollback test.");
/**
 * 公式サンプル通りではなくとも Tx は有効のようだ。
 * 同一セッションであれば問題ないと思われる、コネクション同様これは少し注意しよう。
 * https://dev.mysql.com/doc/x-devapi-userguide/en/transaction-handling.html
*/
        sess.commit();
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        sess.rollback();
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_ormx_PersonRepository_insert() {
    puts("=== test_ormx_PersonRepository_insert");
    // TODO セッションはプールしたものを利用すること
    // mysqlx::Session session("localhost", 33060, "derek", "derek1234");
    mysqlx::Session* session = app_sp.pop();
    try {
        std::string expect_name("Major");
        std::string expect_email("major@loki.org");
        int         expect_age = 24;
        ormx::PersonData major(expect_name, expect_email, expect_age);
        ormx::PersonRepository repo(session);
        std::optional<ormx::PersonData> result = repo.insert(major);
        assert( result.has_value() == 1 );
        if(result.has_value()) {
            // PersonRepository の内部で findOne は実行していない。
            assert(result.value().getName()        == expect_name);
            assert(result.value().getEmail()       == expect_email);
            assert(result.value().getAge().value() == expect_age);
        }

        std::string expect_name_2("Batou");
        std::string expect_email_2("batou@loki.org");
        ormx::PersonData batou(expect_name_2, expect_email_2);
        std::optional<ormx::PersonData> result_2 = repo.insert(batou);
        assert( result_2.has_value() == 1 );
        if(result.has_value()) {
            // PersonRepository の内部で findOne は実行していない。
            assert(result_2.value().getName()        == expect_name_2);
            assert(result_2.value().getEmail()       == expect_email_2);
            assert(result_2.value().getAge().has_value() == 0);
        }
        app_sp.push(session);
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_MySQLXCreateStrategy() {
    puts("=== test_MySQLXCreateStrategy");
    // TODO セッションはプールしたものを利用すること
    // mysqlx::Session session("localhost", 33060, "derek", "derek1234");
    mysqlx::Session* session = app_sp.pop();
    try {
        std::string expect_name("Togusa");
        std::string expect_email("togusa@loki.org");
        int         expect_age = 36;
        ormx::PersonData togusa(expect_name, expect_email, expect_age);
        std::unique_ptr<Repository<ormx::PersonData, std::size_t>>        repo = std::make_unique<ormx::PersonRepository>(session);
        std::unique_ptr<RdbProcStrategy<ormx::PersonData>>        procStrategy = std::make_unique<ormx::MySQLXCreateStrategy<ormx::PersonData, std::size_t>>(repo.get(), togusa);
        ormx::MySQLXTx tx(session, procStrategy.get());
        std::optional<ormx::PersonData> result = tx.executeTx();
        assert( result.has_value() == 1 );
        if(result.has_value()) {
            assert(result.value().getName()        == expect_name);
            assert(result.value().getEmail()       == expect_email);
            assert(result.value().getAge().value() == expect_age);
        }
        app_sp.push(session);
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
    /**
     * これが出来ればまずは OK、CRUD の Create のみを REST-API に導入する手順に入れる。
     * namespace ormx を宣言と定義（ヘッダとソース）に分離する作業に入る。
    */
}

/**
 * libpqxx や X DevAPI はコネクションプールの仕組みは利用できても、先に設計した、トランザクションの仕組みは利用できないと感じた。
 * （使ってもいいが、いらないという意味）Too Much なものになってしまうから。
 * リポジトリは同様に定義できてもそれを利用するのは Tx ではなく、サービスになると思う。今回の Lost Chapter はこれで終了とする。
 * 
 * うん、前言撤回だ、O\R Mapping としてどこまでできるのか、PostgreSQL でも極力 MySQL と同じインタフェースで行けるのか、自分の設計
 * が間違っていないのか、それを確かめなければ、C++ の学習としては不完全だと感じた。MySQL は jdbc.h、xdevapi.h 双方に対応できたの
 * だから、できるところまでやってみる。
 * 余談だが、MySQL は xdevapi.h（mysqlx） を利用し Nginx + FastCGI の構成で、person Table に INSERT する API で 1 ミリ秒を切ること
 * が、この旧式マシンでも確認できた。PostgreSQL では REST API は用意するつもりはない。
 * 
 * では、pqxx のトランザクションの調査からはじめる。
 * pqxx::work に commit() はあるが、begin() と rollback() が見当たらない。
 * https://pqxx.org/development/libpqxx/
 * 公式のリファレスンスでも begin rollback が見当たらない：）
 * 実験するのが手っ取り早いかな。
 * 
 * 段階的に確認する。
 * - CREATE TABLE を コーディングする（テストを楽にするため）
 * - SQL インジェクションの可否
 * - Tx の確認
*/

int test_pqxx_create_table() {
    try {
        puts("=== test_pqxx_create_table");
        // pqxx::connection con{"hostaddr=127.0.0.1 port=5432 dbname=jabberwocky user=derek password=derek1234"};
        pqxx::connection con{appProp.pqx.toString()};
        pqxx::work tx{con};
        const char* createAnimalTableSql = R"(
            CREATE TABLE animal ( 
                id BIGINT NOT NULL PRIMARY KEY
                , name VARCHAR(128) NOT NULL UNIQUE 
            )
        )";
        const char* createCompanyTableSql = R"(
            CREATE TABLE company ( 
                id BIGINT NOT NULL PRIMARY KEY
                , name VARCHAR(128) NOT NULL
                , address VARCHAR(256) NOT NULL 
            )
        )";
        const char* createSequenceSql       = R"(CREATE SEQUENCE table_id_seq)";
        const char* dropAnimalTableSql      = R"(DROP TABLE IF EXISTS animal)";
        const char* dropCompanyTableSql     = R"(DROP TABLE IF EXISTS company)";
        const char* dropSequenceSql         = R"(DROP SEQUENCE IF EXISTS table_id_seq)";
        const char* insertSql               = R"(INSERT INTO animal (id, name) values (0, 'Lion'))";
        std::string sql = dropSequenceSql;
        tx.query(sql);
        sql             = dropAnimalTableSql;
        tx.query(sql);
        sql             = dropCompanyTableSql;
        tx.query(sql);
        sql             = createAnimalTableSql;
        tx.query(sql);
        sql             = createCompanyTableSql;
        tx.query(sql);
        sql             = createSequenceSql;
        tx.query(sql);
        sql             = insertSql;
        tx.exec0(sql);
        tx.commit();
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_pqxx_sql_injection() {
    puts("=== test_pqxx_sql_injection");
    try {
        pqxx::connection con{appProp.pqx.toString()};
        pqxx::work tx{con};
        long nextId = tx.query_value<int>(
            "SELECT nextval('table_id_seq')"
        );
        // コンソールで次を実行すると、DROP も成功する。
        // INSERT INTO animal (id, name) values (20, '');DROP Table company;');
        std::string expect_name("');DROP Table company;");
        std::string sql("INSERT INTO animal (id, name) values (");
        sql.append(std::to_string(nextId)).append(", '").append(expect_name).append("')");
        ptr_lambda_debug<const char*, const std::string&>("sql: ", sql);
        tx.exec0(sql);
        tx.commit();
        /**
         * これで company が消えなければ、SQL Injection の対策は pqxx で対処している。
         * といえるのかな。
        */
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_pqxx_rollback() {
    puts("=== test_pqxx_rollback");
    try {
        pqxx::connection con{appProp.pqx.toString()};
        pqxx::work tx{con};
        long nextId = tx.query_value<int>(
            "SELECT nextval('table_id_seq')"
        );
        std::string expect_name("Little DOG");
        std::string sql("INSERT INTO animal (id, name) values (");
        sql.append(std::to_string(nextId)).append(", '").append(expect_name).append("')");
        ptr_lambda_debug<const char*, const std::string&>("sql: ", sql);
        tx.exec0(sql);
        throw std::runtime_error("It's pqxx rollback test.");   // コミット前にランタイム・エラーを発生させる。
        tx.commit();
        /**
         * commit() を明示的に行わなければ、テーブルには反映されない。
        */
        return EXIT_SUCCESS;
    } catch(pqxx::sql_error& e) {
        std::cerr << "SQL error: " << e.what() << std::endl;
        std::cerr << "Query was: " << e.query() << std::endl;
        return EXIT_FAILURE;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}






/**
 * 私が当初考えた仕組みに合わせて pqxx を利用したデータアクセスの実装を進めてみる。
 * 最初はコメントコーディングで必要な概念を列挙する。
*/

// CREATE TABLE company ( id BIGINT NOT NULL PRIMARY KEY, name VARCHAR(128) NOT NULL, address VARCHAR(256) NOT NULL );
// Table（campany）

class CompanyData {
public:
    CompanyData(const long& _id, const std::string& _name, const std::string& _address) : id(_id), name(_name), address(_address)
    {}
    // ...
    long getId()      const { return id; }
    std::string getName()    const { return name; }
    std::string getAddress() const { return address; }
private:
    long id;
    std::string name;
    std::string address;
};


// Repository の派生クラス

class CompanyRepository final : public Repository<CompanyData, long> {
public:
    CompanyRepository(pqxx::work* _tx): tx(_tx)
    {}
    virtual std::optional<CompanyData> insert(const CompanyData& data)   const override
    {
        // 実装
        puts("------ CompanyRepository::insert()");
        // 次のクエリは 今後、DRY の原則に引っかかると思われる。
        long nextId = tx->query_value<long>(
            "SELECT nextval('table_id_seq')"
        );
        std::string sql("INSERT INTO company (id, name, address) values (");
        sql.append(std::to_string(nextId)).append(", '").append(data.getName()).append("', '").append(data.getAddress()).append("')");
        ptr_lambda_debug<const char*, const std::string&>("sql: ", sql);
        tx->exec0(sql);
        CompanyData result(nextId, data.getName(), data.getAddress());
        return result;
    }
    virtual std::optional<CompanyData> update(const CompanyData&)   const override
    {
        // TODO 実装
        return std::nullopt;
    }
    virtual void remove(const long&)   const override
    {
        // TODO 実装
    }
    virtual std::optional<CompanyData> findOne(const long&)  const override
    {
        // TODO 実装
        return std::nullopt;
    }
private:
    pqxx::work* tx;
};


// RdbTransaction の派生クラス

template <class DATA>
class PGSQLTx final : public RdbTransaction<DATA> {
public:
    PGSQLTx(pqxx::work* _tx, const RdbProcStrategy<DATA>* _strategy): tx(_tx), strategy(_strategy)
    {}
    virtual void begin()    const override
    {
        puts("------ PGSQLTx::begin()");
        // none.
    }
    virtual void commit()   const override
    {
        puts("------ PGSQLTx::commit()");
        tx->commit();
    }
    virtual void rollback() const override
    {
        puts("------ PGSQLTx::rollback()");
        // none. pqxx::work は例外が発生して commit() が呼ばれなければ、勝手に rollback するという認識です（間違ってるかも：）。
    }
    virtual std::optional<DATA> proc() const override
    {
        puts("------ PGSQLTx::proc()");
        return strategy->proc();
    }
private:
    pqxx::work* tx;
    const RdbProcStrategy<DATA>* strategy;
};

// RdbProcStrategy の派生クラス（Create）

template<class DATA, class PKEY>
class PGSQLCreateStrategy final : public RdbProcStrategy<DATA> {
public:
    PGSQLCreateStrategy(const Repository<DATA, PKEY>* _repo, const DATA& _data): repo(_repo), data(_data)
    {}
    virtual std::optional<DATA> proc() const override
    {
        puts("------ PGSQLCreateStrategy::proc()");
        try {
            return repo->insert(data);
        } catch(std::exception& e) {
            throw std::runtime_error(e.what());
        }
    }
private:
    const Repository<DATA, PKEY>* repo;
    DATA data;

};

int test_CompanyRepository_insert() {
    puts("=== test_CompanyRepository_insert");
    try {
        pqxx::connection con{appProp.pqx.toString()};
        pqxx::work tx{con};

        CompanyData data(0u, "ACB 総研", "東京都");
        CompanyRepository repo(&tx);
        std::optional<CompanyData> ret = repo.insert(data);
        tx.commit();
        assert( ret.has_value() == true );
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_PGSQLTx_Create() {
    puts("=== test_PGSQLTx_Create");
    try {
        // pqxx::connection con{"hostaddr=127.0.0.1 port=5432 dbname=jabberwocky user=derek password=derek1234"};
        pqxx::connection con{appProp.pqx.toString()};
        std::clock_t start = clock();
        pqxx::work tx{con};

        std::string expectName    = "LOKI co.,ltd";
        std::string expectAddress = "Tokyo, Japan.";
        CompanyData data(0u, expectName, expectAddress);
        std::unique_ptr<Repository<CompanyData, long>> repo = std::make_unique<CompanyRepository>(&tx);
        std::unique_ptr<RdbProcStrategy<CompanyData>> strategy = std::make_unique<PGSQLCreateStrategy<CompanyData, long>>(repo.get(), data);
        PGSQLTx<CompanyData> pgtx(&tx, strategy.get());
        std::optional<CompanyData> result = pgtx.executeTx();
        assert(result.has_value() == true);
        if(result.has_value()) {
            assert(result.value().getName()    == expectName);
            assert(result.value().getAddress() == expectAddress);
        }
        std::clock_t end = clock();
        std::cout << "passed " << (double)(end-start)/CLOCKS_PER_SEC << " sec." << std::endl;
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * 時間を置いてから見直しても、いい設計だと思う。
 * テーブルに関するクラスは絶対に必要、リポジトリも然り。
 * その中で、CRUD の共通項を取り出し、それをトランザクションに当てはめる設計。
 * RdbProcStrategy は CRUD の数分、つまり 4 つ定義すれば、すべてのリポジトリ
 * に対応でき、データの型にも左右されない。制約は、RDBMS 単位で必要という点。
 * 自画自賛か：）できればね、Spring Data のように Proxy ですべて動的にリポジ
 * トリ及び 最低限の CRUD を自動生成させたいよね。インタフェースの定義だけで
 * 実態は Proxy のクラスという仕組み、今度はこれを考えてみようかな。これは、
 * Proxy パターンなのだろうか。C++ でのコードの自動生成ということになるんだ
 * よな、きっと、違うのかな。あぁ、話が逸れた。
*/




/**
 * 全くの別件だが、今回いろいろ C++ のビルド周りを調べた際に次のような情報を見つけた。
 * C++ と C のコードを混在させてコンパイル、ビルドする際は、以下のような記述が必要との
 * こと。これは C++ と C では関数のシンボル定義が、異なることを防ぐ効果があるという内容
 * だったと記憶しているが、間違っている可能性もある。
*/

#ifdef __cplusplus
extern "C" {
#endif

//ここに関数の宣言

#ifdef __cplusplus
}
#endif


int main(void) {
    puts("START Lost Chapter O/R Mapping ===");
    if(0.01) {
        auto ret = 0;
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_debug_and_error());
        assert(ret == 1);
    }
    if(1.00) {
        auto ret = 0;
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_DataField());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_DataField_2());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_DataField_3());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_PersonData());
        assert(ret == 0);
    }
    if(1.01) {
        auto ret = 0;
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_makeInsertSql());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_makeUpdateSql());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_makeDeleteSql());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_makeFindOneSql());
        assert(ret == 0);
    }
    if(1.02) {
        auto ret = 0;
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_read_appProp());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_makeCreateTableSql());
        assert(ret == 0);
    }
    if(1.03) {
        auto ret = 0;
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_mysql_connect());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_insert_person());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_MySQLDriver());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_ConnectionPool());
        assert(ret == 1);   // テスト内で明示的に exception を投げている
    }
    if(1.05) {
        auto ret = 0;
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_PersonRepository_findOne());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_PersonRepository_update());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_PersonRepository_insert());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_PersonRepository_insert_no_age());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_PersonRepository_remove());
        assert(ret == 0);
    }
    if(1.06) {
        if(1.061) {
            auto ret = 0;
            ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_mysql_connection_pool_A());
            assert(ret == 0);
            ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_mysql_connection_pool_B());
            assert(ret == 0);
        }
        auto ret = 0;
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_MySQLTx());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_MySQLTx_rollback());
        assert(ret == 1);
        std::unique_ptr<std::size_t> insId = std::make_unique<std::size_t>(0ul);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_MySQLTx_Create(insId.get()));
        ptr_lambda_debug<const char*, const std::size_t&>("indId is ", *(insId.get()));
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_MySQLTx_Read(insId.get()));
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_MySQLTx_Update(insId.get()));
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_MySQLTx_Delete(insId.get()));
        assert(ret == 0);
    }
    if(2.00) {      // 2.00
        auto ret = 0;
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_mysqlx_connect());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_mysqlx_session_pool());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_mysqlx_insert());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_mysqlx_tx_insert());
        assert(ret == 1);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_ormx_PersonRepository_insert());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_MySQLXCreateStrategy());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_mysqlx_update());
        assert(ret == 1);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_mysqlx_select());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_mysqlx_delete());
        assert(ret == 0);
    }
    if(3.00){   // 3.00
        auto ret = 0;
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_pqxx_create_table());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_pqxx_connect());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_pqxx_insert());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_pqxx_resultset());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_pqxx_sql_injection());
        assert(ret == 1);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_pqxx_rollback());
        assert(ret == 1);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_CompanyRepository_insert());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_PGSQLTx_Create());
        assert(ret == 0);
    }
    puts("===   Lost Chapter O/R Mapping END");
    return 0;
}
