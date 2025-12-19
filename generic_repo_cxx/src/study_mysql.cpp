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



class Transaction
{
public:
    virtual ~Transaction() = default;
    virtual void begin() = 0;
    virtual void commit() = 0;
    virtual void rollback() = 0;
};

class MySqlTransaction final : public Transaction {
private:
    mysqlx::Session* const tx;
public:
    MySqlTransaction(mysqlx::Session* const _tx): tx{_tx}
    {}
    void begin() override
    {
        int status;
        tmp::print_debug("tx begin ... ", abi::__cxa_demangle(typeid(*this).name(),0,0,&status));
        tx->startTransaction();
    }
    void commit() override
    {
        int status;
        tmp::print_debug("tx commit ... ", abi::__cxa_demangle(typeid(*this).name(),0,0,&status));
        tx->commit();
    }
    void rollback() override
    {
        int status;
        tmp::print_debug("tx rollback ... ", abi::__cxa_demangle(typeid(*this).name(),0,0,&status));
        tx->rollback();
    }
};

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
class RepoExecutor {
private:
    tmp::Repository<ID, Data>* repo;
public:
    RepoExecutor(tmp::Repository<ID, Data>* _repo): repo{_repo}
    {}
    // 戻り値を包括するため、共通の Result 型を定義するか、std::any 等を利用
    auto operator()(InsertOp& op) { return repo->insert(std::move(*op.data)); }
    auto operator()(UpdateOp& op) { repo->update(op.id, std::move(*op.data)); }
    // ...
};

/**
 * 次のイメージを実装する。
 * 
// セッションは一旦先にに取得Repositry を作る。
auto session = pool->pop(); // ObjectPoolから取得
// このセッションを利用して次のTx を作る。
MySqlTransaction tx(session.get());
using TransactionFunc = std::function<ValueType(RepoExecutor&)>;
// 概念的な実装イメージ
// template<typename Func>
auto execute_transaction(Transaction& tx, TransactionFunc&& func, RepoExecutor& repo_exec) {
	std::variant ret{std::monostate, int64_t, tmp::VarNode};    
    try {
        tx.begin();
        ret = func(repo_exec); // ビジネスロジックの実行
        tx.commit();
        return ret;
    } catch (...) {
        tx.rollback();
        throw; // 例外を再送出し、呼び出し側に通知
    }
}
 */

auto execute_transaction(Transaction& tx, RepoExecutor& repoExec, InsertOp& op) {
    try {
        tx.begin();
        auto ret = repoExec(op); // ビジネスロジックの実行
        tx.commit();
        // if(ret != std::monostate) return static_cast<std::int64_t>(ret);
        return ret;
    } catch (...) {
        tx.rollback();
        throw; // 例外を再送出し、呼び出し側に通知
    }
}

int test_RepoExecutor()
{
    puts("------ test_RepoExecutor");
    // ObjectPool のインスタンス化
    auto pool = ObjectPool<mysqlx::Session>::create("mysql");
    try {
        pool->push(std::make_unique<mysqlx::Session>(mysqlx::Session("localhost", 33060, "root", "root1234")));
        auto sess = pool->pop();
        MySqlTransaction tx(sess.get());
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
        uint64_t id = execute_transaction(tx, exec, op);
        tmp::print_debug("id: ", id);
        if(!id) throw std::runtime_error("Unexpected case.");
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
        tmp::print_debug("Play and Result ... ", ret = test_RepoExecutor());
        assert(ret == 0);
    }
    puts("=== END main ");
    return EXIT_SUCCESS;
}