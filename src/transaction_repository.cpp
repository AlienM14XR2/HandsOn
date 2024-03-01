/**
 * トランザクションとリポジトリを如何に抽象化できるか。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror transaction_repository.cpp -o ../bin/main
*/
#include <iostream>
#include <cassert>
#include <memory>
#include "/usr/include/mysql-cppconn-8/mysql/jdbc.h"


template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](const auto message, const auto debug) -> void {
    std::cout << "DEBUG: " << message << '\t' << debug << std::endl;
};

template <class Error>
concept ErrReasonable = requires(Error& e) {
    e.what();
};
template <class Error>
requires ErrReasonable<Error>
void (*ptr_print_error)(Error) = [](const auto e) -> void {
    std::cout << "ERROR: " << e.what() << std::endl;
};

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

// std::unique_ptr<sql::PreparedStatement> prep_stmt(con->prepareStatement(sql));

class Connection {
public:
    void setAutoCommit(const bool& b) {
        puts("------ Connection::setAutoCommit");
        try {

        } catch(std::exception& e) {
            throw std::runtime_error(e.what());
        }
    }
    void commit() {
        puts("------ Connection::commit");
        try {

        } catch(std::exception& e) {
            throw std::runtime_error(e.what());
        }
    }
    void rollback() {
        puts("------ Connection::rollback");
        try {

        } catch(std::exception& e) {
            throw std::runtime_error(e.what());
        }
    }
    void prepareStatement(const std::string& sql) {       // 本来は PreparedStatement のポインタを返却するもの
        puts("------ Connection::prepareStatement");
        try {

        } catch(std::exception& e) {
            throw std::runtime_error(e.what());
        }
    }
};

class Widget {
public:
    Widget(const int& _id): id(_id)
    {}

    int getId() const { return id; }
    void setId(const int& _id) { id = _id; }
private:
    int id;
};

void fx(const Widget& src, Widget& dest) {
    dest.setId(src.getId() + 1);
}

