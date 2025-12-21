#ifndef MYSQLTX_H_
#define MYSQLTX_H_

#include <optional>
#include "/usr/include/mysql-cppconn-8/mysql/jdbc.h"
#include "RdbTransaction.hpp"

/**
 * MySQLTx クラス
 * 
 * RdbTransaction の派生クラス、MySQL の Tx を担う。 
*/

template <class DATA>
class MySQLTx final : public RdbTransaction<DATA> {
public:
    MySQLTx(RdbConnection<sql::PreparedStatement>* _con, const RdbProcStrategy<DATA>* _strategy): con(_con), strategy(_strategy)
    {}
    // ...
    virtual void begin()    const override {
        con->begin();
    }
    virtual void commit()   const override {
        con->commit();
    }
    virtual void rollback() const override {
        con->rollback();
    }
    virtual std::optional<DATA> proc() const override {
        return strategy->proc();
    }

private:
    const RdbConnection<sql::PreparedStatement>* con;
    const RdbProcStrategy<DATA>* strategy;
};

#endif
