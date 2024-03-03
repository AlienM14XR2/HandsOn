#ifndef _RDBCONNECTION_H_
#define _RDBCONNECTION_H_

/**
 * RDBMS のコネクション共通クラス（インタフェース）。
*/

template <class PREPARED_STATEMENT>
class RdbConnection {
public:
    virtual ~RdbConnection() = default;
    // ...
    virtual void begin() const = 0;
    virtual void commit() const = 0;
    virtual void rollback() const = 0;
    virtual PREPARED_STATEMENT* prepareStatement(const std::string& sql) const = 0;
};

#endif
