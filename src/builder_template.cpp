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
 * 未来の私への忠告だ、このソースは現在、理想を求めすぎたRepositry を参照、インクルードしていることに留意しろ。
 * 完成形が知りたければ、generic_repo_cxx ディレクトリをみろ。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Wextra -Werror -I../inc -I/usr/local/include -I/usr/include/postgresql -L/usr/local/lib builder_template.cpp -lpqxx -lpq -o ../bin/builder_template
 */
#include <iostream>
#include <memory>
#include <cassert>
#include <map>
#include <vector>
#include <random>
#include <ctime>
#include <chrono>
#include <algorithm>
#include <string_view> // C++17以降 @see void safe_print(const char* p)
#include <unordered_map>
#include <algorithm> // for find_if, if you keep using vector

#include <ObjectPool.hpp>
#include <Repository.hpp>
#include <sql_helper.hpp>
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

/**
 * 乱数生成関数（C++）
 */
float random_cxx()
{
    std::random_device seed_gen;                            // 非決定的な乱数生成器
    std::mt19937 engine(seed_gen());                        // 擬似乱数製正規。ランダムなシードを設定する。
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);        // 分布方法
    return dist(engine);                                    // 乱数生成
}

std::string generate_id()
{
    const float t = random_cxx();
    auto now = std::chrono::system_clock::now();
    // エポック (起点) からの経過時間をナノ秒単位のdurationに変換
    auto nanoseconds_since_epoch = std::chrono::duration_cast<std::chrono::nanoseconds>(
        now.time_since_epoch()
    );
    // ナノ秒のカウントを取得 (整数値)
    const long long nanosecond_count = nanoseconds_since_epoch.count();
    const float b = random_cxx();
    return std::to_string(t) + std::to_string(nanosecond_count) + std::to_string(b);
}
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
    // 多少記憶は古いが、コネクションをプーリングしている際は次のsqlName は何らかの方法
    // でその一意性を確保しないとDB 側でエラーになる（はず）。
    const std::string sqlName;  // postgres では発行PreparedStatement 単位で何らかの名前が必要。
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
        return tmp::postgres::helper::insert_sql(tableName
            , fieldNames[0]
            , fieldNames[1]
            , fieldNames[2]
            , fieldNames[3]
            , fieldNames[4]
            , fieldNames[5]);
    }
    virtual std::string updateSql() const override
    {
        return tmp::postgres::helper::update_by_pkey_sql(tableName
            , fieldNames[0]
            , fieldNames[1]
            , fieldNames[2]
            , fieldNames[3]
            , fieldNames[4]
            , fieldNames[5]);
    }
    virtual std::string deleteSql() const override
    {
        return tmp::postgres::helper::delete_by_pkey_sql(tableName, fieldNames[0]);
    }
    virtual std::string selectByIdSql() const override
    {
        return tmp::postgres::helper::select_by_pkey_sql(tableName, fieldNames[0]);
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
    virtual uint64_t insert(Data&& data) override
    {
        print_debug_v3("insert ... ", typeid(*this).name());
        std::string sql = tableInfo->insertSql();
        ptr_print_debug<const std::string&, std::string&>("sql: \n", sql);
        SqlBuilder builder{generate_id(), sql};
        uint64_t id = tx->query_value<uint64_t>(
            builder.makeNextvalSql(tableInfo->getSeqName())
        );
        printf("nextval: %lu\n", id);
        builder.makePrepare(tx->conn());
        tx->exec(builder.makePrepped(), tableInfo->makeParams4Save(builder, id, std::move(data)));
        return id;
    }
    virtual void update(const uint64_t& id, Data&& data) override
    {
        print_debug_v3("update ... ", typeid(*this).name());
        std::string sql = tableInfo->updateSql();
        ptr_print_debug<const std::string&, std::string&>("sql: \n", sql);
        SqlBuilder builder{generate_id(), sql};
        builder.makePrepare(tx->conn());
        tx->exec(builder.makePrepped(), tableInfo->makeParams4Save(builder, id, std::move(data)));
    }
    virtual void remove(const uint64_t& id) override
    {
        print_debug_v3("remove ... ", typeid(*this).name());
        // std::string sql = delete_by_pkey_sql("contractor", "id");
        std::string sql = tableInfo->deleteSql();
        ptr_print_debug<const std::string&, std::string&>("sql: \n", sql);
        SqlBuilder builder{generate_id(), sql};
        builder.makePrepare(tx->conn());
        tx->exec(builder.makePrepped(), builder.makeParams(id));
    }
    virtual std::optional<Data> findById(const uint64_t& id) const override
    {
        print_debug_v3("findById ... ", typeid(*this).name());
        // std::string sql = select_by_pkey_sql("contractor", "id");
        std::string sql = tableInfo->selectByIdSql();
        ptr_print_debug<const std::string&, std::string&>("sql: \n", sql);
        SqlBuilder builder{generate_id(), sql};
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
    virtual uint64_t insert(std::map<std::string, std::string>&& data) override
    {
        print_debug_v3("insert ... ", typeid(*this).name());
        uint64_t id = tx->query_value<uint64_t>(
            "SELECT nextval('contractor_id_seq')"
        );
        printf("nextval: %lu\n", id);
        std::string sql = tmp::postgres::helper::insert_sql("contractor", "id", "company_id", "email", "password", "name", "roles");
        ptr_print_debug<const std::string&, std::string&>("sql: \n", sql);
        SqlBuilder builder{generate_id(), sql};
        builder.makePrepare(tx->conn());
        tx->exec(builder.makePrepped(), builder.makeParams(id, data.at("company_id"), data.at("email"), data.at("password"), data.at("name"), data.at("roles")));
        return id;
    }
    virtual void update(const uint64_t& id, std::map<std::string, std::string>&& data) override
    {
        print_debug_v3("update ... ", typeid(*this).name());
        std::string sql = tmp::postgres::helper::update_by_pkey_sql("contractor", "id", "company_id", "email", "password", "name", "roles");
        ptr_print_debug<const std::string&, std::string&>("sql: \n", sql);
        SqlBuilder builder{generate_id(), sql};
        builder.makePrepare(tx->conn());
        tx->exec(builder.makePrepped(), builder.makeParams(id, data.at("company_id"), data.at("email"), data.at("password"), data.at("name"), data.at("roles")));
    }
    virtual void remove(const uint64_t& id) override
    {
        print_debug_v3("remove ... ", typeid(*this).name());
        std::string sql = tmp::postgres::helper::delete_by_pkey_sql("contractor", "id");
        ptr_print_debug<const std::string&, std::string&>("sql: \n", sql);
        SqlBuilder builder{generate_id(), sql};
        builder.makePrepare(tx->conn());
        tx->exec(builder.makePrepped(), builder.makeParams(id));
    }
    virtual std::optional<std::map<std::string, std::string>> findById(const uint64_t& id) const override
    {
        print_debug_v3("findById ... ", typeid(*this).name());
        std::string sql = tmp::postgres::helper::select_by_pkey_sql("contractor", "id");
        ptr_print_debug<const std::string&, std::string&>("sql: \n", sql);
        SqlBuilder builder{generate_id(), sql};
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
    virtual std::string insert(std::map<std::string, std::string>&& data) override
    {
        uint64_t id = adaptee->insert(std::forward<std::map<std::string, std::string>>(data));
        return std::to_string(id);
    }
    virtual void update(const std::string& id, std::map<std::string, std::string>&& data) override
    {
        adaptee->update(std::stoul(id), std::forward<std::map<std::string, std::string>>(data));
    }
    virtual void remove(const std::string& id) override
    {
        adaptee->remove(std::stoul(id));
    }
    virtual std::optional<std::map<std::string, std::string>> findById(const std::string& id) const override
    {
        return adaptee->findById(std::stoul(id));
    }
};

class SqlExecutor final {
    using Data = std::map<std::string,std::string>;
private:
    pqxx::work* const tx;
public:
    SqlExecutor(pqxx::work* const _tx): tx{_tx}
    {}
    std::vector<Data> findProc_v1(std::vector<std::string>&& sqlSyntax, Data (*dataSetHelper)(const pqxx::const_result_iterator::reference& _row))
    {
        // 事前に次のデータを登録した。
        // INSERT INTO contractor(company_id, email, password, name, roles)
        // VALUES
        // ('A1_1000', 'admin@example.com', '$2a$10$Ff.Tr2q.fBciJKiA1b4wAOwNpjr9RypX3DHeQwLQQg0GMxGrl4FcO', 'admin', 'ROLE_ADMIN'),
        // ('A1_1000', 'student@example.com', '$2a$10$fzJDR7BSMQnRg9Odg/JDzemQdLc6g2a7lR/ccHoMQEoxpxhs6dT0m', 'student', 'ROLE_USER');

        // psql ではPrepared Statement に同じ名前を利用するとError になる。
        // PREPARE stmt_1(text) AS
        // 	SELECT id, company_id, email FROM contractor
        // 	WHERE company_id = $1;
        // EXECUTE stmt_1('A1_1000');
        size_t size = sqlSyntax.size();
        size_t i = 0;
        std::vector<Data> result;
        for(auto sql: sqlSyntax) {
            i++;
            if(i < size) tx->exec(sql);
            else {
                pqxx::result res = tx->exec(sql);
                for (auto const &row: res) {
                    print_debug_v3("colums: ", res.columns());
                    // mysql 同様すべて本メソッドで完結させたかったが
                    // posgresql ではそれらしいものが見当たらず断念した。
                    // 結局コールバック関数で、データをセットするものが外部に必要だと思う。
                    result.push_back(dataSetHelper(row));
                }
            }
        }
        return result;
    }
    void cudProc_v1(std::vector<std::string>&& sqlSyntax)
    {
        for(auto sql: sqlSyntax) {
            tx->exec(sql);
        }
        // size_t size = sqlSyntax.size();
        // size_t i = 0;
        // for(auto sql: sqlSyntax) {
        //     i++;
        //     if(i < size) tx->exec(sql);
        //     else {
        //         // 結果の返却がないならもった単純にかけるね。
        //         pqxx::result res = tx->exec(sql);
        //         std::cout << "columns: " << res.columns() << std::endl;
        //     }
        // }
    }
};

class SimplePrepare final {
// e.g.
// PREPARE fooplan (int, text, bool, numeric) AS
// INSERT INTO foo VALUES($1, $2, $3, $4);
// EXECUTE fooplan(1, 'Hunter Valley', 't', 200.00);

private:
    std::string name;
    std::vector<std::string> types;
    std::vector<std::string> values;
    std::string query;

    void pushValue(const std::string& _value)
    {
        values.push_back(_value);
    }
public:
    SimplePrepare(const std::string& _name): name{_name}
    {}
    SimplePrepare& type(const std::string& _type)
    {
        types.push_back(_type);
        return *this;
    }
    SimplePrepare& value(const int& _value)
    {
        pushValue(std::to_string(_value));
        return *this;
    }
    SimplePrepare& value(const float& _value)
    {
        pushValue(std::to_string(_value));
        return *this;
    }
    SimplePrepare& value(const std::string& _value)
    {
        std::string val = "'" + _value + "'";
        values.push_back(val);
        return *this;
    }
    SimplePrepare& setQuery(const std::string& _query) noexcept
    {
        query = _query;
        return *this;
    }
    std::vector<std::string> build() const
    {
        std::vector<std::string> result;
        // PREPARE
        std::string prepare{"PREPARE "};
        prepare.append(name).append("( ");
        size_t i = 0;
        std::string t{""};
        for(const std::string& s: types) {
            if(i == 0) t.append(s);
            else t.append(", ").append(s);
            i++;
        }
        t.append(") AS \n");
        // query
        t.append(query).append(";\n");
        result.push_back(prepare + t);
        // EXECUTE
        std::string exec{"EXECUTE "};
        exec.append(name).append("(");
        i = 0;
        std::string v{""};
        for(const std::string& s: values) {
            if(i == 0) v.append(s);
            else v.append(", ").append(s);
            i++;
        }
        v.append(");\n");
        result.push_back(exec + v);
        return result;
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

// int test_select_by_pkey_sql_M1A1()
// {
//     puts("------ test_select_by_pkey_sql_M1A1");
//     try {
//         std::string sql = tmp::postgres::helper::select_by_pkey_sql("contractor", "id", "company_id", "email", "password");
//         print_debug_v3("sql: ", sql);
//         return EXIT_SUCCESS;
//     } catch(std::exception& e) {
//         ptr_print_error<decltype(e)&>(e);
//         return EXIT_FAILURE;
//     }
// }

int test_ObjectPool()
{
    puts("------ test_ObjectPool");
    try {
        // ObjectPool<pqxx::connection> pool("pqxx::connection");
        std::unique_ptr<pqxx::connection> conn1 = std::make_unique<pqxx::connection>("hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234");
        std::unique_ptr<pqxx::connection> conn2 = std::make_unique<pqxx::connection>("hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234");
        std::unique_ptr<pqxx::connection> conn3 = std::make_unique<pqxx::connection>("hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234");
        // pool.push(move(conn1));
        // pool.push(move(conn2));
        // pool.push(move(conn3));

        // while(!pool.empty()) {
        //    auto conn = pool.pop();
        //    ptr_print_debug<const std::string&, decltype(conn)&>("conn: ", conn);
        // }
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
        // ObjectPool<pqxx::connection> pool{"pqxx::connection"};
        // std::unique_ptr<pqxx::connection> conn1 = std::make_unique<pqxx::connection>("hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234");
        // pool.push(move(conn1));

        pqxx::connection conn{"hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234"};
        // std::unique_ptr<pqxx::connection> conn = pool.pop();
        pqxx::work tx(conn);
        pqxx::result result = tx.exec("SELECT 1, 2, 'Hello', 3");
        tx.commit();
        // pool.push(move(conn));

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
        std::string sql = tmp::postgres::helper::insert_sql("contractor", "id", "company_id", "email", "password", "name", "roles");
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
        std::string sql = tmp::postgres::helper::update_by_pkey_sql("contractor", "id", "company_id", "email", "password", "name", "roles");
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
        std::string sql = tmp::postgres::helper::select_by_pkey_sql("contractor", "id");
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
        std::string sql = tmp::postgres::helper::select_by_pkey_sql("contractor", "id", "id", "name");
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
        std::string sql = tmp::postgres::helper::delete_by_pkey_sql("contractor", "id");
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
        // std::map<std::string, std::string> data2;
        // data.insert(std::make_pair("company_id", "B3_1000"));
        // data.insert(std::make_pair("email", "joe@loki.org"));
        // data.insert(std::make_pair("password", "joe1111"));
        // data.insert(std::make_pair("name", "Joe"));
        // data.insert(std::make_pair("roles", "USER"));
        // uint64_t id_2 = repo->insert(std::move(data2));
        // print_debug_v3("id_2: ", id_2);
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

int test_SqlExecutor_findProc_v1()
{
    puts("------ test_SqlExecutor_findProc_v1");
    using Data = std::map<std::string,std::string>;
    try {
        // Data (*dataSetHelper)(const pqxx::const_result_iterator::reference& _row)
        Data (*helper)(const pqxx::const_result_iterator::reference&) = [](const pqxx::const_result_iterator::reference& row) -> Data {
            Data data;
            auto [id, companyId, email] = row.as<uint64_t, std::string, std::string>();
            data.insert(std::make_pair("id", std::to_string(id)));
            data.insert(std::make_pair("company_id", companyId));
            data.insert(std::make_pair("email", email));
            return data;
        };

        pqxx::connection conn{"hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234"};
        pqxx::work tx(conn);
        tmp::postgres::SqlExecutor exec{&tx};
        std::string s1 = R"(PREPARE stmt_1(text) AS
	                            SELECT id, company_id, email FROM contractor
                                WHERE company_id = $1;)";
        std::string s2 = R"(EXECUTE stmt_1('A1_1000');)";
        std::vector vec{s1, s2};
        std::vector<Data> result = exec.findProc_v1(std::move(vec), helper);
        print_debug_v3("result size: ",result.size());
        for(auto& row: result) {
            puts("--------- check data");
            for(auto& m: row) {
                print_debug_v3(m.first, m.second);
            }
        }
        // 同一コネクションで二回目を実行したらどうなるのか... psql ではプリペアドステートメント名の重複でエラーになったが。
        // 結果はやはり同じ、つまりエラーになる：）
        // この件は頭の片隅に置いておく、これはSQL の発行側が注意すべきことでExecutor（執行者） の
        // 責務ではないと考える。
        // exec.findProc_v1(std::move(vec), helper);
        return EXIT_SUCCESS;
    } catch (std::exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_SqlExecutor_findProc_v1M1()
{
    puts("------ test_SqlExecutor_findProc_v1M1");
    using Data = std::map<std::string,std::string>;
    try {
        Data (*helper)(const pqxx::const_result_iterator::reference&) = [](const pqxx::const_result_iterator::reference& row) -> Data {
            Data data;
            std::tuple<std::string, std::string, std::string> t;
            row.to(t);
            std::cout << std::get<0>(t) << '\t' << std::get<1>(t) << '\t' << std::get<2>(t) << std::endl;
            data.insert(std::make_pair("id", std::get<0>(t)));
            data.insert(std::make_pair("company_id", std::get<1>(t)));
            data.insert(std::make_pair("email", std::get<2>(t)));
            return data;
        };

        pqxx::connection conn{"hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234"};
        pqxx::work tx(conn);
        tmp::postgres::SqlExecutor exec{&tx};
        std::string s1 = R"(PREPARE stmt_1(text) AS
	                            SELECT id, company_id, email FROM contractor
                                WHERE company_id = $1;)";
        std::string s2 = R"(EXECUTE stmt_1('A1_1000');)";
        std::vector vec{s1, s2};
        std::vector<Data> result = exec.findProc_v1(std::move(vec), helper);
        print_debug_v3("result size: ",result.size());
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

int test_SimplePrepare_SqlExecutor_M1()
{
    puts("------ test_SimplePrepare_SqlExecutor_M1");
    using Data = std::map<std::string,std::string>;
    try {
        // insert
        tmp::postgres::SimplePrepare prepare{"insert_"};
        prepare.setQuery(tmp::postgres::helper::insert_sql("contractor", "company_id", "email", "password", "name", "roles"));
        prepare.type("text").type("text").type("text").type("text").type("text");
        prepare.value("D1_1000").value("foo@loki.org").value("foo2222").value("Foo").value("Admin");
        std::vector<std::string> ins_stmt = prepare.build();
        for(auto s: ins_stmt) {
            print_debug(s);
        }
        pqxx::connection conn{"hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234"};
        pqxx::work tx(conn);
        tmp::postgres::SqlExecutor exec{&tx};
        exec.cudProc_v1(move(ins_stmt));
        // select
        Data (*helper)(const pqxx::const_result_iterator::reference&) = [](const pqxx::const_result_iterator::reference& row) -> Data {
            Data data;
            auto [id, company_id, email] = row.as<uint64_t, std::string, std::string>();
            data.insert(std::make_pair("id", std::to_string(id)));
            data.insert(std::make_pair("company_id", company_id));
            data.insert(std::make_pair("email", email));
            return data;
        };
        std::string syntax_1{"PREPARE select_(int) AS select id, company_id, email from contractor order by id desc limit $1;"};
        std::string syntax_2{"EXECUTE select_(1);"};
        std::vector<std::string> vec{syntax_1, syntax_2};
        std::vector<Data> data = exec.findProc_v1(std::move(vec), helper);
        print_debug("last id: ", data[0].at("id"));
        // delete
        tmp::postgres::SimplePrepare del_prepare{"delete_"};
        del_prepare.setQuery(tmp::postgres::helper::delete_by_pkey_sql("contractor", "id"));
        del_prepare.type("int");
        del_prepare.value(data[0].at("id"));
        std::vector<std::string> del_stmt = del_prepare.build();
        for(auto s: del_stmt) {
            print_debug(s);
        }
        exec.cudProc_v1(move(del_stmt));
        tx.commit();
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
    // if(1) {
    //     ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_select_by_pkey_sql_M1A1());
    //     assert(ret == 0);
    // }
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
    if(1) {
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_SqlExecutor_findProc_v1());
        assert(ret == 0);
    }
    if(1) {
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_SqlExecutor_findProc_v1M1());
        assert(ret == 0);
    }
    if(1) {
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_SimplePrepare_SqlExecutor_M1());
        assert(ret == 0);
    }
    puts("=== main END");
}