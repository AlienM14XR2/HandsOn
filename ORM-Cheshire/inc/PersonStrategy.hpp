#ifndef PERSONSTRATEGY_H_
#define PERSONSTRATEGY_H_

#include <vector>
#include <string>
#include "RdbDataStrategy.hpp"
#include "PersonData.hpp"

class PersonStrategy final : public RdbDataStrategy<PersonData> {
public:
    virtual std::vector<std::string> getColumns(const PersonData& data) const override;
    virtual std::vector<std::tuple<std::string, std::string, std::string>> getTableInfo(const PersonData& data) const override;
};

#endif
