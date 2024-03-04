#ifndef _PERSONREPOSITORY_H_
#define _PERSONREPOSITORY_H_

#include "Debug.hpp"
#include "Repository.hpp"
#include "PersonData.hpp"
#include "MySQLConnection.hpp"
#include "RdbDataStrategy.hpp"
#include "PersonStrategy.hpp"
#include "sql_generator.hpp"
#include "/usr/include/mysql-cppconn-8/mysql/jdbc.h"
#include <optional>
#include <memory>

/**
 * PersonRepository クラス
 * 
 * PersonData の CRUD を実現する。
*/

class PersonRepository final : public Repository<PersonData,std::size_t> {
public:
    PersonRepository(const MySQLConnection* _con);
    // ...
    virtual std::optional<PersonData> insert(const PersonData& data) const override;
    virtual std::optional<PersonData> update(const PersonData& data) const override;
    virtual void remove(const std::size_t& pkey) const override;
    virtual std::optional<PersonData> findOne(const std::size_t& pkey) const;
private:
    const MySQLConnection* con;
};

#endif
