/**
 * Builder と　Template method 再び
 * 
 * 具体的なものを作るとイメージから離れてしまうため、今回はイメージ、
 * 理想を優先してみる。
 * 
 * libpqxx のビルド
 * ```
    sudo apt install libpq-dev 

    git clone -b master --recursive https://github.com/jtv/libpqxx.git
    cd ./libpqxx
    ./configure --disable-shared --with-postgres-include
    make
    sudo make install
    pg_config --libdir
    pkg-config --cflags --libs libpqxx

    -I/usr/local/include -I/usr/include/postgresql -L/usr/local/lib -lpqxx
 * ```
 * ```
DROP TABLE IF EXISTS contractor;
CREATE TABLE contractor (
    id              SERIAL          PRIMARY KEY,
    company_id      VARCHAR(100)    NOT NULL,
    email           VARCHAR(100)    NOT NULL,
    password        VARCHAR(255)    NOT NULL,
    name            VARCHAR(100)    NOT NULL,
    roles           VARCHAR(100)
);
ALTER TABLE contractor ADD CONSTRAINT email_uk unique (email);
 * ```
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror -I../inc -I/usr/local/include -I/usr/include/postgresql -L/usr/local/lib  builder_template.cpp -lpqxx -lpq -o ../bin/builder_template
 */
#include <iostream>
#include <memory>
#include <cassert>
#include <map>
#include <vector>
#include <ObjectPool.hpp>
#include <Repository.hpp>
#include <pqxx/pqxx>

template <class M, class D>
void (*ptr_print_debug)(M, D) = [](const auto message, const auto debug) -> void
{
    std::cout << "DEBUG: " << message << debug << std::endl;
};

// 嘘っぱち AI よりよっぽどいいよね（https://cpprefjp.github.io/lang/cpp11/variadic_templates.html）
// うん、これはいい：）

// パラメータパックが空になったら終了
void print_debug() {};

template <class Head, class... Tail>
void print_debug(Head&& head, Tail&&... tail)
{
  std::cout << head << std::endl;

  // パラメータパックtailをさらにheadとtailに分割する
  print_debug(std::forward<Tail>(tail)...);
}

// AI 作成バージョン .. これは動く。
template <class... Args>
void print_debug_v2(Args&&... args)
{
    // std::apply を使用して、タプルに変換された引数を別の関数に渡す
    std::apply([](auto&&... t_args) {
        std::cout << "Debug Tuple elements: ";
        auto print_element = [&](const auto& element) {
            std::cout << element << '\t';
        };
        // C++17以降の pack expansion で要素を順に処理
        (print_element(t_args), ...);
        std::cout << std::endl;
    }, std::make_tuple(std::forward<Args>(args)...));
}

//
// 学習を進めて、次の形が一番使い勝手がいいのではと思った。
//

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

template <class Error>
concept ErrReasonable = requires(Error& e) {
    e.what();
};
template <class Error>
requires ErrReasonable<Error>
void (*ptr_print_error)(Error) = [](const auto e) -> void {
    std::cerr << "ERROR: " << e.what() << std::endl;
};

// ある処理の終了が、次の処理の始まり。
// 単なるチェーン処理だな。
// それが嫌なら、自身を返却して、ひとつにまとめる（普通の Builder パターン）。

template <class T>
class Builder {
public:
    virtual ~Builder() = default;
    virtual T build() const = 0;
};

class ProcC final {
private:
    const int num;
    const std::string str;
public:
    ProcC(const int& _num, const std::string& _str): num{_num}, str{_str}
    {}
    void finish() const {
        int finalNum = num + 1000;
        std::string finalStr = str + "CCC";
        printf("finalNum: %d\n", finalNum);
        printf("finalStr: %s\n", finalStr.c_str());
    }
};

class ProcB final : public Builder<ProcC>
{
private:
    const int num;
    const std::string str;
public:
    ProcB(const int& _num, const std::string& _str): num{_num}, str{_str} 
    {}
    virtual ProcC build() const
    {
        return ProcC(num + 100, str + "BBB");
    }
};

class ProcA final : public Builder<ProcB>
{
private:
    const int num;
    const std::string str;
public:
    ProcA(const int& _num, const std::string& _str): num{_num}, str{_str}
    {}
    virtual ProcB build() const
    {
        // 処理結果を次の処理に引き渡す（データが重複する分無駄も生じる）。
        return ProcB(num * 2, str+"AAA");
    }
};

class Proceed final {
private:
    std::unique_ptr<std::string> str = nullptr;
    std::unique_ptr<int> num         = nullptr;
public:
    Proceed& makeProc1()
    {
        if(!str) str = std::make_unique<std::string>("proc1");
        return *this;
    }
    Proceed& makeProc2()
    {
        if(!num) num = std::make_unique<int>(2);
        return *this;
    }
    void debug()
    {
        if( str ) ptr_print_debug<const std::string&, std::string&>("str: ", *(str.get()));
        if( num ) ptr_print_debug<const std::string&, int&>("num: ", *(num.get()));
    }
};

template <class... Args>
std::string insert_sql(const std::string& table, Args&&... fields)
{
    // R"(INSERT INTO contractor (id, company_id, email, password, name, roles) VALUES ($1, $2, $3, $4, $5, $6))"
    size_t size = sizeof...(fields);
    printf("size: %zu\n", size);
    std::string sql = "INSERT INTO " + table + '\n';
    const std::string dollar = "$";
    std::string flds;
    std::string values;
    size_t i = 0;

    auto print_fields_values = [&](const auto& element) {
        i++;
        if(i < size) {
            flds.append(element).append(", ");
            values.append(dollar + std::to_string(i)).append(", ");
        } else {
            flds.append(element);
            values.append(dollar + std::to_string(i));
        }
    };
    (print_fields_values(std::forward<Args>(fields)), ...);

    sql.append("( ");
    sql.append(flds);
    sql.append(" )");
    sql.append("\nVALUES\n");
    sql.append("( ");
    sql.append(values);
    sql.append(" )\n");
    return sql;
}

