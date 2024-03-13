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
 * 以下の 3 行を毎回実行する必要はなく、必要に応じて適宜、上 2 行は実行すれば良い、main.cpp と比較した場合、アーキテクチャ上の
 * 上位と見ることができる。 main.cpp ほど修正が入らないため。どこかのタイミングで、Makefile にはしたい。
 * 
   g++ -O3 -DNDEBUG -std=c++20 -I../inc/ -pedantic-errors -Wall -Werror -c ./data/PersonData.cpp -o ../bin/PersonData.o
   g++ -O3 -DNDEBUG -std=c++20 -I../inc/ -pedantic-errors -Wall -Werror -c ./model/PersonStrategy.cpp -o ../bin/PersonStrategy.o
   g++ -O3 -DNDEBUG -std=c++20 -I../inc/ -pedantic-errors -Wall -Werror -c ./driver/MySQLDriver.cpp -o ../bin/MySQLDriver.o
   g++ -O3 -DNDEBUG -std=c++20 -I../inc/ -pedantic-errors -Wall -Werror -c ./test/test_1.cpp -o ../bin/test_1.o
   g++ -O3 -DNDEBUG -std=c++20 -I../inc/ -pedantic-errors -Wall -Werror -c ./connection/MySQLConnection.cpp -o ../bin/MySQLConnection.o
   g++ -O3 -DNDEBUG -std=c++20 -I../inc/ -pedantic-errors -Wall -Werror -c ./sql_generator.cpp -o ../bin/sql_generator.o
   g++ -O3 -DNDEBUG -std=c++20 -I../inc/ -pedantic-errors -Wall -Werror -c ./repository/PersonRepository.cpp -o ../bin/PersonRepository.o
   g++ -O3 -DDEBUG -std=c++20 -I../inc/ -I/usr/include/mysql-cppconn-8/ -L/usr/lib/x86_64-linux-gnu/ -pedantic-errors -Wall -Werror main.cpp -lmysqlcppconn -lmysqlcppconn8 -lpqxx -lpq ../bin/PersonRepository.o ../bin/sql_generator.o ../bin/PersonStrategy.o ../bin/PersonData.o ../bin/MySQLDriver.o ../bin/test_1.o ../bin/MySQLConnection.o -o  ../bin/main
*/

#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include <memory>
#include <optional>
#include <set>
#include <chrono>
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




int test_mysql_connect() {              // これが test_1.cpp に移設できない orz
    puts("=== test_mysql_connect");
    sql::Driver* driver = nullptr;
    sql::Connection*          con    = nullptr;
    sql::Statement *          stmt   = nullptr;
    try {
        driver = get_driver_instance();
        con = driver->connect("tcp://127.0.0.1:3306", "derek", "derek1234");
        // con = driver->connect("tcp://172.22.1.64:3306", "derek", "derek1234");
        if(con->isValid()) {
            puts("connected ... ");
        }
        stmt = con->createStatement();
        stmt->execute("USE cheshire");
        stmt->execute("DROP TABLE IF EXISTS person");

        std::unique_ptr<RdbDataStrategy<PersonData>> strategy = std::make_unique<PersonStrategy>(PersonStrategy());
        DataField<std::size_t> id("id", 0, "BIGINT", "AUTO_INCREMENT PRIMARY KEY");  // MySQL でこの構文で問題がないか要検証。
        DataField<std::string> name("name", "Derek", "VARCHAR(128)", "NOT NULL");
        DataField<std::string> email("email", "derek@loki.org", "VARCHAR(256)", "NOT NULL UNIQUE");
        DataField<int> age("age", 21, "INT", "");
        PersonData derek(strategy.get(),id,name,email,age);
        auto sql = makeCreateTableSql(derek.getTableName(),derek.getTableInfo());
        ptr_lambda_debug<const char*, const decltype(sql)&>("sql is ", sql);
        stmt->execute(sql);

        delete stmt;
        delete con;
        // delete driver;           // これは蛇足らしい、これがあるとコアダンプになる。
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        if(stmt) {
            delete stmt;
        }
        if(con) {
            delete con;
        }
        // if(driver) {
        //     delete driver;
        // }
        return EXIT_FAILURE;
    }
}



