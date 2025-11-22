/**
 * builder_template_m.cpp
 * 
 * MySQL 処理再び、今回はDocker コンテナ上のMySQL を利用する。
 * Docker Engine のインストール。
 * ```
 * sudo apt-get update
 * sudo apt-get install docker-ce docker-ce-cli containerd.io docker-compose-plugin
 * ```
 * 
 * Docker にMySQL をインストールする。
 * ```
 * docker pull mysql:8.0.44-debian
 * docker run --name mysql -e MYSQL_ROOT_PASSWORD=root1234 \
 * -p 3306:3306 -p 33060:33060 \
 * -v $(pwd)/mysql-data:/var/lib/mysql \
 * -d mysql:8.0.44-debian
 * 
 * docker exec -it mysql bash
 * mysql -u root -p
 * ```
 * 
 * データベースとテーブルの事前準備
 * ```
 * CREATE DATABASE IF NOT EXISTS test;
 * 
 * DROP TABLE IF EXISTS contractor;
 * CREATE TABLE contractor (
 *  id              SERIAL          PRIMARY KEY,
 *  company_id      VARCHAR(100)    NOT NULL,
 *  email           VARCHAR(100)    NOT NULL,
 *  password        VARCHAR(255)    NOT NULL,
 *  name            VARCHAR(100)    NOT NULL,
 *  roles           VARCHAR(100));
 * ALTER TABLE contractor ADD CONSTRAINT email_uk unique (email);
 * 
 * INSERT INTO contractor(company_id, email, password, name, roles)
 * VALUES
 * ('A1_1000', 'admin@example.com', '$2a$10$Ff.Tr2q.fBciJKiA1b4wAOwNpjr9RypX3DHeQwLQQg0GMxGrl4FcO', 'admin', 'ROLE_ADMIN'),
 * ('A1_1000', 'student@example.com', '$2a$10$fzJDR7BSMQnRg9Odg/JDzemQdLc6g2a7lR/ccHoMQEoxpxhs6dT0m', 'student', 'ROLE_USER'); 
 * 
 * // MySQL のTx が有効であることを確認する
 * use information_schema
 * select table_schema, table_name, engine from tables where table_name = 'contractor';
 * +--------------+------------+--------+
 * | TABLE_SCHEMA | TABLE_NAME | ENGINE |
 * +--------------+------------+--------+
 * | test         | contractor | InnoDB |
 * +--------------+------------+--------+
 * ```
 * 
 * MySQL C++ Connector について
 * ```
 * sudo apt update
 * # MySQL Connector/C++ の開発パッケージをインストールします
 * sudo apt install libmysqlcppconn-dev
 * # インストールライブラリの所在確認
 * dpkg -L libmysqlcppconn10
 * dpkg -L libmysqlcppconn-dev
 * ```
 * 
 * e.g.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror -I../inc/ -I/usr/include/mysql-cppconn/ -L/usr/lib/x86_64-linux-gnu/ builder_template_m.cpp -lmysqlcppconn -lmysqlcppconnx -o ../bin/builder_template_m
 */
#include <iostream>
#include <cassert>
#include <memory>
#include <map>
#include <vector>
// #include <codecvt> // For std::codecvt_utf8_utf16
// #include <locale>  // For std::wstring_convert
#include <Repository.hpp>
#include <sql_helper.hpp>

#include <mysql/jdbc.h>
#include <mysqlx/xdevapi.h>

template <class M, class D>
void (*ptr_print_debug)(M, D) = [](const auto message, const auto debug) -> void
{
    std::cout << "DEBUG: " << message << '\t' << debug << std::endl;
};

template <class Error>
concept ErrReasonable = requires(Error& e) {
    e.what();
};
template <class Error>
requires ErrReasonable<Error>
void (*ptr_print_error)(Error) = [](const auto e) -> void {
    std::cerr << "ERROR: " << e.what() << std::endl;
};

template <class... Args>
void print_debug_v3(Args&&... args)
{
    std::cout << "Debug v3: ";
    auto print_element = [](const auto& element) {
        std::cout << element << '\t';
    };
    // C++17以降の pack expansion で要素を順に処理
    (print_element(std::forward<Args>(args)), ...);
    std::cout << std::endl;
}

class Transaction {
public:
    virtual void begin() const
    {
        puts("tx begin");
    }
    virtual void commit() const
    {
        puts("tx commit");
    }
    virtual void rollback() const
    {
        puts("tx rollback");
    }
};


class MySqlTransaction final : public Transaction {
private:
    mysqlx::Session* const tx;
public:
    MySqlTransaction(mysqlx::Session* const _tx): tx{_tx}
    {}
    virtual void begin() const override
    {
        puts("tx begin");
        tx->startTransaction();
    }
    virtual void commit() const override
    {
        puts("tx commit");
        tx->commit();
    }
    virtual void rollback() const override
    {
        puts("tx rollback");
        tx->rollback();
    }
};

