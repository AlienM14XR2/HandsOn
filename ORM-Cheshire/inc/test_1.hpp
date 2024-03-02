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
#include "../inc/RdbStrategy.hpp"
#include "../inc/PersonStrategy.hpp"
#include "../inc/PersonData.hpp"
#include "../inc/MySQLDriver.hpp"
#include "../inc/ConnectionPool.hpp"
#include "/usr/include/mysql-cppconn-8/mysql/jdbc.h"
#include "/usr/include/mysql-cppconn-8/mysqlx/xdevapi.h"

std::string makeInsertSql(const std::string& tableName, const std::vector<std::string>& colNames);
std::string makeUpdateSql(const std::string& tableName, const std::string& pkName, const std::vector<std::string>& colNames);
std::string makeDeleteSql(const std::string& tableName, const std::string& pkName);
std::string makeFindOneSql(const std::string& tableName, const std::string& pkeyName, const std::vector<std::string>& colNames);
std::string makeCreateTableSql(const std::string& tableName, const std::vector<std::tuple<std::string,std::string,std::string>>& tblInfos);

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

#endif
