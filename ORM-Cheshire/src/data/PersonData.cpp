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
    , const DataField<std::string>& _name
    , const std::optional<DataField<std::string>>& _email 
    , const std::optional<DataField<int>>& _age) 
    : TABLE_NAME{std::move(std::string("person"))}
    , strategy{std::move(_strategy)}, id{std::move(DataField<std::size_t>("id",0))}, name{_name}, email{_email}, age{_age}
{
    // 必要ならここで Validation を行う、妥当性検証のオブジェクトをコンポジションして利用するのもあり。
}

// ... 
std::vector<std::string> PersonData::getColumns() const {   // override
    puts("------ PersonData::getColums");
    return strategy.get()->getColumns(*this);
}

std::vector<std::pair<std::string, std::string>> PersonData::getTableInfo() const { // override
    puts("TODO 実装 ------ PersonData::getTableInfo");
    std::vector<std::pair<std::string, std::string>> vec;
    // TODO 実装詳細は Strategy パターンの具象クラスで行うこと。
    return vec;
}


const std::string                     PersonData::getTableName()  const { return TABLE_NAME; }
DataField<std::size_t>                PersonData::getId()         const { return id; }
DataField<std::string>                PersonData::getName()       const { return name; }
std::optional<DataField<std::string>> PersonData::getEmail()      const { return email; }
std::optional<DataField<int>>         PersonData::getAge()        const { return age; }