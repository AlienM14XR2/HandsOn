#ifndef MYSQLCONNECTION_H_
#define MYSQLCONNECTION_H_

#include "RdbConnection.hpp"
// 当時のこれでいいでしょ感がすごいな
#include "/usr/include/mysql-cppconn-8/mysql/jdbc.h"

/**
 * MySQL 用コネクション
*/

class MySQLConnection final : public RdbConnection<sql::PreparedStatement> {
public:
    MySQLConnection(sql::Connection* _con): con(_con)
    {}
    // ...
    virtual void begin() const override;
    virtual void commit() const override;
    virtual void rollback() const override;
    virtual sql::PreparedStatement* prepareStatement(const std::string& sql) const override;
    sql::Statement* createStatement() const;
private:
    sql::Connection* con;
    // void begin_() const { con->setAutoCommit(false); }
};

#endif
