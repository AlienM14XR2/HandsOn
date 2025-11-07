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
#include <ObjectPool.hpp>
#include <pqxx/pqxx>

template <class M, class D>
void (*ptr_print_debug)(M, D) = [](const auto message, const auto debug) -> void
{
    std::cout << "DEBUG: " << message << '\t' << debug << std::endl;
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
};

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
        std::string sql = R"(
            INSERT INTO contractor (id, company_id, email, password, name, roles) 
            VALUES ($1, $2, $3, $4, $5, $6)
        )";
        SqlBuilder builder{"insert_contractor", sql};
        pqxx::work tx(builder.makePrepare(conn));
        *id = tx.query_value<long>(
            "SELECT nextval('contractor_id_seq')"
        );
        printf("nextId: %ld\n", *id);
        // DONE pqxx::prepped と pqxx::params のインスタンス化を SQLの発行場所で管理できないか。
        pqxx::result result = tx.exec(builder.makePrepped(), builder.makeParams(*id, "bar_3333D", "foo1234@bar.com", "foo12345678", "foo1234", "Admin,User"));
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
        std::string sql = R"(
            SELECT * FROM contractor
            WHERE id = $1
        )";
        SqlBuilder builder{"select_contractor", sql};
        pqxx::work tx(builder.makePrepare(conn));
        pqxx::result result = tx.exec(builder.makePrepped(), builder.makeParams(*id));
        tx.commit();
        conn.close();
        for (auto const &row: result) {
            for (auto const &row: result) {
                auto [id, companyId, email, password, name, roles] = row.as<int, std::string, std::string, std::string, std::string, const char*>();    // テーブルで、null を許可している場合は const char* を使わざるを得ない。
                std::cout << id << '\t' << companyId << '\t' << email << '\t' << password << '\t' << name << '\t' << roles << std::endl;
                if(roles) {
                    std::string sroles(roles);
                    printf("roles: %s\n", sroles.c_str());
                }
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
        std::string sql = R"(DELETE FROM contractor WHERE id = $1)";
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

int main(void)
{
    puts("START main ===");
    if(1) {
        ProcA a(2, "start");
        a.build().build().finish();
        puts("--- Proceed");
        Proceed proceed;
        proceed.makeProc1().makeProc2();
        proceed.makeProc1();
        proceed.debug();
    }
    int ret = -1;
    if(1) {
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_print_debug());
        assert(ret == 0);
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = setupTable());
        assert(ret == 0);
    }
    if(1) {
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_ObjectPool());
        assert(ret == 0);
    }
    if(1) {
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_postgres_connection());
        assert(ret == 0);
    }
    if(1) {
        long id = 0;
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_postgres_insert(&id));
        assert(ret == 0);
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_postgres_select(&id));
        assert(ret == 0);
        ptr_print_debug<const std::string&, int&>("Play and Result ... ", ret = test_postgres_delete(&id));
        assert(ret == 0);
    }
    puts("=== main END");
}