template <class... Args>
std::string update_sql(const std::string& table, Args&&... fields)
{
    // UPDATE contractor
    // SET id=$1, company_id=$2, email=$3, password=$4, name=$5, roles=$6 
    // WHERE id=$1

    size_t size = sizeof...(fields);
    std::string sql = "UPDATE " + table + '\n';
    const std::string dollar = "$";
    std::string fvals;
    std::string pkey;
    size_t i = 0;

    auto print_fields_values = [&](const auto& element) {
        i++;
        if(i < size) {
            if(i == 1) pkey.append(element).append("=").append(dollar+std::to_string(1));
            fvals.append(element).append("=").append(dollar+std::to_string(i)).append(", ");
        } else {
            fvals.append(element).append("=").append(dollar+std::to_string(i));
        }
    };
    (print_fields_values(std::forward<Args>(fields)), ...);
    sql.append("SET ").append(fvals);
    sql.append("\nWHERE ").append(pkey).append("\n");
    return sql;
}

template <class... Args>
std::string select_by_pkey_sql(const std::string& table, const std::string& pkey, Args&&... fields)
{
    size_t size = sizeof...(fields);
    std::string sql = "SELECT ";
    std::string flds;
    size_t i = 0;

    auto print_field = [&](const auto& element) {
        i++;
        if(i < size) {
            flds.append(element).append(", ");
        } else {
            flds.append(element);
        }
    };
    (print_field(std::forward<Args>(fields)), ...);

    sql.append(flds);
    sql.append(" FROM ").append(table);
    sql.append("\nWHERE ").append(pkey).append("=$1");
    return sql;
}

std::string select_by_pkey_sql(const std::string& table, const std::string& pkey)
{
    std::string sql = "SELECT * FROM " + table + '\n';
    sql.append("WHERE ").append(pkey).append("=$1");
    return sql;
}

std::string delete_by_pkey_sql(const std::string& table, const std::string& pkey)
{
//     DELETE FROM contractor
//     WHERE id = $1
    std::string sql = "DELETE FROM " + table + '\n';
    sql.append("WHERE ").append(pkey).append("=$1");
    return sql;
}

/**
 * Lombok がないとこんなにも面倒に感じるものなのか。
 * 実際には、フィールド数が 50 を超える場合もあるため、この手のデータクラスの
 * 意味自体を考え直す必要があるかもしれない（作らない or 作るなら自動生成の仕組みを作る）。
 * 仕組みの中に問題がズレただけに感じるけど。
 */

class Contractor final {
private:
    int id;
    std::string companyId;
    std::string email;
    std::string password;
    std::string name;
    const char* roles;
public:
    Contractor(const int& _id, const std::string& _companyId, const std::string& _email, const std::string& _password, const std::string& _name, const char* _roles)
    : id{_id}, companyId{_companyId}, email{_email}, password{_password}, name{_name}, roles{_roles}
    {}

    int getId()                noexcept { return id; }
    std::string getCompanyId() noexcept { return companyId; }
    std::string getEmail()     noexcept { return email; }
    std::string getPassword()  noexcept { return password; }
    std::string getName()      noexcept { return name; }
    const char* getRoles()     noexcept { return roles; }

    void print()
    {
        puts("------ Contractor::print");
        std::cout << "id: " << id << '\t';
        std::cout << "companyId: " << companyId << '\t';
        std::cout << "email: " << email << '\t';
        std::cout << "password :" << password << '\t';
        std::cout << "name: " << name << '\t';
        if(roles) std::cout << "roles: " << roles << std::endl;
        else std::cout << std::endl;
    }
};

/**
 * pqxx のSQL Builder について考えてみる。
 * 
 * - SQL 単位でクラスを用意して、必要なデータは内部で定数とするか（クラスが複数必要）。
 * - ひとつのクラスで引数として必要なデータはその都度設定するか（クラスは理論上ひとつでよい）。
 * 
 * 前者は間違いが起こりずらい、問題が内部に向く。処理の単一原則にも一致する。
 * 後者はひとつのクラスで済ませることができるが、設計が煩雑にならないとも限らない。
 * 無駄なものを追加しないことが必要ということ。
 * 最初は、後者で進めてみる。
 */

class SqlBuilder final {
private:
    // 内部で保持するデータを不変にすることで、SQL 単位の処理ということを担保している。
    // 別なSQL を利用したい場合は、本オブジェクトを作り直せということ。
    const std::string sqlName;
    const std::string sql;
public:
    SqlBuilder(const std::string& _sqlName, const std::string& _sql):
     sqlName{_sqlName}, sql{_sql}
    {}
    pqxx::connection& makePrepare(pqxx::connection& conn) const
    {
        conn.prepare(
            sqlName,
            sql
        );
        return conn;
    }
    pqxx::prepped makePrepped() const
    {
        return pqxx::prepped{sqlName};
    }
    template <class... Args>
    pqxx::params makeParams(Args&&... args) const
    {
        return pqxx::params{args...};
    }
    std::string makeNextvalSql(const std::string& seqName) const
    {
        std::string sql{"SELECT nextval('"};
        sql.append(seqName).append("')");
        return sql;
    }
};

