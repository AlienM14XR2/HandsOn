#include "../../inc/MySQLConnection.hpp"

/**
 * MySQL Connection の定義
*/

void MySQLConnection::begin() const
{
    puts("------ MySQLConnection::setAutoCommit");
    try {
        con->setAutoCommit(false);
        // begin_();
    } catch(std::exception& e) {
        throw std::runtime_error(e.what());
    }
}
void MySQLConnection::commit() const
{
    puts("------ MySQLConnection::commit");
    try {
        con->commit();
    } catch(std::exception& e) {
        throw std::runtime_error(e.what());
    }
}
void MySQLConnection::rollback() const
{
    puts("------ MySQLConnection::rollback");
    try {
        con->rollback();
    } catch(std::exception& e) {
        throw std::runtime_error(e.what());
    }
}
sql::PreparedStatement* MySQLConnection::prepareStatement(const std::string& sql) const
{
    puts("------ MySQLConnection::prepareStatement");
    try {
        return con->prepareStatement(sql);
    } catch(std::exception& e) {
        throw std::runtime_error(e.what());
    }
}
sql::Statement* MySQLConnection::createStatement() const
{
    puts("------ MySQLConnection::createStatement");
    try {
        return con->createStatement();
    } catch(std::exception& e) {
        throw std::runtime_error(e.what());
    }
}