class Contractor final {
private:
    uint64_t id;
    std::string companyId;
    std::string email;
    std::string password;
    std::string name;
    std::optional<std::string> roles;
public:
    explicit Contractor():id{0}
    {}
    explicit Contractor(const uint64_t& _id, const std::string& _companyId, const std::string& _email, const std::string& _password, const std::string& _name, const std::optional<std::string>& _roles)
    : id{_id}, companyId{_companyId}, email{_email}, password{_password}, name{_name}, roles{_roles}
    {}

    uint64_t getId()           noexcept { return id; }
    std::string getCompanyId() noexcept { return companyId; }
    std::string getEmail()     noexcept { return email; }
    std::string getPassword()  noexcept { return password; }
    std::string getName()      noexcept { return name; }
    std::string getRoles()     noexcept 
    {
        if(roles) return roles.value();
        return std::string("");
    }

    void print()
    {
        puts("Contractor::print");
        std::cout << "id: " << id << '\t';
        std::cout << "companyId: " << companyId << '\t';
        std::cout << "email: " << email << '\t';
        std::cout << "password :" << password << '\t';
        std::cout << "name: " << name << '\t';
        // if(roles) {
        //     std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
        //     std::string utf8_str = converter.to_bytes(*roles);
        //     std::cout << "roles: " << utf8_str;
        // }
        if(roles) {
            std::cout << "roles: " << roles.value();
        }
        std::cout << std::endl;
    }
};

namespace tmp::mysql::r0 {

// 一見次のRepository のサブクラスは良いように思えるが、Data がContractor に
// なっていることが好ましくない。Repository を唯一のインタフェースとしたい場合
// Data はシステムで共通としたい。mysqlx であれば、mysqlx::RowResult になるのかも。
// いずれにせよ、扱いやすい共通のData でなければ、インタフェースの使い勝手が悪い。
// あるいは、Data 自体をサブクラス内に隠蔽して、インタフェースから消す必要がある。

class ContractorRepository final : public tmp::Repository<uint64_t, Contractor> {
private:
    mysqlx::Session* const session;
    const std::string DB{"test"};
    const std::string TABLE{"contractor"};
    // mysqlx::Schema db;
    // mysqlx::Table table;
public:
    ContractorRepository(mysqlx::Session* const _session) : session{_session}
    {}
    virtual uint64_t insert(Contractor&& data) const override
    {
        puts("tmp::mysql::ContractorRepository::insert");
        mysqlx::Schema db{session->getSchema(DB)};
        mysqlx::Table table{db.getTable(TABLE)};
        mysqlx::Result res = table.insert("company_id","email","password","name","roles")
                .values(data.getCompanyId(), data.getEmail(), data.getPassword(), data.getName(), data.getRoles())
                .execute();
        std::cout << "affected items count: " << res.getAffectedItemsCount() << std::endl; // 上記で Insert されたレコード件数
        return res.getAutoIncrementValue();
    }
    virtual void update(const uint64_t& id, Contractor&& data) const override
    {
        puts("tmp::mysql::ContractorRepository::update");
        mysqlx::Schema db{session->getSchema(DB)};
        mysqlx::Table table{db.getTable(TABLE)};
        std::string condition{"id="};
        condition.append(std::to_string(id));
        mysqlx::Result res = table.update()
                .set("company_id", data.getCompanyId()).set("email", data.getEmail()).set("password", data.getPassword()).set("name", data.getName()).set("roles", data.getRoles())
                .where(condition.c_str())
                .execute();
        std::cout << "affected items count: " << res.getAffectedItemsCount() << std::endl;
    }
    virtual void remove(const uint64_t& id) const override
    {
        puts("tmp::mysql::ContractorRepository::remove");
        mysqlx::Schema db{session->getSchema(DB)};
        mysqlx::Table table{db.getTable(TABLE)};
        std::string condition{"id="};
        condition.append(std::to_string(id));
        mysqlx::Result res = table.remove().where(condition.c_str()).execute();
        std::cout << "affected items count: " << res.getAffectedItemsCount() << std::endl;
    }
    virtual std::optional<Contractor> findById(const uint64_t& id) const override
    {
        puts("tmp::mysql::ContractorRepository::findById");
        mysqlx::Schema db{session->getSchema(DB)};
        mysqlx::Table table{db.getTable(TABLE)};
        std::string condition{"id="};
        condition.append(std::to_string(id));
        mysqlx::RowResult row = table.select("id","company_id","email","password","name","roles")
                                        .where(condition.c_str()).execute();
        for(mysqlx::abi2::r0::Row r: row) {
            ptr_print_debug<const char*,const char*>("row d type is ", typeid(r).name());
            std::cout << r.get(0) << '\t' << r.get(1) << '\t' << r.get(2) << '\t' << r.get(3) << '\t' << r.get(4)<< '\t' << r.get(5) << std::endl;            
            uint64_t id              = r.get(0);
            mysqlx::string companyId = r.get(1);
            mysqlx::string email     = r.get(2);
            mysqlx::string password  = r.get(3);
            mysqlx::string name      = r.get(4);
            std::optional<mysqlx::string> roles = std::nullopt;
            if( !(r.get(5).isNull()) ) {
                roles = r.get(5);
            }
            Contractor data{id, companyId, email, password, name, roles};
            return data;
        }
        return std::nullopt;
    }
};

}   // namespace tmp::mysql::r0

