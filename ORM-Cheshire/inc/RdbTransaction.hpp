#ifndef _TDBTRANSACTION_H_
#define _TDBTRANSACTION_H_

#include <optional>

/**
 * RdbTransaction クラス
 * 
 * RDBMS のトランザクション処理の基底クラス
*/

template <class DATA>
class RdbTransaction {
public:
    virtual ~RdbTransaction() = default;
    // ...

    /**
     * 次のメンバ関数は、戻り値を返すものも必要だと思う、proc も然り。
     * std::optional だったら 1 つでいいのかも。
    */

    std::optional<DATA> executeTx() const {
        try {
            begin();
            std::optional<DATA> data = proc();         // これが バリエーション・ポイント
            commit();
            return data;
        } catch(std::exception& e) {
            rollback();
            ptr_print_error<const decltype(e)&>(e);
            throw std::runtime_error(e.what());
        }
    }
    virtual void begin()    const = 0;
    virtual void commit()   const = 0;
    virtual void rollback() const = 0;
    virtual std::optional<DATA> proc() const = 0;
};

#endif
