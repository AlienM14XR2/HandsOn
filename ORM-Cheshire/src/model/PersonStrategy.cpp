#include "../../inc/PersonStrategy.hpp"

std::vector<std::string> PersonStrategy::getColumns(const PersonData& data) const {         // override
    puts("TODO implementation ------ PersonStrategy::getColumns");
    std::vector<std::string> cols;
    // auto[id_name, id_value] = data.getId().bind();   // TODO プライマリキの Auto Increment あり／なし の判断が必要。 
                                                        // そればバリエーションポイントなので 別 Strategy になるかな。
    // Nullable の概念は必要かもしれない。
    auto[name_name, name_value] = data.getName().bind();
    cols.emplace_back(name_name);
    if(data.getEmail().has_value()) {
        auto[email_name, email_value] = data.getEmail().value().bind();
        cols.emplace_back(email_name);
    }
    if(data.getAge().has_value()) {
        auto[age_name, age_value] = data.getAge().value().bind();
        cols.emplace_back(age_name);
    }
    return cols;
}

std::vector<std::pair<std::string, std::string>> PersonStrategy::getTableInfo(const PersonData& data) const {   // override
    puts("TODO 実装 ------ PersonStrategy::getTableInfo");
    std::vector<std::pair<std::string, std::string>> vec;
    return vec;
}
