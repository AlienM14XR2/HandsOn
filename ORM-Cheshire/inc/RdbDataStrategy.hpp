#ifndef RDBDATASTRATEGY_H_
#define RDBDATASTRATEGY_H_

/**
 * RDB の Data に共通する処理のインタフェース。
 * Strategy パターン。
 * RDB に共通する処理は、本クラスを継承した派生クラスで行うこと。
*/

template<class T>
class RdbDataStrategy {
public:
    virtual ~RdbDataStrategy() = default;
    // ...
    virtual std::vector<std::string> getColumns(const T&) const = 0;
    virtual std::vector<std::tuple<std::string, std::string, std::string>> getTableInfo(const T&) const {
        std::vector<std::tuple<std::string, std::string, std::string>> vec;
        return vec;
    }
};

#endif