namespace tmp::postgres {

/**
 * どこまでできるか不明だが、次は具象化クラスをひとつにできないか考えてみる。
 * 以前作ったようなField クラスを定義し、それらをメンバ変数とするTable クラス
 * を作るというのはやめたい。面倒がスライドしただけだ。
 * 
 * 事前確認事項。
 * - NULL を許可したフィールドの扱い。
 * 
 * 制限事項
 * - Auto Increment によるPraimary Key の作成を必須とする。
 * - すでに、Null として登録されているレコードの検索はエラーとなる。
 * - 本API を用いた登録では、NULL 許可フィールドに対しては何らかの値の入力を必須とする。
 * 
 * つまり制限事項を取っ払い、細やかな処理がやりたければ、ContractorRepositoryのように
 * 個々に実装すればよいということ：）
 * */



// 次の処理が、結局はTemplate として、具象化クラス固有のものになる。
// DTO は作らざるを得ない。
// CRUD の各SQL はラップして作る必要がある。

template <class ID, class Data>
class TableInfo {
public:
    virtual ~TableInfo() = default;
    virtual std::string getTableName() const = 0;
    virtual std::string getSeqName() const = 0;
    virtual pqxx::params makeParams4Save(SqlBuilder& builder, const ID& id, Data&& data) const = 0;
    virtual std::string insertSql() const = 0;
    virtual std::string updateSql() const = 0;
    virtual std::string deleteSql() const = 0;
    virtual std::string selectByIdSql() const = 0;
    // dto method の中である程度の自由があるはず。
    virtual Data rowToData(const pqxx::row& row) const = 0;
};

class ContractorInfo final : public TableInfo<uint64_t, std::map<std::string, std::string>> {
    using Data = std::map<std::string, std::string>;
private:
    const std::string tableName{"contractor"};
    const std::string pkeySeqName{"contractor_id_seq"};
    const std::vector<std::string> fieldNames{"id", "company_id", "email", "password", "name", "roles"};
public:
    virtual std::string getTableName() const override
    {
        return tableName;
    }
    virtual std::string getSeqName() const override
    {
        return pkeySeqName;
    }
    virtual pqxx::params makeParams4Save(SqlBuilder& builder, const uint64_t& id, Data&& data) const override
    {
        return builder.makeParams(id
            , data.at(fieldNames[1])
            , data.at(fieldNames[2])
            , data.at(fieldNames[3])
            , data.at(fieldNames[4])
            , data.at(fieldNames[5])
        );
    }
    virtual std::string insertSql() const override
    {
        return insert_sql(tableName
            , fieldNames[0]
            , fieldNames[1]
            , fieldNames[2]
            , fieldNames[3]
            , fieldNames[4]
            , fieldNames[5]);
    }
    virtual std::string updateSql() const override
    {
        return update_sql(tableName
            , fieldNames[0]
            , fieldNames[1]
            , fieldNames[2]
            , fieldNames[3]
            , fieldNames[4]
            , fieldNames[5]);
    }
    virtual std::string deleteSql() const override
    {
        return delete_by_pkey_sql(tableName, fieldNames[0]);
    }
    virtual std::string selectByIdSql() const override
    {
        return select_by_pkey_sql(tableName, fieldNames[0]);
    }
    virtual Data rowToData(const pqxx::row& row) const override
    {
        Data data;
        auto [id, companyId, email, password, name, roles] = row.as<uint64_t, std::string, std::string, std::string, std::string, const char*>();
        data.insert(std::make_pair(fieldNames[0], std::to_string(id)));
        data.insert(std::make_pair(fieldNames[1], companyId));
        data.insert(std::make_pair(fieldNames[2], email));
        data.insert(std::make_pair(fieldNames[3], password));
        data.insert(std::make_pair(fieldNames[4], name));
        if(roles) {
            std::cout << id << '\t' << companyId << '\t' << email << '\t' << password << '\t' << name << '\t' << roles << std::endl;
            data.insert(std::make_pair(fieldNames[5], std::string(roles)));
        } else {
            std::cout << id << '\t' << companyId << '\t' << email << '\t' << password << '\t' << name << std::endl;
            data.insert(std::make_pair(fieldNames[5], ""));
        }
        return data;
    }
};

class PqxxRepository final : public tmp::Repository<uint64_t, std::map<std::string, std::string>> {
    using Data = std::map<std::string, std::string>;
private:
    pqxx::work* const tx;
    TableInfo<uint64_t, Data>* const tableInfo;
public:
    PqxxRepository(pqxx::work* const _tx, TableInfo<uint64_t, Data>* const _tableInfo): tx{_tx}, tableInfo{_tableInfo}
    {}
    virtual uint64_t insert(Data&& data) const override
    {
        print_debug_v3("insert ... ", typeid(*this).name());
        std::string sql = tableInfo->insertSql();
        ptr_print_debug<const std::string&, std::string&>("sql: \n", sql);
        SqlBuilder builder{"insert", sql};
        uint64_t id = tx->query_value<uint64_t>(
            builder.makeNextvalSql(tableInfo->getSeqName())
        );
        printf("nextval: %lu\n", id);
        builder.makePrepare(tx->conn());
        tx->exec(builder.makePrepped(), tableInfo->makeParams4Save(builder, id, std::move(data)));
        return id;
    }
    virtual void update(const uint64_t& id, Data&& data) const override
    {
        print_debug_v3("update ... ", typeid(*this).name());
        std::string sql = tableInfo->updateSql();
        ptr_print_debug<const std::string&, std::string&>("sql: \n", sql);
        SqlBuilder builder{"update", sql};
        builder.makePrepare(tx->conn());
        tx->exec(builder.makePrepped(), tableInfo->makeParams4Save(builder, id, std::move(data)));
    }
    virtual void remove(const uint64_t& id) const override
    {
        print_debug_v3("remove ... ", typeid(*this).name());
        // std::string sql = delete_by_pkey_sql("contractor", "id");
        std::string sql = tableInfo->deleteSql();
        ptr_print_debug<const std::string&, std::string&>("sql: \n", sql);
        SqlBuilder builder{"delete", sql};
        builder.makePrepare(tx->conn());
        tx->exec(builder.makePrepped(), builder.makeParams(id));
    }
    virtual std::optional<Data> findById(const uint64_t& id) const override
    {
        print_debug_v3("findById ... ", typeid(*this).name());
        // std::string sql = select_by_pkey_sql("contractor", "id");
        std::string sql = tableInfo->selectByIdSql();
        ptr_print_debug<const std::string&, std::string&>("sql: \n", sql);
        SqlBuilder builder{"select_by_id", sql};
        builder.makePrepare(tx->conn());
        pqxx::result result = tx->exec(builder.makePrepped(), builder.makeParams(id));
        for (auto const &row: result) {
            for (const pqxx::row& row: result) {
                return tableInfo->rowToData(row);
            }
        }
        return std::nullopt;
    }
};

class ContractorRepository final : public tmp::Repository<uint64_t, std::map<std::string, std::string>> {
private:
    pqxx::work* const tx;
public:
    ContractorRepository(pqxx::work* const _tx): tx{_tx}
    {}
    virtual uint64_t insert(std::map<std::string, std::string>&& data) const override
    {
        print_debug_v3("insert ... ", typeid(*this).name());
        uint64_t id = tx->query_value<uint64_t>(
            "SELECT nextval('contractor_id_seq')"
        );
        printf("nextval: %lu\n", id);
        std::string sql = insert_sql("contractor", "id", "company_id", "email", "password", "name", "roles");
        ptr_print_debug<const std::string&, std::string&>("sql: \n", sql);
        SqlBuilder builder{"insert_contractor", sql};
        builder.makePrepare(tx->conn());
        tx->exec(builder.makePrepped(), builder.makeParams(id, data.at("company_id"), data.at("email"), data.at("password"), data.at("name"), data.at("roles")));
        return id;
    }
    virtual void update(const uint64_t& id, std::map<std::string, std::string>&& data) const override
    {
        print_debug_v3("update ... ", typeid(*this).name());
        std::string sql = update_sql("contractor", "id", "company_id", "email", "password", "name", "roles");
        ptr_print_debug<const std::string&, std::string&>("sql: \n", sql);
        SqlBuilder builder{"update_contractor", sql};
        builder.makePrepare(tx->conn());
        tx->exec(builder.makePrepped(), builder.makeParams(id, data.at("company_id"), data.at("email"), data.at("password"), data.at("name"), data.at("roles")));
    }
    virtual void remove(const uint64_t& id) const override
    {
        print_debug_v3("remove ... ", typeid(*this).name());
        std::string sql = delete_by_pkey_sql("contractor", "id");
        ptr_print_debug<const std::string&, std::string&>("sql: \n", sql);
        SqlBuilder builder{"delete_contractor", sql};
        builder.makePrepare(tx->conn());
        tx->exec(builder.makePrepped(), builder.makeParams(id));
    }
    virtual std::optional<std::map<std::string, std::string>> findById(const uint64_t& id) const override
    {
        print_debug_v3("findById ... ", typeid(*this).name());
        std::string sql = select_by_pkey_sql("contractor", "id");
        ptr_print_debug<const std::string&, std::string&>("sql: \n", sql);
        SqlBuilder builder{"select_contractor", sql};
        builder.makePrepare(tx->conn());
        pqxx::result result = tx->exec(builder.makePrepped(), builder.makeParams(id));
        std::map<std::string, std::string> data;
        for (auto const &row: result) {
            for (auto const &row: result) {
                // auto [id, companyId, email, password, name, roles] = row.as<uint64_t, std::string, std::string, std::string, std::string, const char*>();    // テーブルで、null を許可している場合は const char* を使わざるを得ない。
                auto [id, companyId, email, password, name, roles] = row.as<uint64_t, std::string, std::string, std::string, std::string, const char*>();
                data.insert(std::make_pair("id", std::to_string(id)));
                data.insert(std::make_pair("company_id", companyId));
                data.insert(std::make_pair("email", email));
                data.insert(std::make_pair("password", password));
                data.insert(std::make_pair("name", name));
                if(roles) {
                    std::cout << id << '\t' << companyId << '\t' << email << '\t' << password << '\t' << name << '\t' << roles << std::endl;
                    data.insert(std::make_pair("roles", std::string(roles)));
                } else {
                    std::cout << id << '\t' << companyId << '\t' << email << '\t' << password << '\t' << name << std::endl;
                    data.insert(std::make_pair("roles", ""));
                }
                return data;
            }
        }
        return std::nullopt;
    }
};

class RepositoryAdapter final : public tmp::Repository<std::string, std::map<std::string, std::string>> {
    using interface = tmp::Repository<uint64_t, std::map<std::string, std::string>>;
private:
    interface* const adaptee;
public:
    RepositoryAdapter(interface* const _adaptee): adaptee{_adaptee}
    {}
    virtual std::string insert(std::map<std::string, std::string>&& data) const override
    {
        uint64_t id = adaptee->insert(std::forward<std::map<std::string, std::string>>(data));
        return std::to_string(id);
    }
    virtual void update(const std::string& id, std::map<std::string, std::string>&& data) const override
    {
        adaptee->update(std::stoul(id), std::forward<std::map<std::string, std::string>>(data));
    }
    virtual void remove(const std::string& id) const override
    {
        adaptee->remove(std::stoul(id));
    }
    virtual std::optional<std::map<std::string, std::string>> findById(const std::string& id) const override
    {
        return adaptee->findById(std::stoul(id));
    }
};

}   // namespace tmp::postgres






