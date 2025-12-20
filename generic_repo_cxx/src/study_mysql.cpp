/**
 * 習作　MySQLに関するもの
 * 
 * 学習、実験、簡易サンプリング等、なんでもありの場所。
 * 
 * e.g. compile
 * 非常に厳格。
 * g++ -O3 -std=c++20 -DDEBUG -pedantic-errors -Wall -Wextra -Werror -I../inc/ -I/usr/include/mysql-cppconn/ -L/usr/lib/x86_64-linux-gnu/ study_mysql.cpp -lmysqlcppconn -lmysqlcppconnx -o ../bin/study_mysql
 * 実験、サンプリングでは以下を推奨（-Wextra があると簡単に関数内をすべてコメントとかできないから。
 * g++ -O3 -std=c++20 -DDEBUG -pedantic-errors -Wall -Werror -I../inc/ -I/usr/include/mysql-cppconn/ -L/usr/lib/x86_64-linux-gnu/ study_mysql.cpp -lmysqlcppconn -lmysqlcppconnx -o ../bin/study_mysql
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
#include <sstream>
#include <string_view> // C++17以降 @see void safe_print(const char* p)

#include <Repository.hpp>
#include <ObjectPool.hpp>
#include <mysql_template.hpp>
#include <mysql/jdbc.h>
#include <mysqlx/xdevapi.h>



// ユーザーが実装する具体的なサービス
// template<typename... Repos>
// concept AllRepos = (tmp::mysql::r3::VarNodeRepository<std::uint64_t><Repos> && ...);
// template<AllRepos... Args>
template<typename... Repos>
class MyBusinessService : public tmp::ServiceExecutor {
    using Data = tmp::VarNode;
private:
    std::tuple<Repos...> repos; // 複数のリポジトリを保持

public:
    // 可変引数でリポジトリを受け取る
    explicit MyBusinessService(Repos&&... args) : repos(args...) {}

    void execute() override {
        // 1. リポジトリの取り出し
        // 2. 複数のリポジトリを「串刺し」にしたビジネスロジック
        // 例: std::get<0>(repos)->insert(...);
        // Insert
        std::string pkcol{"id"};
        Data root{"primaryKey", pkcol};
        std::string companyId{"B3_1000"};
        root.addChild("company_id", companyId);
        std::string email{"alice@loki.org"};
        root.addChild("email", email);
        std::string password{"alice1111"};
        root.addChild("password", password);
        std::string name{"Alice"};
        root.addChild("name", name);
        int64_t id = static_cast<tmp::mysql::r3::VarNodeRepository<uint64_t>>(std::get<0>(repos)).insert(std::move(root));
        tmp::print_debug("id: ", id);
        // Update
        std::string pkcol_u{"id"};
        Data updateData{"primaryKey", pkcol_u};
        std::string companyId_u{"B3_3333"};
        updateData.addChild("company_id", companyId_u);
        std::string email_u{"alice_uloki.org"};
        updateData.addChild("email", email_u);
        std::string password_u{"alice3333"};
        updateData.addChild("password", password_u);
        std::string name_u{"Alice_U"};
        updateData.addChild("name", name_u);
        std::string role_u{"Admin,User"};
        updateData.addChild("roles", role_u);
        static_cast<tmp::mysql::r3::VarNodeRepository<uint64_t>>(std::get<1>(repos)).update(id, std::move(updateData));
        // Find
        std::optional<Data> fnode = static_cast<tmp::mysql::r3::VarNodeRepository<uint64_t>>(std::get<2>(repos)).findById(id);
        if(!fnode) throw std::runtime_error("Unexpected case.");
        else tmp::debug_print_varnode(&(fnode.value()));
        // Remove
        static_cast<tmp::mysql::r3::VarNodeRepository<uint64_t>>(std::get<3>(repos)).remove(id);
    }
};

int test_execute_service_with_tx_M1()
{
    // Poolオブジェクトの作成
    auto pool = ObjectPool<mysqlx::Session>::create("mysql");
    // コネクション（セッション）をひとつPool に確保。
    pool->push(std::make_unique<mysqlx::Session>(mysqlx::Session("localhost", 33060, "root", "root1234")));
    // Poolから取り出す
    auto sess = pool->pop();
    tmp::mysql::r3::MySqlTransaction tx(sess.get());
    try {
        tmp::mysql::r3::VarNodeRepository<uint64_t> insert_repo{sess.get(), "test", "contractor"};
        tmp::mysql::r3::VarNodeRepository<uint64_t> update_repo{sess.get(), "test", "contractor"};
        tmp::mysql::r3::VarNodeRepository<uint64_t> find_repo{sess.get(), "test", "contractor"};
        tmp::mysql::r3::VarNodeRepository<uint64_t> remove_repo{sess.get(), "test", "contractor"};
        MyBusinessService service(std::move(insert_repo), std::move(update_repo), std::move(find_repo), std::move(remove_repo));
        tmp::mysql::r3::execute_service_with_tx(tx, service);
        // tx.begin();
        // service.execute();
        // tx.commit();
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        // tx.rollback();
        tmp::ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_MyBusinessService_M1()
{
    // 疑似的な確認... Tx の手動操作。
    auto pool = ObjectPool<mysqlx::Session>::create("mysql");
    pool->push(std::make_unique<mysqlx::Session>(mysqlx::Session("localhost", 33060, "root", "root1234")));
    auto sess = pool->pop();
    tmp::mysql::r3::MySqlTransaction tx(sess.get());
    try {
        tmp::mysql::r3::VarNodeRepository<uint64_t> insert_repo{sess.get(), "test", "contractor"};
        tmp::mysql::r3::VarNodeRepository<uint64_t> update_repo{sess.get(), "test", "contractor"};
        tmp::mysql::r3::VarNodeRepository<uint64_t> find_repo{sess.get(), "test", "contractor"};
        tmp::mysql::r3::VarNodeRepository<uint64_t> remove_repo{sess.get(), "test", "contractor"};
        MyBusinessService service(std::move(insert_repo), std::move(update_repo), std::move(find_repo), std::move(remove_repo));
        tx.begin();
        service.execute();
        tx.commit();
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        tx.rollback();
        tmp::ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}
//
// 以下はもう利用しない。
// 
using Data = tmp::VarNode;
using ID = uint64_t;
struct InsertOp
{
    Data* const data;
    InsertOp(Data* const _data): data{_data}
    {}
};
struct UpdateOp
{
    ID id;
    Data* data;
    UpdateOp(const ID& _id, Data* const _data): id{_id}, data{_data}
    {}
};
struct FindByIdOp
{
    ID id;
    FindByIdOp(const ID& _id): id{_id}
    {}
};
struct RemoveOp
{
    ID id;
    RemoveOp(const ID& _id): id{_id}
    {}
};
class RepoExecutor {
private:
    tmp::Repository<ID, Data>* const repo;
public:
    RepoExecutor(tmp::Repository<ID, Data>*  const _repo): repo{_repo}
    {}
    // 戻り値を包括するため、共通の Result 型を定義するか、std::any 等を利用
    auto operator()(InsertOp& op) { return repo->insert(std::move(*(op.data))); }
    void operator()(UpdateOp& op) { repo->update(op.id, std::move(*(op.data))); }
    auto operator()(FindByIdOp& op) { return repo->findById(op.id); }
    void operator()(RemoveOp& op) { repo->remove(op.id); }
    // ...
};

// TODO ... 次の課題、RepoExecutor& を template <class... Repos> として複数の任意のリポジトリ実行ができるのか検証する。
// Tx ラッピング関数。Insert の実行。
auto execute_transaction(tmp::Transaction& tx, RepoExecutor& repoExec, InsertOp& op) {
    try {
        tx.begin();
        auto ret = repoExec(op); // ビジネスロジックの実行
        tx.commit();
        return ret;
    } catch (...) {
        tx.rollback();
        throw; // 例外を再送出し、呼び出し側に通知
    }
}

// Tx ラッピング関数。Update の実行。
void execute_transaction(tmp::Transaction& tx, RepoExecutor& repoExec, UpdateOp& op) {
    try {
        tx.begin();
        repoExec(op);
        tx.commit();
    } catch (...) {
        tx.rollback();
        throw;
    }
}

// Tx ラッピング関数。FindById の実行（find はトランザクションをはる必要は本来ないが、他と合わせている）。
auto execute_transaction(tmp::Transaction& tx, RepoExecutor& repoExec, FindByIdOp& op) {
    try {
        tx.begin();
        auto ret = repoExec(op);
        tx.commit();
        return ret;
    } catch (...) {
        tx.rollback();
        throw;
    }
}
// Tx ラッピング関数。Remove の実行。
void execute_transaction(tmp::Transaction& tx, RepoExecutor& repoExec, RemoveOp& op) {
    try {
        tx.begin();
        repoExec(op);
        tx.commit();
    } catch (...) {
        tx.rollback();
        throw;
    }
}

int test_RepoExecutor_Insert(uint64_t* id)
{
    puts("------ test_RepoExecutor_Insert");
    // ObjectPool のインスタンス化
    auto pool = ObjectPool<mysqlx::Session>::create("mysql");
    try {
        pool->push(std::make_unique<mysqlx::Session>(mysqlx::Session("localhost", 33060, "root", "root1234")));
        auto sess = pool->pop();
        tmp::mysql::r3::MySqlTransaction tx(sess.get());
        std::unique_ptr<tmp::Repository<uint64_t, Data>> irepo
            = std::make_unique<tmp::mysql::r3::VarNodeRepository<uint64_t>>(sess.get(), "test", "contractor");
        // データベース上でNULL を許可しているカラムは設定しなくてよい（contractor table では roles にあたる）。
        // root ノードのKey とValue は空でなければ何でもよい（現状は）。
        std::string pkcol{"id"};
        Data root{"primaryKey", pkcol};
        std::string companyId{"B3_1000"};
        root.addChild("company_id", companyId);
        std::string email{"alice@loki.org"};
        root.addChild("email", email);
        std::string password{"alice1111"};
        root.addChild("password", password);
        std::string name{"Alice"};
        root.addChild("name", name);

        RepoExecutor exec{irepo.get()};
        InsertOp op{&root};
        *id = execute_transaction(tx, exec, op);
        tmp::print_debug("id: ", *id);
        if(!(*id)) throw std::runtime_error("Unexpected case.");
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        tmp::ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_RepoExecutor_Update(uint64_t* id)
{
    puts("------ test_RepoExecutor_Update");
    // ObjectPool のインスタンス化
    auto pool = ObjectPool<mysqlx::Session>::create("mysql");
    try {
        pool->push(std::make_unique<mysqlx::Session>(mysqlx::Session("localhost", 33060, "root", "root1234")));
        auto sess = pool->pop();
        tmp::mysql::r3::MySqlTransaction tx(sess.get());
        std::unique_ptr<tmp::Repository<uint64_t, Data>> irepo
            = std::make_unique<tmp::mysql::r3::VarNodeRepository<uint64_t>>(sess.get(), "test", "contractor");
        // データベース上でNULL を許可しているカラムは設定しなくてよい（contractor table では roles にあたる）。
        // root ノードのKey とValue は空でなければ何でもよい（現状は）。
        std::string pkcol{"id"};
        Data root{"primaryKey", pkcol};
        std::string companyId{"B3_3333"};
        root.addChild("company_id", companyId);
        std::string email{"alice_uloki.org"};
        root.addChild("email", email);
        std::string password{"alice3333"};
        root.addChild("password", password);
        std::string name{"Alice_U"};
        root.addChild("name", name);
        std::string role{"Admin,User"};
        root.addChild("roles", role);

        RepoExecutor exec{irepo.get()};
        UpdateOp op{*id, &root};
        execute_transaction(tx, exec, op);
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        tmp::ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_RepoExecutor_FindById(uint64_t* id)
{
    puts("------ test_RepoExecutor_FindById");
    // ObjectPool のインスタンス化
    auto pool = ObjectPool<mysqlx::Session>::create("mysql");
    try {
        pool->push(std::make_unique<mysqlx::Session>(mysqlx::Session("localhost", 33060, "root", "root1234")));
        auto sess = pool->pop();
        tmp::mysql::r3::MySqlTransaction tx(sess.get());
        std::unique_ptr<tmp::Repository<uint64_t, Data>> irepo
            = std::make_unique<tmp::mysql::r3::VarNodeRepository<uint64_t>>(sess.get(), "test", "contractor");

        tmp::print_debug("id: ", *id);
        RepoExecutor exec{irepo.get()};
        FindByIdOp op{*id};
        std::optional<Data> node = execute_transaction(tx, exec, op);
        if(!node) throw std::runtime_error("Unexpected case.");
        else tmp::debug_print_varnode(&node.value());
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        tmp::ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_RepoExecutor_Remove(uint64_t* id)
{
    puts("------ test_RepoExecutor_Remove");
    // ObjectPool のインスタンス化
    auto pool = ObjectPool<mysqlx::Session>::create("mysql");
    try {
        pool->push(std::make_unique<mysqlx::Session>(mysqlx::Session("localhost", 33060, "root", "root1234")));
        auto sess = pool->pop();
        tmp::mysql::r3::MySqlTransaction tx(sess.get());
        std::unique_ptr<tmp::Repository<uint64_t, Data>> irepo
            = std::make_unique<tmp::mysql::r3::VarNodeRepository<uint64_t>>(sess.get(), "test", "contractor");

        tmp::print_debug("id: ", *id);
        RepoExecutor exec{irepo.get()};
        RemoveOp op{*id};
        execute_transaction(tx, exec, op);
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
    if(0) {
        uint64_t id = 0;
        tmp::print_debug("Play and Result ... ", ret = test_RepoExecutor_Insert(&id));
        assert(ret == 0);
        tmp::print_debug("Play and Result ... ", ret = test_RepoExecutor_Update(&id));
        assert(ret == 0);
        tmp::print_debug("Play and Result ... ", ret = test_RepoExecutor_FindById(&id));
        assert(ret == 0);
        tmp::print_debug("Play and Result ... ", ret = test_RepoExecutor_Remove(&id));
        assert(ret == 0);
    }
    if(1) {
        tmp::print_debug("Play and Result ... ", ret = test_MyBusinessService_M1());
        assert(ret == 0);
        tmp::print_debug("Play and Result ... ", ret = test_execute_service_with_tx_M1());
        assert(ret == 0);
    }
    puts("=== END main ");
    return EXIT_SUCCESS;
}