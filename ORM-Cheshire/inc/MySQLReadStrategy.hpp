#ifndef _MYSQLREADSTRATEGY_H_
#define _MYSQLREADSTRATEGY_H_

#include "RdbProcStrategy.hpp"
#include "Repository.hpp"
#include <optional>

/**
 * MySQLReadStrategy クラス
 * 
 * Read（Select） を行う。
*/

template <class DATA, class PKEY>
class MySQLReadStrategy final : public RdbProcStrategy<DATA> {
public:
    MySQLReadStrategy(const Repository<DATA,PKEY>* _repo, const PKEY& _pkey)
    : repo(_repo)
    , pkey(_pkey)
    {}
    virtual std::optional<DATA> proc() const override {
        puts("------ MySQLReadStrategy::proc");
        try {
            return repo->findOne(pkey);
        } catch(std::exception& e) {
            throw std::runtime_error(e.what());
        }
    }
private:
    const Repository<DATA,PKEY>* repo;
    PKEY pkey;
};

#endif
