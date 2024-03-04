#include "../../inc/PersonData.hpp"

PersonData::PersonData(RdbDataStrategy<PersonData>* _strategy
    , const DataField<std::string>& _name
    , const DataField<std::string>& _email 
    , const DataField<int>& _age) 
    : TABLE_NAME{std::move(std::string("person"))}
    , strategy{_strategy}, id{std::move(DataField<std::size_t>("id",0))}, name{_name}, email{_email}, age{_age}
{
    // 必要ならここで Validation を行う、妥当性検証のオブジェクトをコンポジションして利用するのもあり。
}
PersonData::PersonData(RdbDataStrategy<PersonData>* _strategy
    , const DataField<std::size_t>& _id
    , const DataField<std::string>& _name
    , const DataField<std::string>& _email 
    , const DataField<int>& _age)
    : TABLE_NAME{std::move(std::string("person"))}
    , strategy{_strategy}, id{_id}, name{_name}, email{_email}, age{_age}
{

}
PersonData::PersonData(RdbDataStrategy<PersonData>* _strategy
    , const DataField<std::size_t>& _id
    , const DataField<std::string>& _name
    , const DataField<std::string>& _email 
    , std::optional<DataField<int>>& _age)
    : TABLE_NAME{std::move(std::string("person"))}
    , strategy{std::move(_strategy)}, id{_id}, name{_name}, email{_email}, age{_age}
{
}

PersonData::PersonData(RdbDataStrategy<PersonData>* _strategy
    , const DataField<std::string>& _name
    , const DataField<std::string>& _email 
    , const std::optional<DataField<int>>& _age) 
    : TABLE_NAME{std::move(std::string("person"))}
    , strategy{_strategy}, id{std::move(DataField<std::size_t>("id",0))}, name{_name}, email{_email}, age{_age}
{
    // 必要ならここで Validation を行う、妥当性検証のオブジェクトをコンポジションして利用するのもあり。
}
// デフォルトコンストラクタ、ダミーとして使うこと。
PersonData::PersonData(): strategy(nullptr)
    , id(std::move(DataField<std::size_t>("id", 0ul)))
    , name(std::move(DataField<std::string>("name", "")))
    , email(std::move(DataField<std::string>("email", "")))
    , age(std::nullopt)
{
}

PersonData PersonData::dummy() {
    return PersonData();
}

PersonData PersonData::factory(sql::ResultSet* rs, RdbDataStrategy<PersonData>* strategy) 
{
    auto rs_id    = rs->getUInt64(1);
    auto rs_name  = rs->getString(2);
    auto rs_email = rs->getString(3);
    auto rs_age   = rs->getInt(4);
    DataField<std::size_t> p_id("id", rs_id); 
    DataField<std::string> p_name("name", rs_name);
    DataField<std::string> p_email("email", rs_email);
    std::optional<DataField<int>> p_age;
    if(rs_age) {
        p_age = DataField<int>("age", rs_age);
    }
    PersonData person(strategy, p_id, p_name, p_email, p_age);
    return person;
}

PersonData PersonData::factory(
      std::string _name
    , std::string _email
    , int         _age
    , RdbDataStrategy<PersonData>* _strategy)
{
    DataField<std::string> df_name("name", _name);
    DataField<std::string> df_email("email", _email);
    DataField<int>         df_age("age", _age);
    return PersonData(_strategy, df_name, df_email, df_age);
}


// ... 
std::vector<std::string> PersonData::getColumns() const {   // override
    puts("------ PersonData::getColums");
    return strategy->getColumns(*this);
}

std::vector<std::tuple<std::string, std::string, std::string>> PersonData::getTableInfo() const { // override
    puts("------ PersonData::getTableInfo");
    return strategy->getTableInfo(*this);
}


const std::string                     PersonData::getTableName()  const { return TABLE_NAME; }
DataField<std::size_t>                PersonData::getId()         const { return id; }
DataField<std::string>                PersonData::getName()       const { return name; }
DataField<std::string>                PersonData::getEmail()      const { return email; }
std::optional<DataField<int>>         PersonData::getAge()        const { return age; }
RdbDataStrategy<PersonData>*          PersonData::getDataStrategy() const { return strategy; }
void                                  PersonData::setDataStrategy(RdbDataStrategy<PersonData>* _strategy) { strategy = _strategy; }
void                                  PersonData::setName(DataField<std::string> _name) { name = _name; }
void                                  PersonData::setEmail(DataField<std::string> _email) { email = _email; }
void                                  PersonData::setAge(DataField<int> _age) { age = _age; }