int setupTable()
{
    puts("------ setupTable");
    try {
        pqxx::connection conn{"hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234"};
        pqxx::work tx(conn);
        std::string dropTableSql = R"(DROP TABLE IF EXISTS contractor)";
        ptr_print_debug<const std::string&, std::string&>("sql: ", dropTableSql);
        tx.exec(dropTableSql);
        std::string createTableSql = R"(
            CREATE TABLE contractor (
            id              SERIAL          PRIMARY KEY,
            company_id      VARCHAR(100)    NOT NULL,
            email           VARCHAR(100)    NOT NULL,
            password        VARCHAR(255)    NOT NULL,
            name            VARCHAR(100)    NOT NULL,
            roles           VARCHAR(100)
            )
        )";
        ptr_print_debug<const std::string&, std::string&>("sql: ", createTableSql);
        tx.exec(createTableSql);
        std::string uniqueKeySql = R"(
            ALTER TABLE contractor ADD CONSTRAINT email_uk unique (email)
        )";
        ptr_print_debug<const std::string&, std::string&>("sql: ", uniqueKeySql);
        tx.exec(uniqueKeySql);
        tx.commit();
        conn.close();
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_print_debug()
{
    puts("------ test_print_debug");
    try {
        print_debug<const char*, const int&, const int&, const double&>("test", 1, 2, 3.141592);
        print_debug_v2<const char*, const int&, const int&, const double&>("test", 1, 2, 3.141592);
        // 型指定とかいらなかったんだ。
        print_debug_v3("test", 1, 2, 3.141592);
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_ObjectPool()
{
    puts("------ test_ObjectPool");
    try {
        ObjectPool<pqxx::connection> pool("pqxx::connection");
        std::unique_ptr<pqxx::connection> conn1 = std::make_unique<pqxx::connection>("hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234");
        std::unique_ptr<pqxx::connection> conn2 = std::make_unique<pqxx::connection>("hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234");
        std::unique_ptr<pqxx::connection> conn3 = std::make_unique<pqxx::connection>("hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234");
        pool.push(move(conn1));
        pool.push(move(conn2));
        pool.push(move(conn3));

        while(!pool.empty()) {
           auto conn = pool.pop();
           ptr_print_debug<const std::string&, decltype(conn)&>("conn: ", conn);
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_postgres_connection()
{
    puts("------ test_postgres_connection");
    try {
        ObjectPool<pqxx::connection> pool{"pqxx::connection"};
        std::unique_ptr<pqxx::connection> conn1 = std::make_unique<pqxx::connection>("hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234");
        pool.push(move(conn1));

        // pqxx::connection conn{"hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234"};
        std::unique_ptr<pqxx::connection> conn = pool.pop();
        pqxx::work tx(*(conn.get()));
        pqxx::result result = tx.exec("SELECT 1, 2, 'Hello', 3");
        tx.commit();
        pool.push(move(conn));

        std::cout << result.column_name(0) << std::endl;
        puts("usage 1 ------");
        for (auto const &row: result) {
            for (auto const &field: row) std::cout << field.c_str() << '\t';
            std::cout << '\n';
        }
        puts("usage 2 ------");
        for (auto const &row: result) {
            auto [one, two, hello, three] = row.as<int, int, std::string, int>();
            std::cout << one << '\t' << two << '\t' << hello << '\t' << three << std::endl;
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_postgres_insert(long* id)
{
    puts("------ test_postgres_insert");
    try {
        pqxx::connection conn{"hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234"};
        // DONE SQL の発行を別で管理したい。
        // conn.prepare(
        //     "insert_contractor",
        //     R"(INSERT INTO contractor (id, company_id, email, password, name, roles) VALUES ($1, $2, $3, $4, $5, $6))"
        // );
        // std::string sql = R"(
        //     INSERT INTO contractor (id, company_id, email, password, name, roles) 
        //     VALUES ($1, $2, $3, $4, $5, $6)
        // )";
        std::string sql = insert_sql("contractor", "id", "company_id", "email", "password", "name", "roles");
        ptr_print_debug<const std::string&, std::string&>("sql: \n", sql);
        SqlBuilder builder{"insert_contractor", sql};
        pqxx::work tx(builder.makePrepare(conn));
        *id = tx.query_value<long>(
            "SELECT nextval('contractor_id_seq')"
        );
        printf("nextId: %ld\n", *id);
        // DONE pqxx::prepped と pqxx::params のインスタンス化を SQLの発行場所で管理できないか。
        pqxx::result result = tx.exec(builder.makePrepped(), builder.makeParams(*id, "bar_3333D", "joe1234@bar.com", "joe12345678", "Joe", nullptr));
        for (auto const &row: result) {
            for (auto const &field: row) std::cout << field.c_str() << '\t';
            std::cout << '\n';
        }
// throw std::runtime_error("It's test runtime error.");
        tx.commit();
        conn.close();
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_postgres_insert_v2(uint64_t* id)
{
    puts("------ test_postgres_insert_v2");
    try {
        std::clock_t start_1 = clock();
        pqxx::connection conn{"hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234"};
        std::clock_t start_2 = clock();
        pqxx::work tx(conn);

        std::unique_ptr<tmp::Repository<uint64_t, std::map<std::string, std::string>>> repo = std::make_unique<tmp::postgres::ContractorRepository>(&tx);
        std::map<std::string, std::string> data;
        data.insert(std::make_pair("company_id", "B3_1000"));
        data.insert(std::make_pair("email", "alice@loki.org"));
        data.insert(std::make_pair("password", "alice1111"));
        data.insert(std::make_pair("name", "Alice"));
        data.insert(std::make_pair("roles", ""));
        *id = repo->insert(std::move(data));
        print_debug_v3("id: ", *id);
// throw std::runtime_error("It's test runtime error.");
        tx.commit();
        conn.close();
        std::clock_t end = clock();
        std::cout << "passed " << (double)(end-start_1)/CLOCKS_PER_SEC << " sec." << std::endl;
        std::cout << "passed " << (double)(end-start_2)/CLOCKS_PER_SEC << " sec." << std::endl;
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_postgres_insert_v3(std::string* id)
{
    puts("------ test_postgres_insert_v3");
    try {
        pqxx::connection conn{"hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234"};
        pqxx::work tx(conn);
        std::unique_ptr<tmp::Repository<uint64_t, std::map<std::string, std::string>>> adaptee = std::make_unique<tmp::postgres::ContractorRepository>(&tx);
        std::unique_ptr<tmp::Repository<std::string, std::map<std::string, std::string>>> adapter = std::make_unique<tmp::postgres::RepositoryAdapter>(adaptee.get());

        std::map<std::string, std::string> data;
        data.insert(std::make_pair("company_id", "B3_1000"));
        data.insert(std::make_pair("email", "jack@loki.org"));
        data.insert(std::make_pair("password", "jack1111"));
        data.insert(std::make_pair("name", "Jack"));
        data.insert(std::make_pair("roles", "Admin"));
        *id = adapter->insert(std::move(data));
        print_debug_v3("id: ", *id);
// throw std::runtime_error("It's test runtime error.");
        tx.commit();
        conn.close();
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_postgres_update(long* id)
{
    puts("------ test_postgres_update");
    try {
        pqxx::connection conn{"hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234"};
        // std::string sql = R"(
        //     UPDATE contractor
        //     SET id=$1, company_id=$2, email=$3, password=$4, name=$5, roles=$6 
        //     WHERE id=$1
        // )";
        std::string sql = update_sql("contractor", "id", "company_id", "email", "password", "name", "roles");
        ptr_print_debug<const std::string&, std::string&>("sql: \n", sql);
        SqlBuilder builder{"update_contractor", sql};
        pqxx::work tx(builder.makePrepare(conn));
        pqxx::result result = tx.exec(builder.makePrepped(), builder.makeParams(*id, "bar_3333D", "alice1234@bar.com", "alice12345678", "Alice", "User"));
        for (auto const &row: result) {
            for (auto const &field: row) std::cout << field.c_str() << '\t';
            std::cout << '\n';
        }
        tx.commit();
        conn.close();
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_postgres_update_v2(uint64_t* id)
{
    puts("------ test_postgres_update_v2");
    try {
        pqxx::connection conn{"hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234"};
        pqxx::work tx(conn);
        std::unique_ptr<tmp::Repository<uint64_t, std::map<std::string, std::string>>> repo = std::make_unique<tmp::postgres::ContractorRepository>(&tx);
        std::map<std::string, std::string> data;
        data.insert(std::make_pair("company_id", "B3_1333"));
        data.insert(std::make_pair("email", "bar@loki.org"));
        data.insert(std::make_pair("password", "bar1111"));
        data.insert(std::make_pair("name", "Bar"));
        data.insert(std::make_pair("roles", "User"));
        repo->update(*id, std::move(data));
        tx.commit();
        conn.close();
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_postgres_update_v3(std::string* id)
{
    puts("------ test_postgres_update_v3");
    try {
        pqxx::connection conn{"hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234"};
        pqxx::work tx(conn);
        std::unique_ptr<tmp::Repository<uint64_t, std::map<std::string, std::string>>> adaptee = std::make_unique<tmp::postgres::ContractorRepository>(&tx);
        std::unique_ptr<tmp::Repository<std::string, std::map<std::string, std::string>>> adapter = std::make_unique<tmp::postgres::RepositoryAdapter>(adaptee.get());
        std::map<std::string, std::string> data;
        data.insert(std::make_pair("company_id", "B4_4444"));
        data.insert(std::make_pair("email", "jjjj@loki.org"));
        data.insert(std::make_pair("password", "jjjj1111"));
        data.insert(std::make_pair("name", "JJJJ"));
        data.insert(std::make_pair("roles", "User"));
        adapter->update(*id, std::move(data));
        tx.commit();
        conn.close();
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_postgres_select(long* id)
{
    puts("------ test_postgres_select");
    try {
        pqxx::connection conn{"hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234"};
        // conn.prepare(
        //     "select_contractor",
        //     R"(
        //     SELECT * FROM contractor
        //     WHERE id = $1)"
        // );
        // std::string sql = R"(
        //     SELECT * FROM contractor
        //     WHERE id = $1
        // )";
        std::string sql = select_by_pkey_sql("contractor", "id");
        ptr_print_debug<const std::string&, std::string&>("sql: \n", sql);
        SqlBuilder builder{"select_contractor", sql};
        pqxx::work tx(builder.makePrepare(conn));
        pqxx::result result = tx.exec(builder.makePrepped(), builder.makeParams(*id));
        tx.commit();
        conn.close();
        for (auto const &row: result) {
            for (auto const &row: result) {
                auto [id, companyId, email, password, name, roles] = row.as<int, std::string, std::string, std::string, std::string, const char*>();    // テーブルで、null を許可している場合は const char* を使わざるを得ない。
                if(roles) {
                    std::cout << id << '\t' << companyId << '\t' << email << '\t' << password << '\t' << name << '\t' << roles << std::endl;
                } else {
                    std::cout << id << '\t' << companyId << '\t' << email << '\t' << password << '\t' << name << std::endl;
                }
                Contractor contractor(id, companyId, email, password, name, roles);
                contractor.print();
            }
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_postgres_select_v2(uint64_t* id)
{
    puts("------ test_postgres_select_v2");
    try {
        pqxx::connection conn{"hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234"};
        pqxx::work tx(conn);

        std::unique_ptr<tmp::Repository<uint64_t, std::map<std::string, std::string>>> repo = std::make_unique<tmp::postgres::ContractorRepository>(&tx);
        std::optional<std::map<std::string, std::string>> data = repo->findById(*id);
        tx.commit();
        conn.close();
        if(data.has_value()) {
            print_debug_v3("Hit data ... id: ", *id);
            for(auto f: data.value()) {
                std::cout << f.first << '\t' << f.second << std::endl;
            }
        } else {
            print_debug_v3("No data ... id: ", *id);
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_postgres_select_v3(std::string* id)
{
    puts("------ test_postgres_select_v3");
    try {
        pqxx::connection conn{"hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234"};
        pqxx::work tx(conn);
        std::unique_ptr<tmp::Repository<uint64_t, std::map<std::string, std::string>>> adaptee = std::make_unique<tmp::postgres::ContractorRepository>(&tx);
        std::unique_ptr<tmp::Repository<std::string, std::map<std::string, std::string>>> adapter = std::make_unique<tmp::postgres::RepositoryAdapter>(adaptee.get());
        std::optional<std::map<std::string, std::string>> data = adapter->findById(*id);
        tx.commit();
        conn.close();
        if(data.has_value()) {
            print_debug_v3("Hit data ... id: ", *id);
            for(auto f: data.value()) {
                std::cout << f.first << '\t' << f.second << std::endl;
            }
        } else {
            print_debug_v3("No data ... id: ", *id);
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_postgres_select_field(long* id)
{
    puts("------ test_postgres_select_field");
    try {
        pqxx::connection conn{"hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234"};
        std::string sql = select_by_pkey_sql("contractor", "id", "id", "name");
        ptr_print_debug<const std::string&, std::string&>("sql: \n", sql);
        SqlBuilder builder{"select_contractor", sql};
        pqxx::work tx(builder.makePrepare(conn));
        pqxx::result result = tx.exec(builder.makePrepped(), builder.makeParams(*id));
        tx.commit();
        conn.close();
        for (auto const &row: result) {
            for (auto const &row: result) {
                auto [id, name] = row.as<int, std::string>();
                std::cout << id << '\t' << name << std::endl;
            }
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_postgres_delete(long* id)
{
    puts("------ test_postgres_delete");
    try {
        pqxx::connection conn{"hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234"};
        // conn.prepare(
        //     "delete_contractor",
        //     R"(
        //     DELETE FROM contractor
        //     WHERE id = $1)"
        // );
        // std::string sql = R"(DELETE FROM contractor WHERE id = $1)";
        std::string sql = delete_by_pkey_sql("contractor", "id");
        ptr_print_debug<const std::string&, std::string&>("sql: \n", sql);
        SqlBuilder builder{"delete_contractor", sql};
        pqxx::work tx(builder.makePrepare(conn));
        pqxx::result result = tx.exec(builder.makePrepped(), builder.makeParams(*id));
        tx.commit();
        conn.close();
        for (auto const &row: result) {
            for (auto const &field: row) std::cout << field.c_str() << '\t';
            std::cout << '\n';
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_postgres_delete_v2(uint64_t* id)
{
    puts("------ test_postgres_delete_v2");
    try {
        pqxx::connection conn{"hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234"};
        pqxx::work tx(conn);

        std::unique_ptr<tmp::Repository<uint64_t, std::map<std::string, std::string>>> repo = std::make_unique<tmp::postgres::ContractorRepository>(&tx);
        repo->remove(*id);
        tx.commit();
        conn.close();
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_postgres_delete_v3(std::string* id)
{
    puts("------ test_postgres_delete_v3");
    try {
        pqxx::connection conn{"hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234"};
        pqxx::work tx(conn);
        std::unique_ptr<tmp::Repository<uint64_t, std::map<std::string, std::string>>> adaptee = std::make_unique<tmp::postgres::ContractorRepository>(&tx);
        std::unique_ptr<tmp::Repository<std::string, std::map<std::string, std::string>>> adapter = std::make_unique<tmp::postgres::RepositoryAdapter>(adaptee.get());
        adapter->remove(*id);
        tx.commit();
        conn.close();
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_postgres_insert_v4(uint64_t* id)
{
    puts("------ test_postgres_insert_v4");
    using iface = tmp::Repository<uint64_t, std::map<std::string, std::string>>;
    using subclazz = tmp::postgres::PqxxRepository;
    try {
        std::clock_t start_1 = clock();
        pqxx::connection conn{"hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234"};
        std::clock_t start_2 = clock();
        pqxx::work tx(conn);

        tmp::postgres::ContractorInfo info;
        std::unique_ptr<iface> repo = std::make_unique<subclazz>(&tx, &info);
        std::map<std::string, std::string> data;
        data.insert(std::make_pair("company_id", "B3_1000"));
        data.insert(std::make_pair("email", "alice@loki.org"));
        data.insert(std::make_pair("password", "alice1111"));
        data.insert(std::make_pair("name", "Alice"));
        data.insert(std::make_pair("roles", ""));
        *id = repo->insert(std::move(data));
        print_debug_v3("id: ", *id);
// throw std::runtime_error("It's test runtime error.");
        tx.commit();
        conn.close();
        std::clock_t end = clock();
        std::cout << "passed " << (double)(end-start_1)/CLOCKS_PER_SEC << " sec." << std::endl;
        std::cout << "passed " << (double)(end-start_2)/CLOCKS_PER_SEC << " sec." << std::endl;
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_postgres_update_v4(uint64_t* id)
{
    puts("------ test_postgres_update_v4");
    using iface = tmp::Repository<uint64_t, std::map<std::string, std::string>>;
    using subclazz = tmp::postgres::PqxxRepository;
    try {
        pqxx::connection conn{"hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234"};
        pqxx::work tx(conn);
        tmp::postgres::ContractorInfo info;
        std::unique_ptr<iface> repo = std::make_unique<subclazz>(&tx, &info);
        std::map<std::string, std::string> data;
        data.insert(std::make_pair("company_id", "B4_1444"));
        data.insert(std::make_pair("email", "joe@loki.org"));
        data.insert(std::make_pair("password", "joe1111"));
        data.insert(std::make_pair("name", "Joe"));
        data.insert(std::make_pair("roles", "User"));
        repo->update(*id, std::move(data));
        tx.commit();
        conn.close();
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_postgres_select_v4(uint64_t* id)
{
    puts("------ test_postgres_select_v4");
    using iface = tmp::Repository<uint64_t, std::map<std::string, std::string>>;
    using subclazz = tmp::postgres::PqxxRepository;
    try {
        pqxx::connection conn{"hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234"};
        pqxx::work tx(conn);
        tmp::postgres::ContractorInfo info;
        std::unique_ptr<iface> repo = std::make_unique<subclazz>(&tx, &info);
        std::optional<std::map<std::string, std::string>> data = repo->findById(*id);
        tx.commit();
        conn.close();
        if(data.has_value()) {
            print_debug_v3("Hit data ... id: ", *id);
            for(auto f: data.value()) {
                std::cout << f.first << '\t' << f.second << std::endl;
            }
        } else {
            print_debug_v3("No data ... id: ", *id);
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_postgres_delete_v4(uint64_t* id)
{
    puts("------ test_postgres_delete_v4");
    using iface = tmp::Repository<uint64_t, std::map<std::string, std::string>>;
    using subclazz = tmp::postgres::PqxxRepository;
    try {
        pqxx::connection conn{"hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234"};
        pqxx::work tx(conn);
        tmp::postgres::ContractorInfo info;
        std::unique_ptr<iface> repo = std::make_unique<subclazz>(&tx, &info);
        repo->remove(*id);
        tx.commit();
        conn.close();
        ptr_print_debug<const std::string&, decltype(*id)&>("delete id: ", *id);
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int main(void)
{
    puts("START main ===");
    if(0) {
        ProcA a(2, "start");
        a.build().build().finish();
        puts("--- Proceed");
        Proceed proceed;
        proceed.makeProc1().makeProc2();
        proceed.makeProc1();
        proceed.debug();
    }
    int ret = -1;
    if(0) {
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_print_debug());
        assert(ret == 0);
    }
    if(0) {
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_ObjectPool());
        assert(ret == 0);
    }
    if(0) {
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_postgres_connection());
        assert(ret == 0);
    }
    if(0) {
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = setupTable());
        assert(ret == 0);
        long id = 0;
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_postgres_insert(&id));
        assert(ret == 0);
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_postgres_select(&id));
        assert(ret == 0);
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_postgres_update(&id));
        assert(ret == 0);
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_postgres_select(&id));
        assert(ret == 0);
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_postgres_select_field(&id));
        assert(ret == 0);
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_postgres_delete(&id));
        assert(ret == 0);
    }
    if(0) {
        uint64_t id = 0;
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_postgres_insert_v2(&id));
        assert(ret == 0);
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_postgres_select_v2(&id));
        assert(ret == 0);
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_postgres_update_v2(&id));
        assert(ret == 0);
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_postgres_select_v2(&id));
        assert(ret == 0);
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_postgres_delete_v2(&id));
        assert(ret == 0);
    }
    if(0) {
        std::string sid{""};
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_postgres_insert_v3(&sid));
        assert(ret == 0);
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_postgres_update_v3(&sid));
        assert(ret == 0);
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_postgres_select_v3(&sid));
        assert(ret == 0);
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_postgres_delete_v3(&sid));
        assert(ret == 0);
    }
    if(1) {
        uint64_t id = 0;
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_postgres_insert_v4(&id));
        assert(ret == 0);
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_postgres_update_v4(&id));
        assert(ret == 0);
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_postgres_select_v4(&id));
        assert(ret == 0);
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_postgres_delete_v4(&id));
        assert(ret == 0);
    }
    puts("=== main END");
}