ConnectionPool<sql::Connection> app_cp;


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
*/














/**
 * MySQL X DevAPI
 * 以前利用した mysqlx を再度検証してみる。
 * 
 * /usr/include/mysql-cppconn-8/mysqlx/xdevapi.h
 * 
 * 次のテスト関数は以前確認した事の移植、mysqlx::Session の作り方以外は同じもの。
*/

int test_mysqlx_connect() {
    puts("=== test_mysqlx_connect");
    std::clock_t start = clock();
    try {
        mysqlx::Session sess("localhost", 33060, "derek", "derek1234");
        // mysqlx::Session sess("172.22.1.64", 33060, "derek", "derek1234");
        mysqlx::Schema db = sess.getSchema("cheshire");
        std::cout << "your schema is " << db.getName() << std::endl;
        mysqlx::Table person = db.getTable("person");
        std::cout << "person count is " << person.count() << std::endl;
        mysqlx::TableSelect selectOpe = person.select("email").where("id=2");
        mysqlx::Row row = selectOpe.execute().fetchOne();
        std::cout << "row is null ? " << row.isNull() << std::endl;
        mysqlx::Value val = row.get(0);
        std::cout << val << std::endl;
        sess.close();
        std::clock_t end = clock();
        std::cout << "passed " << (double)(end-start)/CLOCKS_PER_SEC << " sec." << std::endl;
        /**
         * X DevAPI 圧倒的に速いんだよな。
        */
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)>(e);
        return EXIT_FAILURE;
    }
}


