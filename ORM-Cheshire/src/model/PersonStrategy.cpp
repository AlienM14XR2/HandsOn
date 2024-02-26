#include "../../inc/PersonStrategy.hpp"

std::vector<std::string> PersonStrategy::getColumns(const PersonData& data) const {         // override
    puts("------ PersonStrategy::getColumns");
    std::vector<std::string> cols;
    // auto[id_name, id_value] = data.getId().bind();   // TODO プライマリキの Auto Increment あり／なし の判断が必要。 
                                                        // そればバリエーションポイントなので 別 Strategy になるかな。
    // Nullable の概念は必要かもしれない。
    auto[name_name, name_value] = data.getName().bind();
    cols.emplace_back(name_name);
    auto[email_name, email_value] = data.getEmail().bind();
    cols.emplace_back(email_name);
    if(data.getAge().has_value()) {
        auto[age_name, age_value] = data.getAge().value().bind();
        cols.emplace_back(age_name);
    }
    return cols;
}

std::vector<std::tuple<std::string, std::string, std::string>> PersonStrategy::getTableInfo(const PersonData& data) const {   // override
    puts("------ PersonStrategy::getTableInfo");
    try {
        std::vector<std::tuple<std::string, std::string, std::string>> vec;
        vec.emplace_back(data.getId().bindTupleTblInfo());
        vec.emplace_back(data.getName().bindTupleTblInfo());
        vec.emplace_back(data.getEmail().bindTupleTblInfo());
        vec.emplace_back(data.getAge().value().bindTupleTblInfo());
        return vec;
    } catch(std::exception& e) {
        throw std::runtime_error(e.what());
    }
}
