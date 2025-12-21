#ifndef MYSQLUPDATESTRATEGY_H_
#define MYSQLUPDATESTRATEGY_H_

#include "RdbProcStrategy.hpp"
#include "Repository.hpp"
#include <optional>

/**
 * MySQLUpdateStrategy クラス
 * 
 * Read（Select） を行う。
*/

template <class DATA, class PKEY>
class MySQLUpdateStrategy final : public RdbProcStrategy<DATA> {
public:
    MySQLUpdateStrategy(const Repository<DATA,PKEY>* _repo, const DATA& _data)
    : repo(_repo)
    , data(_data)
    {}
    virtual std::optional<DATA> proc() const override {
        puts("------ MySQLUpdateStrategy::proc");
        try {
            return repo->update(data);
        } catch(std::exception& e) {
            throw std::runtime_error(e.what());
        }
    }
private:
    const Repository<DATA,PKEY>* repo;
    DATA data;
};

#endif