int test_mysqlx_insert() {
    puts("=== test_mysqlx_insert");
    std::clock_t start_1 = clock();
    try {
        mysqlx::Session sess("localhost", 33060, "derek", "derek1234");
        std::clock_t start_2 = clock();
        mysqlx::Schema db = sess.getSchema("cheshire");
        mysqlx::Table person = db.getTable("person");
        mysqlx::Result res = person.insert("name", "email", "age")
                .values("Jabberwocky", "Jabberwocky@loki.org", nullptr)
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
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * libpqxx PostgreSQL C++ Connection
 * 
 * 接続確認を行う。
*/

int test_pqxx_connect() {
    puts("=== test_pqxx_connect");
    try {
        // pqxx::connection con{"postgresql://derek@localhost/jabberwocky"};
        pqxx::connection con{"hostaddr=127.0.0.1 port=5432 dbname=jabberwocky user=derek password=derek1234"};
        pqxx::work tx{con};
        std::string name = tx.query_value<std::string>(
            "SELECT name "
            "FROM animal "
            "WHERE id = 0"
        );
        std::cout << "name is " << name << '\n';

        int lastId = tx.query_value<int>(
            "SELECT last_value from table_id_seq"
        );
        std::cout << "lastId is " << lastId << '\n';
        tx.commit();
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * PostgreSQL における pkey Auto-Increment は次のようにすればよいと考えていた（SERIAL || BIGSERIAL）。
 * 
 * e.g. 
CREATE TABLE animal (
    id SERIAL NOT NULL PRIMARY KEY
    , name VARCHAR(128) NOT NULL
);
 * 
 * Repository で INSERT 後にレコードを返却する場合は、
 * SELECT last_value from [your_sequence_name];
 * で取得できるとも。つまり、MySQL と同じ処理順番でよいと。（MySQL は SELECT LAST_INSERT_ID()）
 * 
 * ただ、この両者は明確にその動作が異なると考えた。MySQL の LAST_INSERT_ID() はテーブルに一行 pkey 指定なしの
 * INSERT 時にインクリメントされる値と同期する、つまり、テーブルの行ロック？と連動されるので、テーブルのINSERT 
 * トランザクションが有効に働く（はず）。AUTO-INC ロックというらしい（@see https://qiita.com/ham0215/items/99679d499869365446ec）
 * 
 * しかし、Postgres では SERIAL や BIGSERIAL は Sequence に過ぎない。
 * トランザクション内の INSERT 文と SELECT last_value from [your_sequence_name] の間に別のトランザクションで 
 * INSERT 文がコミットされている可能性があり、信用することができない。したがって、Postgres では先に Sequence 
 * から ID の値を払い受け、それを INSERT 文に含める。つまり、Sequence は利用するが、SERIAL、BIGSERIAL は利用
 * しない。
 * 
 
CREATE TABLE animal (
    id BIGINT NOT NULL PRIMARY KEY
    , name VARCHAR(128) NOT NULL
);
// データベース内のテーブルで共通、あるいは個別利用でもいい。
CREATE SEQUENCE table_id_seq;
INSERT INTO animal (id, name) values (0, 'Lion');

*/


int test_pqxx_insert() {
    puts("=== test_pqxx_insert");
    std::clock_t start_1 = clock();
    try {
        pqxx::connection con{"hostaddr=127.0.0.1 port=5432 dbname=jabberwocky user=derek password=derek1234"};
        std::clock_t start_2 = clock();
        pqxx::work tx{con};
        long nextId = tx.query_value<int>(
            "SELECT nextval('table_id_seq')"
        );
        std::string expect_name("Cerberus");
        std::string sql("INSERT INTO animal (id, name) values (");
        sql.append(std::to_string(nextId)).append(", '").append(expect_name).append("')");
        ptr_lambda_debug<const char*, const std::string&>("sql: ", sql);
        tx.exec0(sql);
        std::clock_t end_2 = clock();
        std::cout << "passed " << (double)(end_2-start_2)/CLOCKS_PER_SEC << " sec." << std::endl;

        std::string select_sql("SELECT id, name FROM animal WHERE id = ");
        select_sql.append(std::to_string(nextId));
        ptr_lambda_debug<const char*, const std::string&>("select_sql: ", select_sql);
        auto [ins_id, ins_name] = tx.query1<long, std::string>(select_sql);
        ptr_lambda_debug<const char*, const decltype(ins_id)&>("ins_id is ", ins_id);
        ptr_lambda_debug<const char*, const decltype(ins_name)&>("ins_name is ", ins_name);
        assert(ins_id   == nextId);
        assert(ins_name == expect_name);
        tx.commit();
        std::clock_t end_1 = clock();
        std::cout << "passed " << (double)(end_1-start_1)/CLOCKS_PER_SEC << " sec." << std::endl;
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_pqxx_resultset() {
    puts("=== test_pqxx_resultset");
    std::clock_t start = clock();
    try {
        pqxx::connection con{"hostaddr=127.0.0.1 port=5432 dbname=jabberwocky user=derek password=derek1234"};
        pqxx::work tx{con};
        pqxx::result res = tx.exec("SELECT * FROM animal");
        // std::cout << "Columns:\n";
        // for (pqxx::row_size_type col = 0; col < res.columns(); ++col) {
        //     std::cout << res.column_name(col) << '\n';
        // }

        // for(auto i = 0; i < res.size(); i++) {
        //     auto [id, name] = res.at(i).as<long, std::string>();
        //     ptr_lambda_debug<const char*, const decltype(id)&>("id is "    , id);
        //     ptr_lambda_debug<const char*, const decltype(name)&>("name is ", name);
        // }

        auto [id, name] = res.at(res.size() -1).as<long, std::string>();
        ptr_lambda_debug<const char*, const decltype(id)&>("id is "    , id);
        ptr_lambda_debug<const char*, const decltype(name)&>("name is ", name);
        tx.commit();
        std::clock_t end = clock();
        std::cout << "passed " << (double)(end-start)/CLOCKS_PER_SEC << " sec." << std::endl;
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * libpqxx や X DevAPI はコネクションプールの仕組みは利用できても、先に設計した、トランザクションの仕組みは利用できないと感じた。
 * （使ってもいいが、いらないという意味）Too Much なものになってしまうから。
 * リポジトリは同様に定義できてもそれを利用するのは Tx ではなく、サービスになると思う。今回の Lost Chapter はこれで終了とする。
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
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_mysqlx_insert());
        assert(ret == 0);
    }
    if(3.00){   // 3.00
        auto ret = 0;
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_pqxx_connect());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_pqxx_insert());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_pqxx_resultset());
        assert(ret == 0);
    }
    puts("===   Lost Chapter O/R Mapping END");
    return 0;
}
