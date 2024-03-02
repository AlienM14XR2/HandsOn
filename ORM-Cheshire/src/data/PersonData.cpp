#include "../../inc/PersonData.hpp"

PersonData::PersonData(std::unique_ptr<RdbStrategy<PersonData>> _strategy
    , const DataField<std::string>& _name
    , const DataField<std::string>& _email 
    , const DataField<int>& _age) 
    : TABLE_NAME{std::move(std::string("person"))}
    , strategy{std::move(_strategy)}, id{std::move(DataField<std::size_t>("id",0))}, name{_name}, email{_email}, age{_age}
{
    // 必要ならここで Validation を行う、妥当性検証のオブジェクトをコンポジションして利用するのもあり。
}
PersonData::PersonData(std::unique_ptr<RdbStrategy<PersonData>> _strategy
    , const DataField<std::size_t>& _id
    , const DataField<std::string>& _name
    , const DataField<std::string>& _email 
    , const DataField<int>& _age)
    : TABLE_NAME{std::move(std::string("person"))}
    , strategy{std::move(_strategy)}, id{_id}, name{_name}, email{_email}, age{_age}
{

}
PersonData::PersonData(std::unique_ptr<RdbStrategy<PersonData>> _strategy
    , const DataField<std::string>& _name
    , const DataField<std::string>& _email 
    , const std::optional<DataField<int>>& _age) 
    : TABLE_NAME{std::move(std::string("person"))}
    , strategy{std::move(_strategy)}, id{std::move(DataField<std::size_t>("id",0))}, name{_name}, email{_email}, age{_age}
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

// ... 
std::vector<std::string> PersonData::getColumns() const {   // override
    puts("------ PersonData::getColums");
    return strategy.get()->getColumns(*this);
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
std::unique_ptr<RdbStrategy<PersonData>>& PersonData::getDataStrategy() { return strategy; }
