#ifndef _PERSONSTRATEGY_H_
#define _PERSONSTRATEGY_H_

#include <vector>
#include <string>
#include "RdbStrategy.hpp"
#include "PersonData.hpp"

class PersonStrategy final : public RdbStrategy<PersonData> {
public:
    virtual std::vector<std::string> getColumns(const PersonData& data) const override;
    virtual std::vector<std::pair<std::string, std::string>> getTableInfo(const PersonData& data) const override;
};

#endif
