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
 * g++ -O3 -DDEBUG -std=c++20 -I../inc/ -I/usr/include/mysql-cppconn-8/ -L/usr/lib/x86_64-linux-gnu/ -pedantic-errors -Wall -Werror main.cpp -lmysqlcppconn -lmysqlcppconn8 ./model/PersonStrategy.cpp ./data/PersonData.cpp ./driver/MySQLDriver.cpp -o ../bin/main
 * 
 * e.g. compile B. 
 * 分割した方が少しだけコンパイル時間が短縮できるかな、体感値で申し訳ないが。
 * 以下の 3 行を毎回実行する必要はなく、必要に応じて適宜、上 2 行は実行すれば良い、main.cpp と比較した場合、アーキテクチャ上の
 * 上位と見ることができる。 main.cpp ほど修正が入らないため。どこかのタイミングで、Makefile にはしたい。
 * 
 * g++ -O3 -DDEBUG -std=c++20 -I../inc/ -pedantic-errors -Wall -Werror -c ./data/PersonData.cpp -o ../bin/PersonData.o
 * g++ -O3 -DDEBUG -std=c++20 -I../inc/ -pedantic-errors -Wall -Werror -c ./model/PersonStrategy.cpp -o ../bin/PersonStrategy.o
 * g++ -O3 -DDEBUG -std=c++20 -I../inc/ -pedantic-errors -Wall -Werror -c ./driver/MySQLDriver.cpp -o ../bin/MySQLDriver.o
 * g++ -O3 -DDEBUG -std=c++20 -I../inc/ -I/usr/include/mysql-cppconn-8/ -L/usr/lib/x86_64-linux-gnu/ -pedantic-errors -Wall -Werror main.cpp -lmysqlcppconn -lmysqlcppconn8 ../bin/PersonStrategy.o ../bin/PersonData.o ../bin/MySQLDriver.o -o ../bin/main
*/

#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include <memory>
#include <optional>
#include <set>
#include <chrono>
#include "../inc/Debug.hpp"
#include "../inc/DataField.hpp"
#include "../inc/RdbStrategy.hpp"
#include "../inc/PersonStrategy.hpp"
#include "../inc/PersonData.hpp"
#include "../inc/MySQLDriver.hpp"
#include "/usr/include/mysql-cppconn-8/mysql/jdbc.h"
#include "/usr/include/mysql-cppconn-8/mysqlx/xdevapi.h"

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

