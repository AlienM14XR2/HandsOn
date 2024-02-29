#ifndef _MYSQLDRIVER_H_
#define _MYSQLDRIVER_H_

#include "./RdbDriver.hpp"
#include "/usr/include/mysql-cppconn-8/mysql/jdbc.h"

class MySQLDriver final : public RdbDriver<sql::Driver> {
public:
    static MySQLDriver& getInstance();
    virtual sql::Driver* getDriver() const override; 
private:
    MySQLDriver();
    MySQLDriver(const MySQLDriver&)            = delete;
    MySQLDriver& operator=(const MySQLDriver&) = delete;
    MySQLDriver(MySQLDriver&&)                 = delete;
    MySQLDriver& operator=(MySQLDriver&&)      = delete;
    // ...
    sql::Driver* driver = nullptr;
};

#endif
