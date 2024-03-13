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
#include "/usr/include/mysql-cppconn-8/mysql/jdbc.h"
#include "/usr/include/mysql-cppconn-8/mysqlx/xdevapi.h"

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

int test_insert_person();
int test_PersonRepository_findOne();
int test_PersonRepository_update();
int test_PersonRepository_insert();
int test_PersonRepository_insert_no_age();
int test_PersonRepository_remove();

#endif