int test_DataField() {
    puts("=== test_DataField");
    try {
        DataField<int> d1("id",1);
        auto[name, value] = d1.bind();
        ptr_lambda_debug<const char*,const decltype(value)&>("value is ", value);
        ptr_lambda_debug<const char*,const std::string&>("value type is ", typeid(value).name());   // Boost のライブラリの方が正確との情報があった。
        ptr_lambda_debug<const char*,const decltype(name)&>("name is ", name);

        DataField<std::string> d2("name", "Alice");
        auto[name2, value2] = d2.bind();
        ptr_lambda_debug<const char*,const decltype(value2)&>("value2 is ", value2);
        ptr_lambda_debug<const char*,const std::string&>("value2 type is ", typeid(value2).name());   // Boost のライブラリの方が正確との情報があった。
        ptr_lambda_debug<const char*,const decltype(name2)&>("name2 is ", name2);
        /**
         * TODO Jack
         * 時間を見て、MySQL にある型と一致する C++ の型を網羅させる。
        */
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_DataField_2() {
    puts("=== test_DataField_2");
    try {
        // タプルの動作確認
        DataField<int> d1("id", 3, "integer");
        auto[name, value, type] = d1.bindTuple();
        ptr_lambda_debug<const char*, const decltype(name)&>("name is ", name);
        ptr_lambda_debug<const char*, const decltype(value)&>("value is ", value);
        ptr_lambda_debug<const char*, const decltype(type)&>("sql type is ", type);
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_DataField_3() {
    puts("=== test_DataField_3");
    try {
        DataField<int> d1("id", 3, "integer", "PRIMARY KEY");
        auto[name, type, constraint] = d1.bindTupleTblInfo();
        ptr_lambda_debug<const char*, const decltype(name)&>("name is ", name);
        ptr_lambda_debug<const char*, const decltype(type)&>("sql type is ", type);
        ptr_lambda_debug<const char*, const decltype(constraint)&>("constraint is ", constraint);
        // テスト内で次をやらないと厳密なテストとは言えない。
        assert( name == "id" );
        assert( type == "integer" );
        assert( constraint == "PRIMARY KEY" );
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_PersonData() {
    puts("=== test_PersonData");
    try {
        std::unique_ptr<RdbStrategy<PersonData>> strategy = std::make_unique<PersonStrategy>(PersonStrategy());
        DataField<std::string> name("name", "Derek");
        DataField<std::string> email("email", "derek@loki.org");
        DataField<int> age("age", 21);
        PersonData derek(std::move(strategy),name,email,age);

        ptr_lambda_debug<const char*,const std::string&>("table is ", derek.getTableName());
        auto[nam, val] = derek.getName().bind();
        ptr_lambda_debug<const char*, const decltype(nam)&>("name is ", nam);        
        ptr_lambda_debug<const char*, const decltype(val)&>("value is ", val);        
        auto[nam2, val2] = derek.getEmail().bind();
        ptr_lambda_debug<const char*, const decltype(nam2)&>("name is ", nam2);        
        ptr_lambda_debug<const char*, const decltype(val2)&>("value is ", val2);
        auto[nam3, val3] = derek.getAge().value().bind();
        ptr_lambda_debug<const char*, const decltype(nam3)&>("name is ", nam3);        
        ptr_lambda_debug<const char*, const decltype(val3)&>("value is ", val3);

        /**
         * 次はこのテーブル情報を持ったクラスから、動的に SQL 生成ができればよい、具体的には以下。
         * - INSERT 文
         * - UPDATE 文
         * - DELETE 文
         * - SELECT 文 これは、findOne() のみでよい。
         * これらは定型文なので自動で SQL を生成する仕組みがあった方が絶対にいい。
         * 
        */
        auto cols = derek.getColumns();
        for(auto name: cols) {
            ptr_lambda_debug<const char*, const decltype(name)&>("name is ", name);
        }
        assert(cols.size() == 3U);
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * リポジトリ案 A
*/

template <class Data, class PKEY>
class Repository {
    bool insert(const Data&) = 0;
    bool update(const Data&) = 0;
    bool remove(const PKEY&) = 0;
    Data findOne(const PKEY&) = 0;
};

/**
 * SQL 文の構築に関する考察
 * 
 * INSERT INTO テーブル名 (列1, 列2, ...)
 * VALUES (値1, 値2, ...);
 *
 * INSERT INTO customer (name, route, saved_date, price) 
 * VALUES ('松田', 'ad2', '2023-05-24 19:49:28', 2500);
 *
 * prep_stmt = con->prepareStatement("INSERT INTO test(id, label) VALUES (?, ?)");      // MySQL の Prepared Statements の例
 * 
 * SQL インジェクションの可能性を考慮すれば、Prepared Statements の利用は必須と考える。
 * よって、今回の 動的 SQL 文の最終型は 『"INSERT INTO test (id, label) VALUES (?, ?)"』とする。
 * 
 * カラム名とその数分の ? の出力ということ。
 * 
*/

std::string makeInsertSql(const std::string& tableName, const std::vector<std::string>& colNames) {
    std::string sql("INSERT INTO ");
    sql.append(tableName);
    // カラム
    std::string cols(" (");
    // 値
    std::string vals("VALUES (");
    for(std::size_t i=0 ; i<colNames.size(); i++) {
        cols.append(colNames.at(i));
        vals.append("?");
        if( i < colNames.size()-1 ) {
            cols.append(", ");
            vals.append(", ");
        } 
    }
    cols.append(") ");
    vals.append(")");
    sql.append(std::move(cols));
    sql.append(std::move(vals));
    return sql;
}

int test_makeInsertSql() {
    puts("=== test_makeInsertSql");
    try {
        std::unique_ptr<RdbStrategy<PersonData>> strategy = std::make_unique<PersonStrategy>(PersonStrategy());
        DataField<std::string> name("name", "Derek");
        DataField<std::string> email("email", "derek@loki.org");
        DataField<int> age("age", 21);
        PersonData derek(std::move(strategy),name,email,age);

        auto sql = makeInsertSql(derek.getTableName(), derek.getColumns());
        ptr_lambda_debug<const char*,const decltype(sql)&>("sql: ", sql);

        std::unique_ptr<RdbStrategy<PersonData>> strategy2 = std::make_unique<PersonStrategy>(PersonStrategy());
        DataField<std::string> name2("name", "Cheshire");
        DataField<std::string> email2("email", "cheshire@loki.org");
        std::optional<DataField<int>> empty_age;
        PersonData cheshire(std::move(strategy2),name2,email2,empty_age);

        auto sql2 = makeInsertSql(cheshire.getTableName(), cheshire.getColumns());
        ptr_lambda_debug<const char*,const decltype(sql2)&>("sql2: ", sql2);
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * UPDATE (表名) SET (カラム名1) = (値1) WHERE id = ?
 * 
 * UPDATE table_reference
    SET col_name1 = value1 [, col_name2 = value2, ...]
    [WHERE where_condition]
 * 
 * UPDATE test SET name = ?, label = ? WHERE id = ?
 * 
 * 更新すべき対象は 1行 としたい、Pkey を条件にする。
*/

std::string makeUpdateSql(const std::string& tableName, const std::string& pkName, const std::vector<std::string>& colNames ) {
    std::string sql("UPDATE ");
    sql.append(tableName);
    std::string set(" SET ");
    std::string condition(" WHERE ");
    condition.append(pkName).append(" = ?");
    for(std::size_t i=0; i<colNames.size(); i++) {
        set.append(colNames.at(i)).append(" = ?");
        if( i < colNames.size()-1 ) {
            set.append(", ");
        } 

    }
    sql.append(set).append(condition);
    return sql;    
}

int test_makeUpdateSql() {
    puts("=== test_makeUpdateSql");
    try {
        std::unique_ptr<RdbStrategy<PersonData>> strategy = std::make_unique<PersonStrategy>(PersonStrategy());
        DataField<std::string> name("name", "Derek");
        DataField<std::string> email("email", "derek@loki.org");
        DataField<int> age("age", 21);
        PersonData derek(std::move(strategy),name,email,age);
        auto[pk_nam, pk_val] = derek.getId().bind();
        auto sql = makeUpdateSql(derek.getTableName(),pk_nam, derek.getColumns());
        ptr_lambda_debug<const char*,const decltype(sql)&>("sql: ", sql);

        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * DELETE FROM table_name WHERE id = ?
 * 
 * 削除すべき対象は 1行 としたい、Pkey を条件にする。
*/

std::string makeDeleteSql(const std::string& tableName, const std::string& pkName) {
    std::string sql("DELETE FROM ");
    sql.append(tableName).append(" WHERE ").append(pkName).append(" = ?");
    return sql;
}

int test_makeDeleteSql() {
    puts("=== test_makeDeleteSql");
    try {
        std::unique_ptr<RdbStrategy<PersonData>> strategy = std::make_unique<PersonStrategy>(PersonStrategy());
        DataField<std::string> name("name", "Derek");
        DataField<std::string> email("email", "derek@loki.org");
        DataField<int> age("age", 21);
        PersonData derek(std::move(strategy),name,email,age);
        auto[pk_nam, pk_val] = derek.getId().bind();
        auto sql = makeDeleteSql(derek.getTableName(),pk_nam);
        ptr_lambda_debug<const char*,const decltype(sql)&>("sql: ", sql);

        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * SELECT col1, col2, col3 FROM table WHERE primary-key = ?
 * SELECT primary-key, col1, col2, col3 FROM table WHERE primary-key = ?
 * 
 * 仕様の問題、現状は pkey は RDBMS の Auto Increment を利用することを想定している、これは、次の実装に影響が及ぶ。
 * - PersonStrategy::getColumns メンバ関数に pkey を含めるか否かということ。含めた場合は、別途シーケンステーブルと pkey の取得処理が必要になる。
 * - makeFindOne の SELECT 句 の pkey の取り扱い方法について、上記に依存するため、この関数も本質的には 2 つ必要と思われる。
 * 
 * colNames に pkey は存在しないものとして次の関数は実装する。
 * 
*/

std::string makeFindOneSql(const std::string& tableName, const std::string& pkeyName, const std::vector<std::string>& colNames) {
    std::string sql("SELECT ");
    std::string cols(pkeyName);
    cols.append(", ");
    for(std::size_t i = 0; i < colNames.size(); i++) {
        cols.append(colNames.at(i));
        if( i < colNames.size()-1 ) {
            cols.append(", ");
        } 
    }
    sql.append(cols).append(" FROM ").append(tableName).append(" WHERE ").append(pkeyName).append(" = ?");
    return sql;
}

int test_makeFindOneSql() {
    puts("=== test_makeFindOneSql");
    try {
        std::unique_ptr<RdbStrategy<PersonData>> strategy = std::make_unique<PersonStrategy>(PersonStrategy());
        DataField<std::string> name("name", "Derek");
        DataField<std::string> email("email", "derek@loki.org");
        DataField<int> age("age", 21);
        PersonData derek(std::move(strategy),name,email,age);
        auto[pk_nam, pk_val] = derek.getId().bind();
        auto sql = makeFindOneSql(derek.getTableName(), pk_nam, derek.getColumns());
        ptr_lambda_debug<const char*, const decltype(sql)&>("sql: ", sql);

        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * TODO 各 テーブル情報を管理するクラスに CREATE TABLE 文を自動作成する機能がほしい。
 * 
 * CREATE TABLE テーブル名(
 *  列名1 列1の型名 PRIMARY KEY,
 *  列名2 列2の型名 UNIQUE,
 *  :
 *  列名X 列Xの型名
 * )
 * 
 * ※ 問題、Key 制約の管理を RdbData の派生クラスに持たせる必要があることが判明した。
 * このメソッドは必須には該当しないし、ER 図作成アプリから SQL の生成は可能だ。また、
 * many-to-one などのリレーションなどを考え始めるとややこしくなるだろう。その点を割り
 * 切った実装としたい。つまり、あくまでも補助機能だ、であれば、RdbData の純粋仮想関数
 * という位置づけでは問題がある。
 *
 * e.g. CREATE TABLE 文（MySQL） 
CREATE TABLE person (
id    BIGINT AUTO_INCREMENT PRIMARY KEY,
name  VARCHAR(128) NOT NULL,
email VARCHAR(256) NOT NULL UNIQUE,
age   INT
);
 * 
*/

std::string makeCreateTableSql(const std::string& tableName, const std::vector<std::tuple<std::string,std::string,std::string>>& tblInfos) {
    std::string sql("CREATE TABLE ");
    sql.append(tableName).append(" (");
    std::string colsDef("");
    for(std::size_t i = 0; i < tblInfos.size(); i++) {
        auto col = tblInfos.at(i);
        colsDef.append(get<0>(col));colsDef.append(" ");colsDef.append(get<1>(col));colsDef.append(" ");colsDef.append(get<2>(col));
        if( i < tblInfos.size()-1 ) {
            colsDef.append(", ");
        }
    }
    sql.append(colsDef).append(")");
    return sql;
}

int test_makeCreateTableSql() {
    puts("=== test_makeCreateTableSql");
    try {
        std::unique_ptr<RdbStrategy<PersonData>> strategy = std::make_unique<PersonStrategy>(PersonStrategy());
        DataField<std::size_t> id("id", 0, "BIGINT", "AUTO_INCREMENT PRIMARY KEY");  // MySQL でこの構文で問題がないか要検証。
        DataField<std::string> name("name", "Derek", "VARCHAR(128)", "NOT NULL");
        DataField<std::string> email("email", "derek@loki.org", "VARCHAR(256)", "NOT NULL UNIQUE");
        DataField<int> age("age", 21, "INT", "");
        PersonData derek(std::move(strategy),id,name,email,age);
        auto tblInfos = derek.getTableInfo();
        for(auto info: tblInfos) {
            ptr_lambda_debug<const char*, const decltype(get<0>(info))&>("name is ", get<0>(info));
            ptr_lambda_debug<const char*, const decltype(get<1>(info))&>("type is ", get<1>(info));
            ptr_lambda_debug<const char*, const decltype(get<2>(info))&>("const is ", get<2>(info));
        }
        auto sql = makeCreateTableSql(derek.getTableName(),derek.getTableInfo());
        ptr_lambda_debug<const char*, const decltype(sql)&>("sql is ", sql);
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_mysql_connect() {
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

        std::unique_ptr<RdbStrategy<PersonData>> strategy = std::make_unique<PersonStrategy>(PersonStrategy());
        DataField<std::size_t> id("id", 0, "BIGINT", "AUTO_INCREMENT PRIMARY KEY");  // MySQL でこの構文で問題がないか要検証。
        DataField<std::string> name("name", "Derek", "VARCHAR(128)", "NOT NULL");
        DataField<std::string> email("email", "derek@loki.org", "VARCHAR(256)", "NOT NULL UNIQUE");
        DataField<int> age("age", 21, "INT", "");
        PersonData derek(std::move(strategy),id,name,email,age);
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

int test_insert_person() {
    puts("=== test_insert_person");
    std::clock_t start = clock();
    sql::Driver* driver = nullptr;
    std::unique_ptr<sql::Connection> con = nullptr;
    try {
        std::unique_ptr<RdbStrategy<PersonData>> strategy = std::make_unique<PersonStrategy>(PersonStrategy());
        DataField<std::string> name("name", "Derek");
        DataField<std::string> email("email", "derek@loki.org");
        DataField<int> age("age", 21);
        PersonData derek(std::move(strategy),name,email,age);
        auto sql = makeInsertSql(derek.getTableName(), derek.getColumns());
        ptr_lambda_debug<const char*,const decltype(sql)&>("sql: ", sql);

        driver = get_driver_instance();
        con = std::move(std::unique_ptr<sql::Connection>(driver->connect("tcp://127.0.0.1:3306", "derek", "derek1234")));
        // con = std::move(std::unique_ptr<sql::Connection>(driver->connect("tcp://172.22.1.64:3306", "derek", "derek1234")));
        if(con->isValid()) {
            puts("connected ... ");
            con->setSchema("cheshire");
            con->setAutoCommit(false);
            ptr_lambda_debug<const char*, const bool&>("auto commit is ",  con->getAutoCommit());
            std::unique_ptr<sql::PreparedStatement> prep_stmt(con->prepareStatement(sql));
            auto[id_nam, id_val] = derek.getId().bind();
            auto[name_nam, name_val] = derek.getName().bind();
            prep_stmt->setString(1, name_val);
            auto[email_nam, email_val] = derek.getEmail().bind();
            prep_stmt->setString(2, email_val);
            auto[age_nam, age_val] = derek.getAge().value().bind();
            prep_stmt->setInt(3, age_val);
            /**
             * ヒント
             * 
                begin;
                INSERT INTO person (name, email, age) VALUES ('a', 'a@loki.org', 30);
                SELECT LAST_INSERT_ID();
                commit;
            */
            int ret = prep_stmt->executeUpdate();
            ptr_lambda_debug<const char*, const int&>("ret is ", ret);
            // throw std::runtime_error("It's test error.");
            std::unique_ptr<sql::Statement> stmt(con->createStatement());
            std::unique_ptr<sql::ResultSet> res( stmt->executeQuery("SELECT LAST_INSERT_ID()") );
            while(res->next()) {
                puts("------ A");
                // ptr_lambda_debug<const char*, const sql::ResultSet::enum_type&>("enum_type is ", res->getType());
                auto id = res->getInt64(1);
                ptr_lambda_debug<const char*, const decltype(id)&>("id is ", id);
                ptr_lambda_debug<const char*, const std::string&>("id type is ", typeid(id).name());
                auto sql_2 = makeFindOneSql(derek.getTableName(), id_nam, derek.getColumns());
                ptr_lambda_debug<const char*,const decltype(sql)&>("sql_2: ", sql_2);
                std::unique_ptr<sql::PreparedStatement> prep_stmt_2(con->prepareStatement(sql_2));
                prep_stmt_2->setBigInt(1, std::to_string(id));
                std::unique_ptr<sql::ResultSet> res_2( prep_stmt_2->executeQuery() );
                while(res_2->next()) {
                    puts("------ B");
                    auto res_id = res_2->getUInt64(1);
                    auto res_name = res_2->getString(2);
                    auto res_email = res_2->getString(3);
                    auto res_age = res_2->getInt(4);
                    ptr_lambda_debug<const char*,const decltype(res_id)&>("res_id: ", res_id);
                    ptr_lambda_debug<const char*,const decltype(res_name)&>("res_name: ", res_name);
                    ptr_lambda_debug<const char*,const decltype(res_email)&>("res_email: ", res_email);
                    ptr_lambda_debug<const char*,const decltype(res_age)&>("res_age: ", res_age);
                    /**
                     * 最終的には ResultSet の各値を Person オブジェクトに詰めて返却するところまでを Insert のタスク
                     * としたい。RDBMS のドライバとコネクションまたトランザクションをどのように設計するかが今後のポイ
                     * ントだと考える。上記をまとめて実装するとこのような出来になってしまう。
                     * Driver Connection Transaction Repository の設計いかんでこのアプリの出来は決まってしまう。
                    */
                }
            }
            con->commit();
        }
        std::clock_t end = clock();
        std::cout << "passed " << (double)(end-start)/CLOCKS_PER_SEC << " sec." << std::endl;
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        con->rollback();
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_MySQLDriver() {
    puts("=== test_MySQLDriver");
    try {
        MySQLDriver& md1 = MySQLDriver::getInstance();
        MySQLDriver& md2 = MySQLDriver::getInstance();
        ptr_lambda_debug<const char*, MySQLDriver*>("md1 addr is ", &md1);
        ptr_lambda_debug<const char*, MySQLDriver*>("md2 addr is ", &md2);
        assert(&md1 == &md2);

        sql::Driver* d1 = md1.getDriver();
        sql::Driver* d2 = md2.getDriver();
        ptr_lambda_debug<const char*, sql::Driver*>("d1 addr is ", d1);
        ptr_lambda_debug<const char*, sql::Driver*>("d2 addr is ", d2);
        assert(d1 == d2);

        RdbDriver<sql::Driver>* rd = &(MySQLDriver::getInstance());
        ptr_lambda_debug<const char*, RdbDriver<sql::Driver>*>("rd addr is ", rd);
        assert(rd == &md1);
        sql::Driver* d3 = rd->getDriver();
        ptr_lambda_debug<const char*, sql::Driver*>("d3 addr is ", d3);
        assert(d3 == d1);

        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * 次は Connection 管理の仕組みを考えてみる。
 * 
 * 既にどこかにあると思うが、Pool する仕組みをイメージしている。
*/

/**
 * MySQL Shell
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
        mysqlx::TableSelect selectOpe = person.select("email").where("id=1");
        mysqlx::Row row = selectOpe.execute().fetchOne();
        std::cout << "row is null ? " << row.isNull() << std::endl;
        mysqlx::Value val = row.get(0);
        std::cout << val << std::endl;    
        sess.close();
        std::clock_t end = clock();
        std::cout << "passed " << (double)(end-start)/CLOCKS_PER_SEC << " sec." << std::endl;
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)>(e);
        return EXIT_FAILURE;
    }
}


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
    }
    if(0) {      // 2.00
        auto ret = 0;
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_mysqlx_connect());
        assert(ret == 0);
    }
    puts("===   Lost Chapter O/R Mapping END");
    return 0;
}
