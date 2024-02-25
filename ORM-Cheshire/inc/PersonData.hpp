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
public:
    PersonData(std::unique_ptr<RdbStrategy<PersonData>> _strategy
    , const DataField<std::string>& _name
    , const DataField<std::string>& _email 
    , const DataField<int>& _age);

    PersonData(std::unique_ptr<RdbStrategy<PersonData>> _strategy
    , const DataField<std::string>& _name
    , const std::optional<DataField<std::string>>& _email 
    , const std::optional<DataField<int>>& _age);

    // ..
    virtual std::vector<std::string> getColumns() const override;

    const std::string                     getTableName() const;
    DataField<std::size_t>                getId() const;
    DataField<std::string>                getName() const;
    std::optional<DataField<std::string>> getEmail() const;
    std::optional<DataField<int>>         getAge() const;
private:
    const std::string TABLE_NAME;
    std::unique_ptr<RdbStrategy<PersonData>> strategy = nullptr;
    DataField<std::size_t> id;
    DataField<std::string> name;
    // 必須ではないデータは optional を利用するといいかもしれない。
    std::optional<DataField<std::string>> email;
    std::optional<DataField<int>>         age;
};

#endif
