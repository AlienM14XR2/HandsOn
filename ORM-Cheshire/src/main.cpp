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
 * 
 * 具体的に、mysqlx を利用した際の設計、実装の理想を考えてみる。良い機会だから、namespace を使っ
 * てみたい。
 * 
 * サンプルを実装した時に感じた疑問点をまず解決すべきかもしれない。
 * - Q．トランザクション（begin commit rollback）はどうなっているのか。
 * - A．mysqlx::Session に startTransaction()、commit()、rollback() があるね。
 * 
*/

namespace ormx {

/**
 * 今回最低限必要な概念をクラスにしてみる。
 * 
 * Tx と Repository 、RdbProcStrategy は前のものをそのまま利用できるはず。
*/

class PersonData {
};

class PersonRepository {
};

class SessionPool {
};

template <class DATA>
class MySQLXTx final : public RdbTransaction<DATA> {
public:
    MySQLXTx()
    {}
};

}   // namespace ormx

/**
 * 細かな疑問がある、Tx は スキーマやテーブルの取得前に startTransaction() できるのか？
 * これをハッキリさせないと、Tx の流用が可能なのか分からない。
*/














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
 * mysql> use information_schema;
 * mysql> select table_schema, table_name, engine from tables where table_name = 'person';
 * 
 * | cheshire     | person     | InnoDB |
 * 
 * 勿論上記を確認した。
*/

int test_mysqlx_tx_insert() {
    puts("=== test_mysqlx_tx_insert");
    mysqlx::Session sess("localhost", 33060, "derek", "derek1234");
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
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_mysqlx_tx_insert());
        assert(ret == 1);
    }
    if(0){   // 3.00
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
