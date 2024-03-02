#ifndef _PERSONDATA_H_
#define _PERSONDATA_H_

#include <memory>
#include <string>
#include <vector>
#include <optional>
#include "RdbData.hpp"
#include "DataField.hpp"
#include "RdbStrategy.hpp"

class PersonData final : public RdbData {
private:
    // ダミーとして使うこと
    PersonData();
public:
    PersonData(std::unique_ptr<RdbStrategy<PersonData>> _strategy
    , const DataField<std::string>& _name
    , const DataField<std::string>& _email 
    , const DataField<int>& _age);

    PersonData(std::unique_ptr<RdbStrategy<PersonData>> _strategy
    , const DataField<std::size_t>& _id
    , const DataField<std::string>& _name
    , const DataField<std::string>& _email 
    , const DataField<int>& _age);

    PersonData(std::unique_ptr<RdbStrategy<PersonData>> _strategy
    , const DataField<std::string>& _name
    , const DataField<std::string>& _email 
    , const std::optional<DataField<int>>& _age);

    static PersonData dummy();

    // ..
    virtual std::vector<std::string> getColumns() const override;
    virtual std::vector<std::tuple<std::string, std::string, std::string>> getTableInfo() const override;

    const std::string                     getTableName() const;
    DataField<std::size_t>                getId() const;
    DataField<std::string>                getName() const;
    DataField<std::string>                getEmail() const;
    std::optional<DataField<int>>         getAge() const;
private:
    const std::string TABLE_NAME;
    std::unique_ptr<RdbStrategy<PersonData>> strategy = nullptr;
    DataField<std::size_t> id;          // 必須
    DataField<std::string> name;        // 必須
    DataField<std::string> email;       // 必須
    // 必須ではないデータは optional を利用するといいかもしれない。
    std::optional<DataField<int>>         age;
};

#endif
