#ifndef _MYSQLXCREATESTRATEGY_H_
#define _MYSQLXCREATESTRATEGY_H_

#include "Repository.hpp"
#include "RdbProcStrategy.hpp"

namespace ormx {
template<class DATA, class PKEY>
class MySQLXCreateStrategy final : public RdbProcStrategy<DATA> {
public:
    MySQLXCreateStrategy(const Repository<DATA, PKEY>* _repo, const DATA& _data): repo(_repo), data(_data)
    {}
    // ...
    virtual std::optional<DATA> proc() const override {
        puts("------ ormx::MySQLXCreateStrategy::proc()");
        try {
            return repo->insert(data);
        } catch(std::exception& e) {
            throw std::runtime_error(e.what());
        }
    }
private:
    const Repository<DATA, PKEY>* repo;
    DATA data;
};
}   // namespace ormx


#endif