int test_fx() {
    puts("=== test_fx");
    try {
        Widget src(1);
        Widget dest(0);
        ptr_lambda_debug<const char*, const int&>("before fx ... dest id is ", dest.getId());
        fx(src, dest);
        ptr_lambda_debug<const char*, const int&>("after  fx ... dest id is ", dest.getId());
        assert(dest.getId() == 2);
        /**
         * 参照を上手く利用すれば、戻り値としてオブジェクトのコピーを返却する必要はない。
        */
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

template <class DATA, class PKEY>
class Repository {
public:
    virtual ~Repository() = default;
    // ...
    virtual DATA insert(const DATA&)  const = 0;
    virtual DATA update(const DATA&)  const = 0;
    virtual void remove(const PKEY&)  const = 0;
    virtual DATA findOne(const PKEY&) const = 0;
};

class WidgetRepository final : public Repository<Widget,int> {
public:
    WidgetRepository(Connection* _con, const Widget& _w) : con(_con), widget(_w)        // 本来は SQL を発行する DATA が必要、この場合 Widget にそのインタフェースが必要という意味になる。
    {}
    // ...
    virtual Widget insert(const Widget& w) const override {
        puts("------ WidgetRepository::insert");
        con->prepareStatement("INSERT INTO ...");
        Widget result(1);
        return result;
    }
    virtual Widget update(const Widget&) const override {
        puts("------ WidgetRepository::update");
        con->prepareStatement("UPDATE ...");
        Widget result(2);
        return result;
    }
    virtual void remove(const int& pkey) const override {
        puts("------ WidgetRepository::remove");
        con->prepareStatement("DELETE ...");
    }
    virtual Widget findOne(const int& pkey) const override {
        puts("------ WidgetRepository::findOne");
        con->prepareStatement("SELECT ...");
        Widget result(3);
        return result;
    }
private:
    Connection* con;
    Widget widget;
};

/**
 * リポジトリは DATA の戻り値を必要とするメンバ関数と戻り値なしが混在する。
 * Tx はリポジトリの詳細とは無縁でありたい。この両者を proc() だけで上手く
 * 解決したい。
*/

class Transaction {
public:
    virtual ~Transaction() = default;
    // ...
    void executeTx() const {
        try {
            begin();
            proc();         // これが バリエーション・ポイント
            commit();
        } catch(std::exception& e) {
            rollback();
            ptr_print_error<const decltype(e)&>(e);
            throw std::runtime_error(e.what());
        }
    }
    virtual void begin()    const = 0;
    virtual void commit()   const = 0;
    virtual void rollback() const = 0;
    virtual void proc()     const = 0;
};

class RdbProcStrategy {
public:
    virtual ~RdbProcStrategy() = default;
    virtual void proc() const = 0;
};

template <class DATA, class PKEY>
class MySQLCreateStrategy final : public RdbProcStrategy {
public:
    MySQLCreateStrategy(const Repository<DATA,PKEY>* _repo, const DATA& _data): repo(_repo), data(_data) 
    {}
    virtual void proc() const override {
        puts("------ MySQLCreateStrategy::proc");
        try {
            repo->insert(data);
        } catch(std::exception& e) {
            throw std::runtime_error(e.what());
        }
    }
private:
    const Repository<DATA,PKEY>* repo;
    DATA data;
};

/**
 * 上例のように CRUD に対応した RdbProcStrategy の派生クラスを定義することで Tx に関する煩雑な処理を
 * 一元管理できると考えた設計になっている。確認したいことは、これが本当にスレッドセーフなのかという点。
 * 実際のコネクションはおそらくスレッドセーフではないと思う、コネクションの共有以外の点で別途確認して
 * みたい。
*/

template <class DATA, class PKEY>
class MySQLErrorStrategy final : public RdbProcStrategy {
public:
    MySQLErrorStrategy(const Repository<DATA,PKEY>* _repo, const DATA& _data): repo(_repo), data(_data) 
    {}
    virtual void proc() const override {
        puts("------ MySQLErrorStrategy::proc");
        try {
            repo->insert(data);
            throw std::runtime_error("It's test error.");
        } catch(std::exception& e) {
            throw std::runtime_error(e.what());
        }
    }
private:
    const Repository<DATA,PKEY>* repo;
    DATA data;
};


class MySQLTx final : public Transaction {
public:
    MySQLTx(Connection* _con, const RdbProcStrategy* _strategy): con(_con), strategy(_strategy)
    {}
    virtual void begin()    const override {
        con->setAutoCommit(false);
    }
    virtual void commit()   const override {
        con->commit();
    }
    virtual void rollback() const override {
        con->rollback();
    }
    virtual void proc()     const override {
        strategy->proc();
    }

private:
    Connection* con;
    const RdbProcStrategy* strategy;
};


int test_MySQLCreateStrategy() {
    puts("=== test_MySQLCreateStrategy");
    Connection* con = nullptr;
    try {
        con = new Connection();
        Widget w(1);
        std::unique_ptr<Repository<Widget,int>> repo = std::make_unique<WidgetRepository>(WidgetRepository(con, w));
        MySQLCreateStrategy create(repo.get(), w);
        puts("--- 派生クラス MySQLCreateStrategy で直接操作する");
        create.proc();
        /**
         * 上記は直接 RdbProcStrategy の派生クラスを操作している。
         * これではまだ不完全だな、MySQLCreateStrategy の基底クラス（インタフェース）である RdbProcStrategy で操作できないといけない。
        */
        puts("--- 基底クラス RdbProcStrategy で間接的に接操作する");
        std::unique_ptr<RdbProcStrategy> strategy = std::make_unique<MySQLCreateStrategy<Widget,int>>(MySQLCreateStrategy(repo.get(), w));
        strategy->proc();

        if(con) {
            delete con;
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        if(con) {
            delete con;
        }
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_MySQLTx_Insert() {
    puts("=== test_MySQLTx_Insert");
    Connection* con = nullptr;
    /**
     * 以下の処理は同一のコネクションを利用すること。
    */
    try {
        con = new Connection();
        Widget w(1);
        std::unique_ptr<Repository<Widget,int>> repo = std::make_unique<WidgetRepository>(WidgetRepository(con, w));
        std::unique_ptr<RdbProcStrategy> strategy = std::make_unique<MySQLCreateStrategy<Widget,int>>(MySQLCreateStrategy(repo.get(), w));
        MySQLTx tx(con, strategy.get());
        tx.executeTx();
        if(con) {
            delete con;
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        if(con) {
            delete con;
        }
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int test_MySQLTx_Insert_Rollback() {
    puts("=== test_MySQLTx_Insert_Rollback");
    Connection* con = nullptr;
    try {
        con = new Connection();
        Widget w(1);
        std::unique_ptr<Repository<Widget,int>> repo = std::make_unique<WidgetRepository>(WidgetRepository(con, w));
        std::unique_ptr<RdbProcStrategy> strategy = std::make_unique<MySQLErrorStrategy<Widget,int>>(MySQLErrorStrategy(repo.get(), w));
        MySQLTx tx(con, strategy.get());
        tx.executeTx();
        if(con) {
            delete con;
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        if(con) {
            delete con;
        }
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int main(void) {
    puts("START トランザクションとリポジトリを如何に抽象化できるか ===");
    if(0.01) {
        auto ret = 0;
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", ret = test_debug_and_error());
        assert(ret == 1);
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", ret = test_fx());
        assert(ret == 0);
    }
    if(1.00) {
        auto ret = 0;
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", ret = test_MySQLCreateStrategy());
        assert(ret == 0);
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", ret = test_MySQLTx_Insert());
        assert(ret == 0);
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", ret = test_MySQLTx_Insert_Rollback());
        assert(ret == 1);
    }
    puts("===   トランザクションとリポジトリを如何に抽象化できるか END");
    return 0;
}