/**
 * トランザクションとリポジトリを如何に抽象化できるか。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror transaction_repository.cpp -o ../bin/main
*/
#include <iostream>
#include <cassert>
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
    virtual DATA insert(const DATA&)  = 0;
    virtual DATA update(const DATA&)  = 0;
    virtual void remove(const PKEY&)  = 0;
    virtual DATA findOne(const PKEY&) = 0;
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
    void executeTx() {
        try {
            begin();
            proc();         // これが バリエーション・ポイント
            commit();
        } catch(std::exception& e) {
            rollback();
            ptr_print_error<const decltype(e)&>(e);
            throw e;
        }
    }
    virtual void begin()    = 0;
    virtual void commit()   = 0;
    virtual void rollback() = 0;
    virtual void proc()     = 0;
};

class MySQLTx final : public Transaction {

};

class RdbProcStrategy {
public:
    virtual ~RdbProcStrategy() = default;
    virtual void proc() = 0;
};

class MySQLCreateStrategy final : public RdbProcStrategy {

};

int main(void) {
    puts("START トランザクションとリポジトリを如何に抽象化できるか ===");
    if(0.01) {
        auto ret = 0;
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", ret = test_debug_and_error());
        assert(ret == 1);
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", ret = test_fx());
        assert(ret == 0);
    }
    puts("===   トランザクションとリポジトリを如何に抽象化できるか END");
    return 0;
}