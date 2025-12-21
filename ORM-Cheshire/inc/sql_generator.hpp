#ifndef SQL_GENERATOR_H_
#define SQL_GENERATOR_H_

#include <iostream>
#include <string>
#include <vector>
#include <memory>

std::string makeInsertSql(const std::string& tableName, const std::vector<std::string>& colNames);
std::string makeUpdateSql(const std::string& tableName, const std::string& pkName, const std::vector<std::string>& colNames );
std::string makeDeleteSql(const std::string& tableName, const std::string& pkName);
std::string makeFindOneSql(const std::string& tableName, const std::string& pkeyName, const std::vector<std::string>& colNames);
std::string makeCreateTableSql(const std::string& tableName, const std::vector<std::tuple<std::string,std::string,std::string>>& tblInfos);


#endif
