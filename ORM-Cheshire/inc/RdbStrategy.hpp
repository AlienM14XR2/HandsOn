#ifndef _RDBSTRATEGY_H_
#define _RDBSTRATEGY_H_

/**
 * RDB に共通する処理のインタフェース。
 * Strategy パターン。
 * RDB に共通する処理は、本クラスを継承した派生クラスで行うこと。
*/

template<class T>
class RdbStrategy {
public:
    virtual ~RdbStrategy() = default;
    virtual std::vector<std::string> getColumns(const T&) const = 0;
    virtual std::vector<std::pair<std::string, std::string>> getTableInfo(const T&) const {
        std::vector<std::pair<std::string, std::string>> vec;
        return vec;
    }
};

#endif
