#include "../../inc/test_1.hpp"



class Widget {
public:
    Widget(const int& _value): value(_value) 
    {}
    int getValue() const { return value; }
private:
    int value;
};

/**
 * 次は Connection 管理の仕組みを考えてみる。
 * 
 * 既にどこかにあると思うが、Pool する仕組みをイメージしている。
 * トランザクションを考えると、read-only と更新系で 2 つ Pool するオブジェクトを用意する必要があるかもしれない。
 * read-only でも begin と commit を明示してコーディングに含めれば 1 つでもいい。
*/

int test_ConnectionPool() {
    puts("=== test_ConnectionPool");
    try {
        Widget* wp1 = new Widget(21);
        Widget* wp2 = new Widget(24);
        Widget* wp3 = new Widget(27);

        ConnectionPool<Widget> cp;
        cp.push(wp1);
        cp.push(wp2);
        cp.push(wp3);

        const Widget* wp = cp.pop();
        ptr_lambda_debug<const char*, const int&>("value is ", wp->getValue()); 
        wp = cp.pop();
        ptr_lambda_debug<const char*, const int&>("value is ", wp->getValue()); 
        wp = cp.pop();
        ptr_lambda_debug<const char*, const int&>("value is ", wp->getValue()); 
        ptr_lambda_debug<const char*, const bool&>("cp is empty ? ", cp.empty()); 

        wp = cp.pop();          // これは nullptr
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}


// namespace cheshire {
extern    ConnectionPool<sql::Connection> app_cp;
    void mysql_connection_pool(const std::string& server, const std::string& user, const std::string& password, const int& sum) 
    {
        sql::Driver* driver = MySQLDriver::getInstance().getDriver();
        for(int i=0; i<sum; i++) {
            sql::Connection* con = driver->connect(server, user, password);
            if(con->isValid()) {
                puts("connected ... ");
                con->setSchema("cheshire");
                // auto commit は true としておく、Tx が必要な場合はリポジトリで明確にすること。あるいは MySQLTx を利用すること。
                app_cp.push(con);
            } else {
                puts("connection is invalid ... ");            
            }
        }
    }

// }   // end namespace cheshire

int test_mysql_connection_pool_A() {
    puts("=== test_mysql_connection_pool_A");
    try {
        mysql_connection_pool("tcp://127.0.0.1:3306", "derek", "derek1234", 2);

        ptr_lambda_debug<const char*, const bool&>("empty ? ", app_cp.empty());
        assert(app_cp.empty() == 0);      // プールされていることを期待する
        sql::Connection* con_1 = app_cp.pop();
        ptr_lambda_debug<const char*, const sql::Connection*>("con_1 addr is ", con_1);
        std::unique_ptr<MySQLConnection> mcon_1 = std::make_unique<MySQLConnection>(con_1);
        std::string sql("SELECT id, name, email, age FROM person WHERE id = ?");
        std::unique_ptr<sql::PreparedStatement> prep_stmt_1(mcon_1->prepareStatement(sql));
        // ... do something
        // コネクションの利用が終わったら返却する
        app_cp.push(con_1);

        sql::Connection* con_2 = app_cp.pop();
        ptr_lambda_debug<const char*, const sql::Connection*>("con_2 addr is ", con_2);
        std::unique_ptr<MySQLConnection> mcon_2 = std::make_unique<MySQLConnection>(con_2);
        std::unique_ptr<sql::PreparedStatement> prep_stmt_2(mcon_2->prepareStatement(sql));
        // ... do something
        app_cp.push(con_2);

        /**
         * これを踏まえて テスト B を行ってみる。
        */
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_mysql_connection_pool_B() {
    puts("=== test_mysql_connection_pool_B");
    try {
        ptr_lambda_debug<const char*, const bool&>("empty ? ", app_cp.empty());
        assert(app_cp.empty() == 0);      // プールされていることを期待する
        sql::Connection* con_1 = app_cp.pop();
        ptr_lambda_debug<const char*, const sql::Connection*>("con_1 addr is ", con_1);
        std::unique_ptr<MySQLConnection> mcon_1 = std::make_unique<MySQLConnection>(con_1);
        std::string sql("SELECT id, name, email, age FROM person WHERE id = ?");
        std::unique_ptr<sql::PreparedStatement> prep_stmt_1(mcon_1->prepareStatement(sql));
        // ... do something
        // コネクションの利用が終わったら返却する
        app_cp.push(con_1);

        sql::Connection* con_2 = app_cp.pop();
        ptr_lambda_debug<const char*, const sql::Connection*>("con_2 addr is ", con_2);
        std::unique_ptr<MySQLConnection> mcon_2 = std::make_unique<MySQLConnection>(con_2);
        std::unique_ptr<sql::PreparedStatement> prep_stmt_2(mcon_2->prepareStatement(sql));
        // ... do something
        app_cp.push(con_2);
        /**
         * テスト A、B でコネクションのアドレスが同じであることが重要。
         * 返却するタイミングでは、前後は異なると考える（ConnectionPool の内部では std::queue を利用している）。
        */

        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)>(e);
        return EXIT_FAILURE;
    }
}

/**
 * うん、分かったようなそうでもないような感じなのだが。
 * 
 * -c オプションを付けた中間ファイルでは -l のリンクは意味がない？
 * この辺は、ビルド及びコンパイルに関する知識が足らないので、検証した結果
 * 導いた結論であり、別のやり方があるのかもしれないが、test_1.o を作らず
 * に、main.cpp で実行ファイルを作る際に test_1.cpp を含めることで、今まで
 * 上手くビルドできなかったことが解消された。
 * だから、以前まで移設できずにいた下記関数（test_mysql_connect）もここ
 * に移動できた。
 * @see main.cpp のコンパイル例参照。
*/

int test_mysql_connect() {              // これが test_1.cpp に移設できない orz
    puts("=== test_mysql_connect");
    sql::Driver* driver = nullptr;
    sql::Connection*          con    = nullptr;
    sql::Statement *          stmt   = nullptr;
    try {
        driver = get_driver_instance();     // 移設できない理由が、これ get_driver_instance が no reference になる。必要な情報を与えてもやはり上手く行かなかった。
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

int test_MySQLTx() {
    puts("=== test_MySQLTx");
    std::unique_ptr<sql::Connection> con    = nullptr;
    sql::Connection*                 rawCon = nullptr;
    std::unique_ptr<MySQLConnection> mcon   = nullptr;
    try {
        if(app_cp.empty()) {
            sql::Driver* driver = MySQLDriver::getInstance().getDriver();
            con = std::move(std::unique_ptr<sql::Connection>(driver->connect("tcp://127.0.0.1:3306", "derek", "derek1234")));
            if(con->isValid()) {
                puts("connected ... ");
                con->setSchema("cheshire");
                mcon = std::make_unique<MySQLConnection>(con.get());
            } else {
                throw std::runtime_error("Invalid connection.");                
            }
        } else {
            puts("It use connection pooling ... ");
            rawCon = app_cp.pop();
            mcon = std::make_unique<MySQLConnection>(rawCon);
        }
        // コネクション取得後から計測を開始する
        std::clock_t start = clock();
        std::unique_ptr<Repository<PersonData,std::size_t>> repo = std::make_unique<PersonRepository>(PersonRepository(mcon.get()));
            
        std::string expect_name("Alice");
        std::string expect_email("alice@loki.org");
        int expect_age = 12;
        std::unique_ptr<RdbDataStrategy<PersonData>> strategy = std::make_unique<PersonStrategy>(PersonStrategy());
        DataField<std::string> name("name", expect_name);
        DataField<std::string> email("email", expect_email);
        DataField<int> age("age", expect_age);
        PersonData alice(strategy.get(),name,email,age);
        // std::optional<PersonData> after = repo->insert(alice);       // リポジトリの単体動作は OK だった。
        // auto [id_nam, id_val] = after.value().getId().bind();
        // ptr_lambda_debug<const char*, const decltype(id_val)&>("after id_val is ", id_val);

        // こんなコーディングを見るとやっぱり、factory がほしくなるよね。
        std::unique_ptr<RdbProcStrategy<PersonData>> proc_strategy = std::make_unique<MySQLCreateStrategy<PersonData,std::size_t>>(repo.get(), alice);
        // MySQLTx(RdbConnection<sql::PreparedStatement>* _con, const RdbProcStrategy<DATA>* _strategy)
        MySQLTx tx(mcon.get(), proc_strategy.get());
        std::optional<PersonData> after = tx.executeTx();
        if(rawCon) {
            app_cp.push(rawCon);
        }
        // 検査
        assert(after.has_value() == true);
        auto [id_nam, id_val] = after.value().getId().bind();
        printf("name is %s\t", id_nam.c_str());
        ptr_lambda_debug<const char*, const decltype(id_val)&>("after id_val is ", id_val);
        auto [name_nam, name_val] = after.value().getName().bind();
        printf("name is %s\n", name_nam.c_str());
        assert(name_val == expect_name);
        auto [email_nam, email_val] = after.value().getEmail().bind();
        printf("name is %s\n", email_nam.c_str());
        assert(email_val == expect_email);
        auto [age_nam, age_val] = after.value().getAge().value().bind();
        printf("name is %s\n", age_nam.c_str());
        assert(age_val == expect_age);
        std::clock_t end = clock();
        std::cout << "passed " << (double)(end-start)/CLOCKS_PER_SEC << " sec." << std::endl;
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)>(e);
        return EXIT_FAILURE;
    }
}

int test_MySQLTx_rollback() {
    puts("=== test_MySQLTx_rollback");
    try {
        sql::Driver* driver = MySQLDriver::getInstance().getDriver();
        std::unique_ptr<sql::Connection> con = std::move(std::unique_ptr<sql::Connection>(driver->connect("tcp://127.0.0.1:3306", "derek", "derek1234")));
        if(con->isValid()) {
            puts("connected ... ");
            con->setSchema("cheshire");

            std::unique_ptr<MySQLConnection> mcon = std::make_unique<MySQLConnection>(con.get()); 
            std::unique_ptr<Repository<PersonData,std::size_t>> repo = std::make_unique<PersonRepository>(PersonRepository(mcon.get()));
            
            std::string expect_name("Alice2rollback");
            std::string expect_email("alice@loki.org");     // これが 一意制約に抵触する
            int expect_age = 12;
            std::unique_ptr<RdbDataStrategy<PersonData>> strategy = std::make_unique<PersonStrategy>(PersonStrategy());
            DataField<std::string> name("name", expect_name);
            DataField<std::string> email("email", expect_email);
            DataField<int> age("age", expect_age);
            PersonData alice(strategy.get(),name,email,age);
            // こんなコーディングを見るとやっぱり、factory がほしくなるよね。
            std::unique_ptr<RdbProcStrategy<PersonData>> proc_strategy = std::make_unique<MySQLCreateStrategy<PersonData,std::size_t>>(repo.get(), alice);
            // MySQLTx(RdbConnection<sql::PreparedStatement>* _con, const RdbProcStrategy<DATA>* _strategy)
            MySQLTx tx(mcon.get(), proc_strategy.get());
            std::optional<PersonData> after = tx.executeTx();
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_MySQLTx_Create(std::size_t* insId) {
    puts("=== test_MySQLTx_Create");
    sql::Connection*                                            rawCon = nullptr;
    try {
        if(!app_cp.empty()) {
            std::clock_t start = clock();
                                                                rawCon = app_cp.pop();
            std::unique_ptr<MySQLConnection>                    mcon = std::make_unique<MySQLConnection>(rawCon);
            std::unique_ptr<Repository<PersonData,std::size_t>> repo = std::make_unique<PersonRepository>(PersonRepository(mcon.get()));                
            std::string expect_name("Dante");
            std::string expect_email("dante@loki.org");
            int expect_age = 39;
            std::unique_ptr<RdbDataStrategy<PersonData>> strategy = std::make_unique<PersonStrategy>();
            DataField<std::string> name("name", expect_name);
            DataField<std::string> email("email", expect_email);
            DataField<int> age("age", expect_age);
            PersonData dante(strategy.get(),name,email,age);

            std::unique_ptr<RdbProcStrategy<PersonData>> proc_strategy = std::make_unique<MySQLCreateStrategy<PersonData,std::size_t>>(repo.get(), dante);
            MySQLTx tx(mcon.get(), proc_strategy.get());
            std::optional<PersonData> after = tx.executeTx();
            // この仕組みは再考の余地がある、エラーが起きた時は、誰がどこで、コネクションを返却するのか？
            if(rawCon) {
                app_cp.push(rawCon);
            }
            // 検査
            assert(after.has_value() == true);
            auto [id_nam, id_val] = after.value().getId().bind();
            *insId = id_val;
            printf("name is %s\t", id_nam.c_str());
            ptr_lambda_debug<const char*, const decltype(id_val)&>("after id_val is ", id_val);
            auto [name_nam, name_val] = after.value().getName().bind();
            printf("name is %s\t value is %s\n", name_nam.c_str(), name_val.c_str());
            assert(name_val == expect_name);
            auto [email_nam, email_val] = after.value().getEmail().bind();
            printf("name is %s\t value is %s\n", email_nam.c_str(), email_val.c_str());
            assert(email_val == expect_email);
            auto [age_nam, age_val] = after.value().getAge().value().bind();
            printf("name is %s\t value is %d\n", age_nam.c_str(), age_val);
            assert(age_val == expect_age);

            std::clock_t end = clock();
            std::cout << "passed " << (double)(end-start)/CLOCKS_PER_SEC << " sec." << std::endl;
        } else {
            throw std::runtime_error("No connection pooling.");
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_MySQLTx_Read(std::size_t* insId) {
    puts("=== test_MySQLTx_Read");
    std::size_t danteId = *insId;
    ptr_lambda_debug<const char*, std::size_t&>("danteId is ", danteId);
    sql::Connection*                                            rawCon = nullptr;
    try {
        if(!app_cp.empty()) {
            std::clock_t start = clock();
                                                                rawCon = app_cp.pop();
            std::unique_ptr<MySQLConnection>                    mcon = std::make_unique<MySQLConnection>(rawCon);
            std::unique_ptr<Repository<PersonData,std::size_t>> repo = std::make_unique<PersonRepository>(PersonRepository(mcon.get()));                
            std::unique_ptr<RdbProcStrategy<PersonData>> proc_strategy = std::make_unique<MySQLReadStrategy<PersonData,std::size_t>>(repo.get(), danteId);
            MySQLTx tx(mcon.get(), proc_strategy.get());
            std::optional<PersonData> after = tx.executeTx();
            // この仕組みは再考の余地がある、エラーが起きた時は、誰がどこで、コネクションを返却するのか？
            if(rawCon) {
                app_cp.push(rawCon);
            }
            // 検証
            assert(after.has_value() == true);
            ptr_lambda_debug<const char*, const std::size_t&>("id is ", after.value().getId().getValue());
            ptr_lambda_debug<const char*, const std::string&>("name is ", after.value().getName().getValue());
            ptr_lambda_debug<const char*, const std::string&>("email is ", after.value().getEmail().getValue());
            ptr_lambda_debug<const char*, const int&>("age is ", after.value().getAge().value().getValue());
            
            std::clock_t end = clock();
            std::cout << "passed " << (double)(end-start)/CLOCKS_PER_SEC << " sec." << std::endl;
        } else {
            throw std::runtime_error("No connection pooling.");
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_MySQLTx_Update(std::size_t* insId) {
    puts("=== test_MySQLTx_Update");
    std::size_t danteId = *insId;
    ptr_lambda_debug<const char*, std::size_t&>("danteId is ", danteId);
    sql::Connection*                                            rawCon = nullptr;
    try {
        if(!app_cp.empty()) {
            std::clock_t start = clock();
            std::unique_ptr<RdbDataStrategy<PersonData>> dataStrategy = std::make_unique<PersonStrategy>();
            std::string expect_name  = "Dante Updated";
            std::string expect_email = "derek_updated@loki.org";
            int         expect_age   = 40;
            DataField<std::size_t> id("id"  , danteId);
            DataField<std::string> name("name"  , expect_name);
            DataField<std::string> email("email", expect_email);
            DataField<int>         age("age"  , expect_age);
            PersonData data(dataStrategy.get(), id, name, email, age);

                                                                rawCon        = app_cp.pop();
            std::unique_ptr<MySQLConnection>                    mcon          = std::make_unique<MySQLConnection>(rawCon);
            std::unique_ptr<Repository<PersonData,std::size_t>> repo          = std::make_unique<PersonRepository>(PersonRepository(mcon.get()));                
            std::unique_ptr<RdbProcStrategy<PersonData>>        proc_strategy = std::make_unique<MySQLUpdateStrategy<PersonData,std::size_t>>(repo.get(), data);
            MySQLTx tx(mcon.get(), proc_strategy.get());
            std::optional<PersonData> after = tx.executeTx();
            // この仕組みは再考の余地がある、エラーが起きた時は、誰がどこで、コネクションを返却するのか？
            if(rawCon) {
                app_cp.push(rawCon);
            }

            // 検証
            assert(after.has_value() == true);
            assert(after.value().getId().getValue()          == danteId);
            assert(after.value().getName().getValue()        == expect_name);
            assert(after.value().getEmail().getValue()       == expect_email);
            assert(after.value().getAge().value().getValue() == expect_age);
            ptr_lambda_debug<const char*, const std::size_t&>("id value is ", after.value().getId().getValue());
            ptr_lambda_debug<const char*, const std::string&>("name value is ", after.value().getName().getValue());
            ptr_lambda_debug<const char*, const std::string&>("email value is ", after.value().getEmail().getValue());
            ptr_lambda_debug<const char*, const int&>("age value is ", after.value().getAge().value().getValue());
            
            std::clock_t end = clock();
            std::cout << "passed " << (double)(end-start)/CLOCKS_PER_SEC << " sec." << std::endl;
        } else {
            throw std::runtime_error("No connection pooling.");
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_MySQLTx_Delete(std::size_t* insId) {
    puts("=== test_MySQLTx_Delete");
    std::size_t danteId = *insId;
    ptr_lambda_debug<const char*, std::size_t&>("danteId is ", danteId);
    sql::Connection*                                            rawCon = nullptr;
    try {
        if(!app_cp.empty()) {
            std::clock_t start = clock();
                                                                rawCon          = app_cp.pop();
            std::unique_ptr<MySQLConnection>                    mcon            = std::make_unique<MySQLConnection>(rawCon);
            std::unique_ptr<Repository<PersonData,std::size_t>> repo            = std::make_unique<PersonRepository>(PersonRepository(mcon.get()));                
            std::unique_ptr<RdbProcStrategy<PersonData>>        proc_strategy_d = std::make_unique<MySQLDeleteStrategy<PersonData,std::size_t>>(repo.get(), danteId);
            MySQLTx tx(mcon.get(), proc_strategy_d.get());
            tx.executeTx();     // これも戻り値があるが、強制的に空の optional を返却しているので、検証に使用するのは妥当ではない。
            std::clock_t end = clock();
            std::cout << "passed " << (double)(end-start)/CLOCKS_PER_SEC << " sec." << std::endl;

            // 検証
            std::unique_ptr<RdbProcStrategy<PersonData>> proc_strategy_r = std::make_unique<MySQLReadStrategy<PersonData,std::size_t>>(repo.get(), danteId);
            MySQLTx tx_r(mcon.get(), proc_strategy_r.get());
            std::optional<PersonData> after = tx_r.executeTx();
            // この仕組みは再考の余地がある、エラーが起きた時は、誰がどこで、コネクションを返却するのか？
            if(rawCon) {
                app_cp.push(rawCon);
            }
            assert(after.has_value() == false);
        } else {
            throw std::runtime_error("No connection pooling.");
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    } 
}

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
        std::unique_ptr<RdbDataStrategy<PersonData>> strategy = std::make_unique<PersonStrategy>(PersonStrategy());
        DataField<std::string> name("name", "Derek");
        DataField<std::string> email("email", "derek@loki.org");
        DataField<int> age("age", 21);
        PersonData derek(strategy.get(),name,email,age);

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

        // dumny 動作確認
        PersonData dummy = PersonData::dummy();
        ptr_lambda_debug<const char*, const std::size_t&>("id value is ", dummy.getId().getValue());
        ptr_lambda_debug<const char*, const std::string&>("name value is ", dummy.getName().getValue());
        ptr_lambda_debug<const char*, const std::string&>("email value is ", dummy.getEmail().getValue());
        ptr_lambda_debug<const char*, const int&>("age value is ", dummy.getAge().value().getValue());
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_makeInsertSql() {
    puts("=== test_makeInsertSql");
    try {
        std::unique_ptr<RdbDataStrategy<PersonData>> strategy = std::make_unique<PersonStrategy>(PersonStrategy());
        DataField<std::string> name("name", "Derek");
        DataField<std::string> email("email", "derek@loki.org");
        DataField<int> age("age", 21);
        PersonData derek(strategy.get(),name,email,age);

        auto sql = makeInsertSql(derek.getTableName(), derek.getColumns());
        ptr_lambda_debug<const char*,const decltype(sql)&>("sql: ", sql);

        std::unique_ptr<RdbDataStrategy<PersonData>> strategy2 = std::make_unique<PersonStrategy>(PersonStrategy());
        DataField<std::string> name2("name", "Cheshire");
        DataField<std::string> email2("email", "cheshire@loki.org");
        std::optional<DataField<int>> empty_age;
        PersonData cheshire(strategy2.get(),name2,email2,empty_age);

        auto sql2 = makeInsertSql(cheshire.getTableName(), cheshire.getColumns());
        ptr_lambda_debug<const char*,const decltype(sql2)&>("sql2: ", sql2);
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_makeUpdateSql() {
    puts("=== test_makeUpdateSql");
    try {
        std::unique_ptr<RdbDataStrategy<PersonData>> strategy = std::make_unique<PersonStrategy>(PersonStrategy());
        DataField<std::string> name("name", "Derek");
        DataField<std::string> email("email", "derek@loki.org");
        DataField<int> age("age", 21);
        PersonData derek(strategy.get(),name,email,age);
        auto[pk_nam, pk_val] = derek.getId().bind();
        auto sql = makeUpdateSql(derek.getTableName(),pk_nam, derek.getColumns());
        ptr_lambda_debug<const char*,const decltype(sql)&>("sql: ", sql);

        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_makeDeleteSql() {
    puts("=== test_makeDeleteSql");
    try {
        std::unique_ptr<RdbDataStrategy<PersonData>> strategy = std::make_unique<PersonStrategy>(PersonStrategy());
        DataField<std::string> name("name", "Derek");
        DataField<std::string> email("email", "derek@loki.org");
        DataField<int> age("age", 21);
        PersonData derek(strategy.get(),name,email,age);
        auto[pk_nam, pk_val] = derek.getId().bind();
        auto sql = makeDeleteSql(derek.getTableName(),pk_nam);
        ptr_lambda_debug<const char*,const decltype(sql)&>("sql: ", sql);

        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_makeFindOneSql() {
    puts("=== test_makeFindOneSql");
    try {
        std::unique_ptr<RdbDataStrategy<PersonData>> strategy = std::make_unique<PersonStrategy>(PersonStrategy());
        DataField<std::string> name("name", "Derek");
        DataField<std::string> email("email", "derek@loki.org");
        DataField<int> age("age", 21);
        PersonData derek(strategy.get(),name,email,age);
        auto[pk_nam, pk_val] = derek.getId().bind();
        auto sql = makeFindOneSql(derek.getTableName(), pk_nam, derek.getColumns());
        ptr_lambda_debug<const char*, const decltype(sql)&>("sql: ", sql);

        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_makeCreateTableSql() {
    puts("=== test_makeCreateTableSql");
    try {
        std::unique_ptr<RdbDataStrategy<PersonData>> strategy = std::make_unique<PersonStrategy>(PersonStrategy());
        DataField<std::size_t> id("id", 0, "BIGINT", "AUTO_INCREMENT PRIMARY KEY");  // MySQL でこの構文で問題がないか要検証。
        DataField<std::string> name("name", "Derek", "VARCHAR(128)", "NOT NULL");
        DataField<std::string> email("email", "derek@loki.org", "VARCHAR(256)", "NOT NULL UNIQUE");
        DataField<int> age("age", 21, "INT", "");
        PersonData derek(strategy.get(),id,name,email,age);
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

int test_insert_person() {
    puts("=== test_insert_person");
    std::clock_t start = clock();
    sql::Driver* driver = nullptr;
    std::unique_ptr<sql::Connection> con = nullptr;
    try {
        std::unique_ptr<RdbDataStrategy<PersonData>> strategy = std::make_unique<PersonStrategy>(PersonStrategy());
        DataField<std::string> name("name", "Derek");
        DataField<std::string> email("email", "derek@loki.org");
        DataField<int> age("age", 21);
        PersonData derek(strategy.get(),name,email,age);
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
                ptr_lambda_debug<const char*,const decltype(sql_2)&>("sql_2: ", sql_2);
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

int test_PersonRepository_findOne() {
    puts("=== test_PersonRepository_findOne");
    try {
        sql::Driver* driver = MySQLDriver::getInstance().getDriver();
        std::unique_ptr<sql::Connection> con = std::move(std::unique_ptr<sql::Connection>(driver->connect("tcp://127.0.0.1:3306", "derek", "derek1234")));
        if(con->isValid()) {
            puts("connected ... ");
            con->setSchema("cheshire");
            std::unique_ptr<MySQLConnection> mcon = std::make_unique<MySQLConnection>(con.get()); 
            std::unique_ptr<Repository<PersonData,std::size_t>> repo = std::make_unique<PersonRepository>(PersonRepository(mcon.get()));
            std::optional<PersonData> result = repo->findOne(1ul);
            
            assert(result.has_value() == true);
            auto [id_nam, id_val] = result.value().getId().bind();
            printf("name is %s\t", id_nam.c_str());
            ptr_lambda_debug<const char*, const decltype(id_val)&>("result id_val is ", id_val);
            assert(id_val == 1ul);
            auto [name_nam, name_val] = result.value().getName().bind();
            printf("name is %s\t value is %s\n", name_nam.c_str(), name_val.c_str());
            auto [email_nam, email_val] = result.value().getEmail().bind();
            printf("name is %s\t value is %s\n", email_nam.c_str(), email_val.c_str());
            auto [age_nam, age_val] = result.value().getAge().value().bind();
            printf("name is %s\t value is %d\n", age_nam.c_str(), age_val);     // 現状では、非常に細かいことに聞こえるが、age は std::optional なので RDB のレコードの値が NULL の場合もチェックしてほしい。
        } else {
            throw std::runtime_error("Invalid connection.");
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_PersonRepository_update() {
    puts("=== test_PersonRepository_update");
    try {
        sql::Driver* driver = MySQLDriver::getInstance().getDriver();
        std::unique_ptr<sql::Connection> con = std::move(std::unique_ptr<sql::Connection>(driver->connect("tcp://127.0.0.1:3306", "derek", "derek1234")));

        if(con->isValid()) {
            puts("connected ... ");
            con->setSchema("cheshire");
            std::unique_ptr<MySQLConnection> mcon = std::make_unique<MySQLConnection>(con.get()); 
            std::unique_ptr<Repository<PersonData,std::size_t>> repo = std::make_unique<PersonRepository>(PersonRepository(mcon.get()));
            std::optional<PersonData> updateData = repo->findOne(1ul);
            if(updateData.has_value()) {
                std::unique_ptr<RdbDataStrategy<PersonData>> dataStrategy = std::make_unique<PersonStrategy>(PersonStrategy());
                PersonData data = updateData.value();
                data.setDataStrategy(dataStrategy.get());
                std::string expect_name  = "DEREK_2";
                std::string expect_email = "derek_2@loki.org";
                int         expect_age   = 33;
                DataField<std::string> name("name"  , expect_name);
                DataField<std::string> email("email", expect_email);
                DataField<int>         age("age"  , expect_age);

                data.setName(name);
                data.setEmail(email);
                data.setAge(age);
                std::optional<PersonData> result = repo->update(data);
                assert(result.has_value() == true);
                if(result.has_value()) {
                    ptr_lambda_debug<const char*, const std::string&>("result name is ", result.value().getName().getValue());
                    ptr_lambda_debug<const char*, const std::string&>("result email is ", result.value().getEmail().getValue());
                    ptr_lambda_debug<const char*, const int&>("result age is ", result.value().getAge().value().getValue());
                    // TODO id 以外のすべてのデータの確認が必要
                    assert(expect_name == result.value().getName().getValue()); 
                    assert(expect_email == result.value().getEmail().getValue());
                    assert(expect_age == result.value().getAge().value().getValue());
                }
            } else {
                throw std::runtime_error("Not found test data.");
            }
        } else {
            throw std::runtime_error("Invalid connection.");
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_PersonRepository_insert() {
    puts("=== test_PersonRepository_insert");
    try {
        sql::Driver* driver = MySQLDriver::getInstance().getDriver();
        std::unique_ptr<sql::Connection> con = std::move(std::unique_ptr<sql::Connection>(driver->connect("tcp://127.0.0.1:3306", "derek", "derek1234")));
        if(con->isValid()) {
            puts("connected ... ");
            con->setSchema("cheshire");
            std::unique_ptr<MySQLConnection> mcon = std::make_unique<MySQLConnection>(con.get()); 
            std::unique_ptr<RdbDataStrategy<PersonData>> strategy = std::make_unique<PersonStrategy>(PersonStrategy());
            std::string expect_name  = std::string("cheshire");
            std::string expect_email = std::string("cheshire@loki.org");
            int         expect_age   = 3;
            PersonData data = PersonData::factory(expect_name, expect_email, expect_age, strategy.get());
            std::unique_ptr<Repository<PersonData,std::size_t>> repo = std::make_unique<PersonRepository>(mcon.get());
            std::optional<PersonData> opt = repo->insert(data);

            assert(opt.has_value() == true);
            if(opt.has_value()) {
                assert(expect_name  == opt.value().getName().getValue());
                assert(expect_email == opt.value().getEmail().getValue());
                assert(expect_age   == opt.value().getAge().value().getValue());
            }
        } else {
            throw std::runtime_error("Invalid connection.");
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_PersonRepository_insert_no_age() {
    puts("=== test_PersonRepository_insert_no_age");
    try {
        sql::Driver* driver = MySQLDriver::getInstance().getDriver();
        std::unique_ptr<sql::Connection> con = std::move(std::unique_ptr<sql::Connection>(driver->connect("tcp://127.0.0.1:3306", "derek", "derek1234")));
        if(con->isValid()) {
            puts("connected ... ");
            con->setSchema("cheshire");
            std::unique_ptr<MySQLConnection> mcon = std::make_unique<MySQLConnection>(con.get()); 
            std::unique_ptr<RdbDataStrategy<PersonData>> strategy = std::make_unique<PersonStrategy>(PersonStrategy());
            std::string expect_name  = std::string("cheshire_2");
            std::string expect_email = std::string("cheshire_2@loki.org");
            // int         expect_age   = 3;
            PersonData data = PersonData::factory(expect_name, expect_email, strategy.get());
            std::unique_ptr<Repository<PersonData,std::size_t>> repo = std::make_unique<PersonRepository>(mcon.get());
            std::optional<PersonData> opt = repo->insert(data);

            assert(opt.has_value() == true);
            if(opt.has_value()) {
                assert(expect_name  == opt.value().getName().getValue());
                assert(expect_email == opt.value().getEmail().getValue());
                assert(opt.value().getAge().has_value() == false);
            }
        } else {
            throw std::runtime_error("Invalid connection.");
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_PersonRepository_remove() {
    puts("=== test_PersonRepository_remove");
    try {
        sql::Driver* driver = MySQLDriver::getInstance().getDriver();
        std::unique_ptr<sql::Connection> con = std::move(std::unique_ptr<sql::Connection>(driver->connect("tcp://127.0.0.1:3306", "derek", "derek1234")));
        if(con->isValid()) {
            puts("connected ... ");
            con->setSchema("cheshire");
            std::unique_ptr<MySQLConnection> mcon = std::make_unique<MySQLConnection>(con.get());
            std::size_t remove_id(1ul);
            std::unique_ptr<Repository<PersonData,std::size_t>> repo = std::make_unique<PersonRepository>(mcon.get());
            repo->remove(remove_id);

            // 検証
            std::optional<PersonData> test = repo->findOne(remove_id);
            assert(test.has_value() == false);
        } else {
            throw std::runtime_error("Invalid connection.");
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}
