#ifndef MYSQLXTX_H_
#define MYSQLXTX_H_

#include "RdbTransaction.hpp"
#include "RdbProcStrategy.hpp"
#include "/usr/include/mysql-cppconn-8/mysqlx/xdevapi.h"

namespace ormx {
template <class DATA>
class MySQLXTx final : public RdbTransaction<DATA> {
public:
    MySQLXTx(mysqlx::Session* _session, const RdbProcStrategy<DATA>* _strategy): session(_session), strategy(_strategy)
    {}
    // ...
    virtual void begin() const override {
        puts("------ ormx::MySQLXTx::begin()");
        session->startTransaction();
    }
    virtual void commit() const override {
        puts("------ ormx::MySQLXTx::commit()");
        session->commit();
    }
    virtual void rollback() const override {
        puts("------ ormx::MySQLXTx::rollback()");
        session->rollback();
    }
    virtual std::optional<DATA> proc() const override {
        puts("------ ormx::MySQLXTx::proc()");
        return strategy->proc();
    }
private:
    mysqlx::Session* session;
    const RdbProcStrategy<DATA>* strategy;
};
}   // namespace ormx

#endif
