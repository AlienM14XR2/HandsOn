#ifndef _REPOSITORY_H_
#define _REPOSITORY_H_

#include <optional>

/**
 * リポジトリ基底クラス
 * 
 * PKEY が複合 Key の場合の考慮はしていない。
 * 派生クラス、あるいは別の基底クラスを用意してほしい。
*/

template <class DATA, class PKEY>
class Repository {
public:
    virtual ~Repository() = default;
    // ...
    virtual std::optional<DATA> insert(const DATA&)  const = 0;
    virtual std::optional<DATA> update(const DATA&)   const = 0;
    virtual void remove(const PKEY&)   const = 0;
    virtual std::optional<DATA> findOne(const PKEY&)  const = 0;
};

#endif
