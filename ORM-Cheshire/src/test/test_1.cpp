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
