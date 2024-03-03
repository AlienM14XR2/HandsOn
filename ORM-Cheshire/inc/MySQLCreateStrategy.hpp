#ifndef _MYSQLCREATESTRATEGY_H_
#define _MYSQLCREATESTRATEGY_H_

#include <optional>

/**
 * MySQLCreateStrategy クラス
 * 
 * Create（Insert） を行う。
*/

template <class DATA, class PKEY>
class MySQLCreateStrategy final : public RdbProcStrategy<DATA> {
public:
    MySQLCreateStrategy(const Repository<DATA,PKEY>* _repo, const DATA& _data): repo(_repo), data(_data) 
    {}
    virtual std::optional<DATA> proc() const override {
        puts("------ MySQLCreateStrategy::proc");
        try {
            return repo->insert(data);
        } catch(std::exception& e) {
            throw std::runtime_error(e.what());
        }
    }
private:
    const Repository<DATA,PKEY>* repo;
    DATA data;
};

#endif