namespace tmp::mysql::v2 {

/**
 * PostgreSQL で行ったように、テーブル情報とリポジトリを分離したい。
 * 基本となるCRUD はひとつのリポジトリで実行できるはず。
 * 
 * MySQL X DevAPI ... SQL の構築とその実行が一体となっているため、リポジトリの処理が希薄
 * になると思う。mysqlx::Tableのインスタンス化のみになりそう。つまり、大半の処理を新たに
 * 作るクラスへ委譲することになる。
 * 
 * 以下のような、第二のリポジトリになるので却下。
 * つまり、現状のままで正しいカプセル化と、柔軟性、適度な拡張性があると判断している。
 * 複雑なSelect は、個々のリポジトリで実装すべきと考える。
 */

// template <class ID, class Data>
// class TableHandler {
// public:
//     virtual ~TableHandler() = default;
//     virtual ID insert() const = 0;
//     virtual void update(const ID& id, Data&& data) const = 0;
//     virtual void remove(const ID& id) const = 0;
//     virtual Data findById(const ID& id) const = 0;
// };

class ContractorRepository final : public tmp::Repository<uint64_t, std::map<std::string, std::string>> {
    using Data = std::map<std::string, std::string>;
private:
    mysqlx::Session* const session;
    const std::string DB{"test"};               // 1. データベース名は固有
    const std::string TABLE{"contractor"};      // 2. テーブル名は固有
    Data rowToData(mysqlx::abi2::r0::Row&& r) const
    {
        uint64_t id              = r.get(0);
        mysqlx::string companyId = r.get(1);
        mysqlx::string email     = r.get(2);
        mysqlx::string password  = r.get(3);
        mysqlx::string name      = r.get(4);
        std::optional<mysqlx::string> roles = std::nullopt;
        if( !(r.get(5).isNull()) ) {
            roles = r.get(5);
        }
        std::map<std::string, std::string> data;
        data.insert(std::make_pair("id", std::to_string(id)));
        data.insert(std::make_pair("company_id", companyId));
        data.insert(std::make_pair("email", email));
        data.insert(std::make_pair("password", password));
        data.insert(std::make_pair("name", name));
        if(roles) data.insert(std::make_pair("roles", roles.value()));
        return data;
    }
public:
    ContractorRepository(mysqlx::Session* const _session) : session{_session}
    {}
    virtual uint64_t insert(Data&& data) const override
    {
        print_debug_v3("insert ... ", typeid(*this).name());
        mysqlx::Schema db{session->getSchema(DB)};
        mysqlx::Table table{db.getTable(TABLE)};
        // いいから、roles（Null 許可フィールド） も入れろという仕様です。
        // 3. insert, values のパラメータは、テーブルに依存する。
        mysqlx::Result res = table.insert("company_id","email","password","name","roles")
                .values(data.at("company_id"), data.at("email"), data.at("password"), data.at("name"), data.at("roles"))
                .execute();
        std::cout << "affected items count: " << res.getAffectedItemsCount() << std::endl; // 上記で Insert されたレコード件数
        return res.getAutoIncrementValue();
    }
    virtual void update(const uint64_t& id, Data&& data) const override
    {
        print_debug_v3("update ... ", typeid(*this).name());
        mysqlx::Schema db{session->getSchema(DB)};
        mysqlx::Table table{db.getTable(TABLE)};
        // 4a. プライマリキの名称はテーブルに依存する。
        std::string condition{"id = :id"};
        // 5. set する数はテーブルに依存する。
        mysqlx::Result res = table.update()
                .set("company_id", data.at("company_id")).set("email", data.at("email")).set("password", data.at("password")).set("name", data.at("name")).set("roles", data.at("roles"))
                .where(condition)
                .bind("id", id)
                .execute();
        std::cout << "affected items count: " << res.getAffectedItemsCount() << std::endl;
    }
    virtual void remove(const uint64_t& id) const override
    {
        print_debug_v3("remove ... ", typeid(*this).name());
        mysqlx::Schema db{session->getSchema(DB)};
        mysqlx::Table table{db.getTable(TABLE)};
        // 4b. プライマリキの名称はテーブルに依存する。
        std::string condition{"id = :id"};
        mysqlx::Result res = table.remove()
                            .where(condition)
                            .bind("id", id).execute();
        std::cout << "affected items count: " << res.getAffectedItemsCount() << std::endl;
    }
    virtual std::optional<Data> findById(const uint64_t& id) const override
    {
        print_debug_v3("findById ... ", typeid(*this).name());
        mysqlx::Schema db{session->getSchema(DB)};
        mysqlx::Table table{db.getTable(TABLE)};
        // 4c. プライマリキの名称はテーブルに依存する。
        std::string condition{"id = :id"};
        // 6. select のパラメータはテーブルに依存する。
        mysqlx::RowResult row = table.select("id","company_id","email","password","name","roles")
                                    .where(condition)
                                    .bind("id", id).execute();
        // 7. foreach の中身すべてがテーブルに依存する（private のメンバ関数にまとめるのが先かな：）。
        for(mysqlx::abi2::r0::Row r: row) {
            ptr_print_debug<const char*,const char*>("row d type is ", typeid(r).name());
            std::cout << r.get(0) << '\t' << r.get(1) << '\t' << r.get(2) << '\t' << r.get(3) << '\t' << r.get(4)<< '\t' << r.get(5) << std::endl;
            return rowToData(std::move(r));
        }
        return std::nullopt;
    }
    //
    // ContractorRepository origin
    //
    std::vector<Data> findSample(const size_t& limit, const size_t& offset) const
    {
        mysqlx::Schema db{session->getSchema(DB)};
        mysqlx::Table table{db.getTable(TABLE)};
        mysqlx::RowResult row = table
            .select("id","company_id","email","password","name","roles")
            .groupBy("id").having("id >= 2")
            .orderBy("id DESC").limit(limit).offset(offset).execute();
        std::vector<Data> result;
        for(mysqlx::abi2::r0::Row r: row) {
            ptr_print_debug<const char*,const char*>("row d type is ", typeid(r).name());
            std::cout << r.get(0) << '\t' << r.get(1) << '\t' << r.get(2) << '\t' << r.get(3) << '\t' << r.get(4)<< '\t' << r.get(5) << std::endl;
            uint64_t id              = r.get(0);
            mysqlx::string companyId = r.get(1);
            mysqlx::string email     = r.get(2);
            mysqlx::string password  = r.get(3);
            mysqlx::string name      = r.get(4);
            std::optional<mysqlx::string> roles = std::nullopt;
            if( !(r.get(5).isNull()) ) {
                roles = r.get(5);
            }
            Data data;
            data.insert(std::make_pair("id", std::to_string(id)));
            data.insert(std::make_pair("company_id", companyId));
            data.insert(std::make_pair("email", email));
            data.insert(std::make_pair("password", password));
            data.insert(std::make_pair("name", name));
            if(roles) data.insert(std::make_pair("roles", roles.value()));
            result.push_back(data);
        }
        return result;
    }
};

/**
 * mysqlx ... X DevAPI
 * 
 * RDBMS だけではなくドキュメントベースのNoSQL ストレージにも対応している。
 * 今回はRDBMS にのみ焦点を当てている。前回NoSQL を試したが、実行速度に問題が
 * あったと記憶している。そのため、機会をみて、再度確認してみたい。
 * 
 * mysqlx::Table のSQL ビルドはよくできているが、あくまで単純なSQL のみしか
 * つくれなそうなのが不安だ。その点をX DevAPI で可能なのか調査してみる。だめ
 * なら、古き良きJDBC、C++ Connector 版を利用するしかないだろう。
 */

// Select、Where、Dto この要素が引数に必要 ？
// 戻値は std::map<std::string,std::string> ？
// Dto は個別定義されたクラスでもいいかも。


class SqlExecutor final {
    using Data = std::map<std::string, std::string>;
private:
    mysqlx::Session* const sess;
    const std::string DB{"test"};
public:
    SqlExecutor(mysqlx::Session* const _sess): sess{_sess}
    {}
    std::vector<Data> findProc_v1(std::vector<std::string>&& sqlSyntax)
    {
    // e.g.
    // mysql> PREPARE stmt FROM 'SELECT id, company_id, email FROM contractor WHERE company_id = ?';
    // mysql> SET @1 = 'A1_1000';
    // mysql> EXECUTE stmt USING @1;
        using Type = mysqlx::abi2::r0::Value::Type;
        std::vector<Data> result;
        sess->sql("USE "+DB).execute();  // データベースは流石にクラスで管理したい。
        size_t size = sqlSyntax.size();
        size_t i = 0;
        for(auto s: sqlSyntax) {
            i++;
            if(i < size) sess->sql(s).execute();
            else {
                mysqlx::RowResult rowResult = sess->sql(s).execute();
                print_debug_v3("row count: ", rowResult.count());
                print_debug_v3("column count: ", rowResult.getColumnCount());
                auto cols = rowResult.getColumnCount();
                for(mysqlx::abi2::r0::Row r: rowResult) {
                    Data data;
                    for(decltype(cols) j=0; j < cols; j++) {
                        auto colName = rowResult.getColumn(j).getColumnName();
                        std::cout << colName << ": " << r.get(j) << '\t';
                        uint64_t val_uint64=0;
                        int64_t val_int64=0;
                        float val_float=0.f;
                        double val_double=0.f;
                        mysqlx::string val_bool;
                        mysqlx::string val_string;
                        switch(r.get(j).getType()) {
                            case Type::VNULL:
                                data.insert(std::make_pair(colName, ""));
                                break;
                            case Type::UINT64:
                                val_uint64 = r.get(j);
                                data.insert(std::make_pair(colName, std::to_string(val_uint64)));
                                break;
                            case Type::INT64:
                                val_int64 =  r.get(j);
                                data.insert(std::make_pair(colName, std::to_string(val_int64)));
                                break;
                            case Type::FLOAT:
                                val_float = r.get(j);
                                data.insert(std::make_pair(colName, std::to_string(val_float)));
                                break;
                            case Type::DOUBLE:
                                val_double = r.get(j);
                                data.insert(std::make_pair(colName, std::to_string(val_double)));
                                break;
                            case Type::BOOL:
                                val_bool = r.get(j);
                                data.insert(std::make_pair(colName, val_bool));
                                break;
                            case Type::STRING:
                                val_string = r.get(j);
                                data.insert(std::make_pair(colName, val_string));
                                break;
                            default:    // DOCUMENT RAW ARRAY は無視してる。
                                break;
                        }
                    }   // end of for
                    result.push_back(data);
                    std::cout << std::endl;
                }
            }
        }
        return result;
    }
    void cudProc_v1(std::vector<std::string>&& sqlSyntax)
    {
        sess->sql("USE "+DB).execute();
        size_t size = sqlSyntax.size();
        size_t i = 0;
        for(auto s: sqlSyntax) {
            i++;
            if(i < size) sess->sql(s).execute();
            else {
                mysqlx::RowResult rowResult = sess->sql(s).execute();
                print_debug_v3("row count: ", rowResult.count());
            }
        }
    }
};

class SimplePrepare final {
private:
    std::string name;
    std::string query;
    std::vector<std::string> sets;
    void valueToSets(const std::string& val)
    {
        const std::string s{"SET @"};
        const size_t count = sets.size() + 1;
        std::string elm;
        elm.append(s).append(std::to_string(count)).append("=").append(val).append(";");
        sets.push_back(elm);
    }
public:
    SimplePrepare(const std::string _name): name{_name}
    {}
    SimplePrepare& setQuery(const std::string _query) noexcept
    {
        query = _query;
        return *this;
    }
    SimplePrepare& set(const int64_t& val)
    {
        valueToSets(std::to_string(val));
        return *this;
    }
    SimplePrepare& set(const int& val)
    {
        valueToSets(std::to_string(val));
        return *this;
    }
    SimplePrepare& set(const double& val)
    {
        valueToSets(std::to_string(val));
        return *this;
    }
    SimplePrepare& set(const std::string& val)
    {
        const std::string s{"SET @"};
        const size_t count = sets.size() + 1;
        std::string elm;
        elm.append(s).append(std::to_string(count)).append("='").append(val).append("';");
        sets.push_back(elm);
        return *this;
    }
    std::vector<std::string> build() const
    {
        std::vector<std::string> result;
        std::string prepare_syntax{"PREPARE "};
        std::string using_syntax{""};
        std::string execute_syntax{""};
        size_t i = 0;
        // PREPARE からクエリまで
        prepare_syntax.append(name).append(" FROM '\n")
        .append(query).append("';\n");
        result.push_back(prepare_syntax);
        // SET 句以降
        for(const std::string& s: sets) {
            result.push_back(std::string(s + "\n"));
            if(i == 0) using_syntax.append("@").append(std::to_string(++i));
            else using_syntax.append(", @").append(std::to_string(++i));
        }
        execute_syntax.append("EXECUTE ").append(name).append(" USING ").append(using_syntax).append(";\n");
        result.push_back(execute_syntax);
        return result;
    }
};

}   // namespace tmp::mysql::v2

