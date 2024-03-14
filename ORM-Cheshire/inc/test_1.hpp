#ifndef _TEST_1_H_
#define _TEST_1_H_

#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include <memory>
#include <optional>
#include <set>
#include <chrono>
#include "../inc/Debug.hpp"
#include "../inc/DataField.hpp"
#include "../inc/RdbDataStrategy.hpp"
#include "../inc/PersonStrategy.hpp"
#include "../inc/PersonData.hpp"
#include "../inc/MySQLDriver.hpp"
#include "../inc/ConnectionPool.hpp"
#include "../inc/sql_generator.hpp"
#include "../inc/PersonRepository.hpp"
#include "../inc/RdbProcStrategy.hpp"
#include "../inc/MySQLCreateStrategy.hpp"
#include "../inc/MySQLReadStrategy.hpp"
#include "../inc/MySQLUpdateStrategy.hpp"
#include "../inc/MySQLDeleteStrategy.hpp"
#include "../inc/MySQLTx.hpp"                       // src 相対にしている
#include "/usr/include/mysql-cppconn-8/mysql/jdbc.h"
#include "/usr/include/mysql-cppconn-8/mysqlx/xdevapi.h"
#include "/usr/local/include/pqxx/pqxx"

int test_DataField();
int test_DataField_2();
int test_DataField_3();
int test_PersonData();
int test_makeInsertSql();
int test_makeUpdateSql();
int test_makeDeleteSql();
int test_makeFindOneSql();
int test_makeCreateTableSql();
int test_MySQLDriver();

// int test_mysql_connect();
int test_ConnectionPool();

// extern    ConnectionPool<sql::Connection> app_cp;
void mysql_connection_pool(const std::string& server, const std::string& user, const std::string& password, const int& sum);
int test_mysql_connection_pool_A();
int test_mysql_connection_pool_B();
int test_mysql_connect();
int test_MySQLTx();
int test_MySQLTx_rollback();
int test_MySQLTx_Create(std::size_t* insId);
int test_MySQLTx_Read(std::size_t* insId);
int test_MySQLTx_Update(std::size_t* insId);
int test_MySQLTx_Delete(std::size_t* insId);

int test_mysqlx_connect();
int test_pqxx_connect();
int test_pqxx_insert();
int test_pqxx_resultset();

int test_insert_person();
int test_PersonRepository_findOne();
int test_PersonRepository_update();
int test_PersonRepository_insert();
int test_PersonRepository_insert_no_age();
int test_PersonRepository_remove();

#endif
