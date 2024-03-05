#ifndef _MYSQLDELETESTRATEGY_H_
#define _MYSQLDELETESTRATEGY_H_

#include "RdbProcStrategy.hpp"
#include "Repository.hpp"
#include <optional>

/**
 * MySQLDeleteStrategy クラス
 * 
 * Read（Select） を行う。
*/

template <class DATA, class PKEY>
class MySQLDeleteStrategy final : public RdbProcStrategy<DATA> {
public:
    MySQLDeleteStrategy(const Repository<DATA,PKEY>* _repo, const PKEY& _pkey)
    : repo(_repo)
    , pkey(_pkey)
    {}
    virtual std::optional<DATA> proc() const override {
        puts("------ MySQLDeleteStrategy::proc");
        try {
            repo->remove(pkey);
            return std::nullopt;
        } catch(std::exception& e) {
            throw std::runtime_error(e.what());
        }
    }
private:
    const Repository<DATA,PKEY>* repo;
    PKEY pkey;
};

#endif