int test_conn_mysqlx()
{
    puts("------ test_conn_mysqlx");
    try {
        Contractor tmp;
        tmp.print();
        uint64_t id = 3;
        mysqlx::Session sess("localhost", 33060, "root", "root1234");
        tmp::mysql::r0::ContractorRepository repo{&sess};
        std::optional<Contractor> data = repo.findById(id);
        if(data) {
            data.value().print();
        } else {
            print_debug_v3("No data ... id: ", id);
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_insert_mysqlx(uint64_t* id)
{
    puts("------ test_insert_mysqlx");
    std::clock_t start_1 = clock();
    mysqlx::Session sess("localhost", 33060, "root", "root1234");
    std::unique_ptr<Transaction> tx = std::make_unique<MySqlTransaction>(&sess);
    try {
        std::clock_t start_2 = clock();
        // sess.startTransaction();
        tx->begin();
        tmp::mysql::r0::ContractorRepository repo{&sess};
        Contractor data{0, "B3_1000", "alice@loki.org", "alice1111", "Alice", std::nullopt};
        data.print();
        *id = repo.insert(std::move(data));
// throw std::runtime_error("It's test.");
        // sess.commit();
        tx->commit();
        std::clock_t end = clock();
        std::cout << *id << std::endl;
        std::cout << "passed " << (double)(end-start_1)/CLOCKS_PER_SEC << " sec." << std::endl;
        std::cout << "passed " << (double)(end-start_2)/CLOCKS_PER_SEC << " sec." << std::endl;
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        // sess.rollback();
        tx->rollback();
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_update_mysqlx(uint64_t* id) {
    puts("------ test_update_mysqlx");
    mysqlx::Session sess("localhost", 33060, "root", "root1234");
    std::unique_ptr<Transaction> tx = std::make_unique<MySqlTransaction>(&sess);
    try {
        tx->begin();
        std::optional<std::string> roles = std::string{"Admin,User"};
        tmp::mysql::r0::ContractorRepository repo{&sess};
        Contractor data{*id, "B3_1000", "foo@loki.org", "alice1111", "Foo", roles};
        data.print();
        repo.update(*id, std::move(data));
        tx->commit();
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        tx->rollback();
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_delete_mysqlx(uint64_t* id)
{
    puts("------ test_delete_mysqlx");
    mysqlx::Session sess("localhost", 33060, "root", "root1234");
    std::unique_ptr<Transaction> tx = std::make_unique<MySqlTransaction>(&sess);
    try {
        tx->begin();
        tmp::mysql::r0::ContractorRepository repo{&sess};
        repo.remove(*id);
        tx->commit();
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        tx->rollback();
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_conn_mysqlx_v2()
{
    puts("------ test_conn_mysqlx_v2");
    try {
        uint64_t id = 3;
        mysqlx::Session sess("localhost", 33060, "root", "root1234");
        std::unique_ptr<tmp::Repository<uint64_t,std::map<std::string, std::string>>> irepo
                        = std::make_unique<tmp::mysql::v2::ContractorRepository>(&sess);
        std::optional<std::map<std::string, std::string>> data = irepo->findById(id);
        if(data) {
            print_debug_v3("Hit data ... id: ", id);
            for(auto f: data.value()) {
                std::cout << f.first << '\t' << f.second << std::endl;
            }
        } else {
            print_debug_v3("No data ... id: ", id);
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_insert_mysqlx_v2(uint64_t* id)
{
    puts("------ test_insert_mysqlx_v2");
    std::clock_t start_1 = clock();
    mysqlx::Session sess("localhost", 33060, "root", "root1234");
    std::unique_ptr<Transaction> tx = std::make_unique<MySqlTransaction>(&sess);
    try {
        std::clock_t start_2 = clock();
        // sess.startTransaction();
        tx->begin();
        std::map<std::string, std::string> data;
        data.insert(std::make_pair("company_id", "B3_1000"));
        data.insert(std::make_pair("email", "alice@loki.org"));
        data.insert(std::make_pair("password", "alice1111"));
        data.insert(std::make_pair("name", "Alice"));
        data.insert(std::make_pair("roles", ""));
        std::unique_ptr<tmp::Repository<uint64_t,std::map<std::string, std::string>>> irepo
                        = std::make_unique<tmp::mysql::v2::ContractorRepository>(&sess);
        *id = irepo->insert(std::move(data));
// throw std::runtime_error("It's test.");
        // sess.commit();
        tx->commit();
        std::clock_t end = clock();
        std::cout << "id: " << *id << std::endl;
        std::cout << "passed " << (double)(end-start_1)/CLOCKS_PER_SEC << " sec." << std::endl;
        std::cout << "passed " << (double)(end-start_2)/CLOCKS_PER_SEC << " sec." << std::endl;
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        // sess.rollback();
        tx->rollback();
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_update_mysqlx_v2(uint64_t* id) {
    puts("------ test_update_mysqlx_v2");
    mysqlx::Session sess("localhost", 33060, "root", "root1234");
    std::unique_ptr<Transaction> tx = std::make_unique<MySqlTransaction>(&sess);
    try {
        tx->begin();
        std::map<std::string, std::string> data;
        data.insert(std::make_pair("company_id", "B3_1000"));
        data.insert(std::make_pair("email", "foo@loki.org"));
        data.insert(std::make_pair("password", "alice1111"));
        data.insert(std::make_pair("name", "Foo"));
        data.insert(std::make_pair("roles", "Admin,User"));
        std::unique_ptr<tmp::Repository<uint64_t,std::map<std::string, std::string>>> irepo
                        = std::make_unique<tmp::mysql::v2::ContractorRepository>(&sess);
        irepo->update(*id, std::move(data));
        tx->commit();
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        tx->rollback();
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_delete_mysqlx_v2(uint64_t* id)
{
    puts("------ test_delete_mysqlx_v2");
    mysqlx::Session sess("localhost", 33060, "root", "root1234");
    std::unique_ptr<Transaction> tx = std::make_unique<MySqlTransaction>(&sess);
    try {
        tx->begin();
        std::unique_ptr<tmp::Repository<uint64_t,std::map<std::string, std::string>>> irepo
                        = std::make_unique<tmp::mysql::v2::ContractorRepository>(&sess);
        irepo->remove(*id);
        tx->commit();
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        tx->rollback();
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_findOrderByIdDescLimitOffset_mysqlx_v2(uint64_t* id)
{
    puts("------ test_findOrderByIdDescLimitOffset_mysqlx_v2");
    using Data = std::map<std::string, std::string>;
    using iface = tmp::Repository<uint64_t, Data>;
    using subclazz = tmp::mysql::v2::ContractorRepository;
    mysqlx::Session sess("localhost", 33060, "root", "root1234");
    std::unique_ptr<Transaction> tx = std::make_unique<MySqlTransaction>(&sess);
    try {
        tx->begin();
        std::unique_ptr<iface> repo = std::make_unique<subclazz>(&sess);
        tmp::mysql::v2::ContractorRepository* crepo = dynamic_cast<tmp::mysql::v2::ContractorRepository*>(repo.get());
        std::vector<Data> result = crepo->findSample(10, 0);
        tx->commit();
        if(result.size() == 0) std::cout << "No record." << std::endl;
        for(Data& data: result) {
            for(auto f: data) {
                std::cout << f.first << '\t' << f.second << '\t';
            }
            std::cout << std::endl;
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        tx->rollback();
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_conn_mysqlx_v3()
{
    puts("------ test_conn_mysqlx_v3");
    try {
        // Connect to server on localhost
        mysqlx::Session mySession("localhost", 33060, "root", "root1234");
        // Switch to use schema 'test'
        mySession.sql("USE test").execute();
        // In a Session context the full SQL language can be used
        mySession.sql("CREATE PROCEDURE my_add_one_procedure "
                    " (INOUT incr_param INT) "
                    "BEGIN "
                    "  SET incr_param = incr_param + 1;"
                    "END;")
                .execute();
        mySession.sql("SET @my_var = ?;").bind(10).execute();
        mySession.sql("CALL my_add_one_procedure(@my_var);").execute();
        mySession.sql("DROP PROCEDURE my_add_one_procedure;").execute();
        // Use an SQL query to get the result... mysqlx::Result
        auto myResult = mySession.sql("SELECT @my_var").execute();
        // Gets the row and prints the first column
        // auto col = myResult.getColumn(0).getColumnName();
        // std::cout << col << std::endl;
        mysqlx::abi2::r0::Row row = myResult.fetchOne();
        std::cout << row[0] << std::endl;
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_SqlExecutor_findProc_v1()
{
    puts("------ test_SqlExecutor_findProc_v1");
    using Data = std::map<std::string,std::string>;
    try {
        mysqlx::Session sess("localhost", 33060, "root", "root1234");
        tmp::mysql::v2::SqlExecutor exec{&sess};
        std::string s1 = R"(PREPARE stmt FROM '
        SELECT id, company_id, email
        FROM contractor
        WHERE company_id = ?
        ';)";
        std::string s2 = R"(SET @1 = 'A1_1000';)";
        std::string s3 = R"(EXECUTE stmt USING @1;)";
        std::vector<std::string> vec{s1, s2, s3};
        std::vector<Data> result = exec.findProc_v1(std::move(vec));
        for(auto& row: result) {
            puts("--------- check data");
            for(auto& m: row) {
                print_debug_v3(m.first, m.second);
            }
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_output_prep_sql()
{
    puts("------ test_output_prep_sql");
    try {
        std::string sql_i = tmp::mysql::helper::insert_sql("contractor", "company_id", "email", "password", "name");
        print_debug_v3("insert sql: ", sql_i);
// PREPARE stmt FROM '
// INSERT INTO contractor
// ( company_id, email, password, name )
// VALUES
// ( ?, ?, ?, ? )';
// SET @1='C3_3000';
// SET @2='jack@loki.org';
// SET @3='jack1111';
// SET @4='Jack';
// EXECUTE stmt USING @1, @2, @3, @4;
        std::string sql_u = tmp::mysql::helper::update_by_pkey_sql("contractor", "id", "company_id", "email", "password", "name");
        print_debug_v3("update sql: ", sql_u);
// PREPARE stmt FROM '
// UPDATE contractor
// SET company_id=?, email=?, password=?, name=?
// WHERE id=?';
// SET @1='C3_3333';
// SET @2='jack@loki.org';
// SET @3='jack2222';
// SET @4='JACK';
// SET @5=180;
// EXECUTE stmt USING @1, @2, @3, @4, @5;
        std::string sql_s = tmp::mysql::helper::select_by_pkey_sql("contractor", "id");
        print_debug_v3("select sql: ", sql_s);
// PREPARE stmt FROM '
// SELECT * FROM contractor
// WHERE id=?';
// SET @1=180;
// EXECUTE stmt USING @1;
        std::string sql_d = tmp::mysql::helper::delete_by_pkey_sql("contractor", "id");
        print_debug_v3("delete sql: ", sql_d);
// PREPARE stmt FROM '
// DELETE FROM contractor
// WHERE id=?';
// SET @1=180;
// EXECUTE stmt USING @1;
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_SimplePrepare_SqlExecutor_M1()
{
    puts("------ test_SimplePrepare_SqlExecutor_M1");
    using Data = std::map<std::string,std::string>;
    mysqlx::Session sess("localhost", 33060, "root", "root1234");
    try {
        // Insert
        tmp::mysql::v2::SimplePrepare insPrepare{"stmt1"};
        insPrepare.setQuery(tmp::mysql::helper::insert_sql("contractor", "company_id", "email", "password", "name"));
        std::vector<std::string> syntax = insPrepare.set("C3_3000").set("derek@loki.org").set("derek1111").set("DEREK").build();
        for(auto s: syntax) {
            print_debug_v3(s);
        }
        sess.startTransaction();
        tmp::mysql::v2::SqlExecutor exec{&sess};
        exec.cudProc_v1(std::move(syntax));
        sess.commit();
        // Insert 後の確認、ID の取得が目的。
        std::string s1 = R"(SELECT * FROM contractor ORDER BY id DESC LIMIT 1;)";
        std::vector<std::string> vec{s1};
        std::vector<Data> r1 = exec.findProc_v1(std::move(vec));
        for(auto& row: r1) {
            puts("--------- check data");
            for(auto& m: row) {
                print_debug_v3(m.first, m.second);
            }
        }
        // Update
        tmp::mysql::v2::SimplePrepare upPrepare{"stmt2"};
        upPrepare.setQuery(tmp::mysql::helper::update_by_pkey_sql("contractor", "id", "company_id", "email", "password", "name"));
        std::vector<std::string> syntax_u = upPrepare.set("C3_3333").set("jack@loki.org").set("jack2222").set("JACK").set(std::stoi(r1[0].at("id"))).build();
        for(auto s: syntax_u) {
            print_debug_v3(s);
        }
        sess.startTransaction();
        exec.cudProc_v1(std::move(syntax_u));
        sess.commit();
        // Delete
        tmp::mysql::v2::SimplePrepare delPrepare{"stmt3"};
        delPrepare.setQuery(tmp::mysql::helper::delete_by_pkey_sql("contractor", "id"));
        std::vector<std::string> syntax_d = delPrepare.set(std::stoi(r1[0].at("id"))).build();
        for(auto s: syntax_d) {
            print_debug_v3(s);
        }
        sess.startTransaction();
        exec.cudProc_v1(std::move(syntax_d));
        sess.commit();
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        sess.rollback();
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int main(void)
{
    puts("START main ===");
    int ret = -1;
    if(0) {
        print_debug_v3("Play and Result ... ", ret = test_conn_mysqlx());
        assert(ret == 0);
    }
    if(1) {
        print_debug_v3("Play and Result ... ", ret = test_conn_mysqlx_v2());
        assert(ret == 0);
    }
    uint64_t id = 0;
    if(0) {
        print_debug_v3("Play and Result ... ", ret = test_insert_mysqlx(&id));
        print_debug_v3("id: ", id);
        assert(ret == 0);
        print_debug_v3("Play and Result ... ", ret = test_update_mysqlx(&id));
        assert(ret == 0);
        print_debug_v3("Play and Result ... ", ret = test_delete_mysqlx(&id));
        assert(ret == 0);
    }
    if(1) {
        print_debug_v3("Play and Result ... ", ret = test_insert_mysqlx_v2(&id));
        print_debug_v3("id: ", id);
        assert(ret == 0);
        print_debug_v3("Play and Result ... ", ret = test_update_mysqlx_v2(&id));
        assert(ret == 0);
        print_debug_v3("Play and Result ... ", ret = test_delete_mysqlx_v2(&id));
        assert(ret == 0);
        print_debug_v3("Play and Result ... ", ret = test_findOrderByIdDescLimitOffset_mysqlx_v2(&id));
        assert(ret == 0);
        print_debug_v3("Play and Result ... ", ret = test_SqlExecutor_findProc_v1());
        assert(ret == 0);
    }
    if(0) {
        print_debug_v3("Play and Result ... ", ret = test_conn_mysqlx_v3());
        assert(ret == 0);
    }
    if(0) {
        print_debug_v3("Play and Result ... ", ret = test_output_prep_sql());
        assert(ret == 0);
    }
    if(0) {
        print_debug_v3("Play and Result ... ", ret = test_SimplePrepare_SqlExecutor_M1());
        assert(ret == 0);
    }
    puts("=== main END");
    return EXIT_SUCCESS;
}