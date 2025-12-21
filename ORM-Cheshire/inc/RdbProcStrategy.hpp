#ifndef RDBPROCSTRATEGY_H_
#define RDBPROCSTRATEGY_H_

#include <optional>

/**
 * RdbProcStrategy クラス
 * 
 * RdbTransaction::proc のパリエーション・ポイントを表現するインタフェース。
 * CRUD の個別の処理は、本クラスの派生クラスで実現すること。
*/

template <class DATA>
class RdbProcStrategy {
public:
    virtual ~RdbProcStrategy() = default;
    virtual std::optional<DATA> proc() const = 0;
};

#endif
