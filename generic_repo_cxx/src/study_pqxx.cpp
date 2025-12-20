/**
 * 習作　PostgreSQL に関するもの
 * 
 * 
 * e.g. compile
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Wextra -Werror -I../inc -I/usr/local/include -I/usr/include/postgresql -L/usr/local/lib study_pqxx.cpp -lpqxx -lpq -o ../bin/study_pqxx
 */
#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <cxxabi.h>
#include <cassert>
#include <any>
#include <functional>
#include <optional>
#include <random>
#include <ctime>
#include <chrono>
#include <string_view> // C++17以降 @see void safe_print(const char* p)
#include <unordered_map>
#include <algorithm> // for find_if, if you keep using vector
#include <stdexcept>

#include <Repository.hpp>
#include <sql_helper.hpp>
#include <ObjectPool.hpp>
#include <pqxx_template.hpp>
#include <pqxx/pqxx>
#include <pqxx/version>

// class Transaction
// {
// public:
//     virtual ~Transaction() = default;
//     virtual void begin() = 0;
//     virtual void commit() = 0;
//     virtual void rollback() = 0;
// };

// class PostgresTransaction final : public Transaction {
// private:
//     pqxx::connection* const conn;
//     std::unique_ptr<pqxx::work> work; // libpqxx のトランザクション本体

// public:
//     explicit PostgresTransaction(pqxx::connection* const _conn) 
//         : conn(_conn), work(nullptr) {
//         if (!conn) throw std::invalid_argument("Connection is null.");
//     }

//     void begin() override {
//         if (work) throw std::runtime_error("Transaction already started.");
//         // インスタンス化の瞬間に BEGIN 発行
//         work = std::make_unique<pqxx::work>(*conn);
//     }

//     void commit() override {
//         if (!work) throw std::runtime_error("No active transaction to commit.");
//         work->commit();
//         work.reset(); // 完了後、速やかに破棄して connection を解放
//     }

//     void rollback() override {
//         if (work) {
//             try {
//                 work->abort();
//             } catch (...) {
//                 // ロールバック中の例外は握り潰し、堅牢性を維持
//             }
//             work.reset();
//         }
//     }

//     // コピー禁止
//     PostgresTransaction(const PostgresTransaction&) = delete;
//     PostgresTransaction& operator=(const PostgresTransaction&) = delete;
// };

// サービス層のサンプル
// サンプルでは一つのテーブルで表現しているが、複数の異なるテーブルに
// 対応したリポジトリが扱える。
template<typename... Repos>
class MyBusinessService : public tmp::ServiceExecutor {
    using Data = tmp::VarNode;
private:
    std::tuple<Repos...> repos; // 複数のリポジトリを保持

public:
    // 可変引数でリポジトリを受け取る
    explicit MyBusinessService(Repos&&... args) : repos(args...) {}

    void execute() override {
        using Data = tmp::VarNode;
        // Insert
        const std::string pkey  = "id";
        Data root{"__DATA__KEY__", std::monostate{}};
        uint64_t nid{0};
        root.addChild(pkey, nid);
        std::string companyId{"B3_1000"};
        root.addChild("company_id", companyId);
        std::string email{"alice@loki.org"};
        root.addChild("email", email);
        std::string password{"alice1111"};
        root.addChild("password", password);
        std::string name{"Alice"};
        root.addChild("name", name);
        int64_t id = static_cast<tmp::postgres::r3::VarNodeRepository<uint64_t>>(std::get<0>(repos)).insert(std::move(root));
        tmp::print_debug("id: ", id);
        // Update
        Data updateNode{"__DATA__KEY__", std::monostate{}};
        uint64_t u_nid{0};
        updateNode.addChild(pkey, u_nid);
        std::string u_companyId{"B3_3333"};
        updateNode.addChild("company_id", u_companyId);
        std::string u_email{"foo@loki.org"};
        updateNode.addChild("email", u_email);
        std::string u_password{"foo1111"};
        updateNode.addChild("password", u_password);
        std::string u_name{"Foo"};
        updateNode.addChild("name", u_name);
        std::string u_roles{"Admin,User"};
        updateNode.addChild("roles", u_roles);
        static_cast<tmp::postgres::r3::VarNodeRepository<uint64_t>>(std::get<1>(repos)).update(id, std::move(updateNode));
        // Find
        std::optional<Data> f_result = static_cast<tmp::postgres::r3::VarNodeRepository<uint64_t>>(std::get<2>(repos)).findById(id);
        if(!(f_result)) throw std::runtime_error("Unexpected case.");
        else tmp::debug_print_varnode(&(f_result.value()));
        // 4. Remove
        static_cast<tmp::postgres::r3::VarNodeRepository<uint64_t>>(std::get<3>(repos)).remove(id);
    }
};

int test_execute_service_with_tx_M1()
{
    using Data = tmp::VarNode;
    puts("------ test_execute_service_with_tx_M1");
    // Poolオブジェクトの作成
    auto pool = ObjectPool<pqxx::connection>::create("postgres");
    // コネクション（セッション）をひとつPool に確保。
    pool->push(std::make_unique<pqxx::connection>(pqxx::connection("hostaddr=127.0.0.1 port=5432 dbname=derek user=derek password=derek1234")));
    // Poolから取り出す
    auto conn = pool->pop();
    pqxx::work tx(*(conn.get()));   // これが実質 Tx BEGIN
    try {
        const std::string table = "contractor";
        const std::string idseq = "contractor_id_seq";
        const std::string pkey  = "id";
        tmp::postgres::r3::VarNodeRepository<uint64_t> insert_repo{&tx, table, idseq, pkey};
        tmp::postgres::r3::VarNodeRepository<uint64_t> update_repo{&tx, table, idseq, pkey};
        auto dataMapper = [](Data& root, const pqxx::row& row) -> void {
            // テーブルで、null を許可している場合は const char* を使わざるを得ない。
            auto [id, companyId, email, password, name, roles] = row.as<uint64_t, std::string, std::string, std::string, std::string, const char*>();
            tmp::print_debug(id, companyId, email, password, name, roles);
            root.addChild("id", id);
            root.addChild("companyId", companyId);
            root.addChild("email", email);
            root.addChild("password", password);
            root.addChild("name", name);
            if(roles) root.addChild("roles", std::string(roles));
            else root.addChild("roles", std::monostate{});
        };
        tmp::postgres::r3::VarNodeRepository<uint64_t> find_repo{&tx, table, idseq, dataMapper, pkey};
        tmp::postgres::r3::VarNodeRepository<uint64_t> remove_repo{&tx, table, idseq, pkey};
        MyBusinessService service(
            std::move(insert_repo), 
            std::move(update_repo), 
            std::move(find_repo),
            std::move(remove_repo)
        );
        tmp::postgres::r3::execute_service_with_tx(tx, service);
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        tmp::ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int main()
{
    puts("START main ===");
    int ret = -1;
    if(1) {
        tmp::print_debug("Play and Result ... ", ret = test_execute_service_with_tx_M1());
        assert(ret == 0);
    }
    puts("=== main END");
    return EXIT_SUCCESS;
}