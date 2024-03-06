#ifndef _MYSQLCREATESTRATEGY_H_
#define _MYSQLCREATESTRATEGY_H_

#include <optional>

/**
 * MySQLCreateStrategy クラス
 * 
 * Create（Insert） を行う。
 * 
 * 補足、これは別に Repository と 1 対 1 である必要はなく proc（同一トランザクション） で必要なリポジトリを
 * 派生クラスでは複数インジェクションできることを覚えておいてほしい（未来の私：）。
 * これは、あくまでも最低限のサンプルであり、実際は複数のテーブルへの Insert、Update などが行われることが常
 * である。
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
