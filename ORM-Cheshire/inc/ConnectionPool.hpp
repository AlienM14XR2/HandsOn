#ifndef _CONNECTIONPOOL_H_
#define _CONNECTIONPOOL_H_

#include <queue>
#include <mutex>
#include "Exception.hpp"

template <class T>
class ConnectionPool final {
public:
    ~ConnectionPool() {     // その役割が任意のポインタの Pool なので、解放は本クラスで行う必要がある。
        while(!q.empty()) {
            const T* pt = q.front();
            q.pop();
            delete pt;
        }
        puts("...... Done ConnectionPool Destructor.");
    }
    bool empty() {
        return q.empty();
    }
    void push(const T* pt) const {
        std::lock_guard<std::mutex> guard(m);
        q.push(pt);
    }
    const T* pop() const {
        std::lock_guard<std::mutex> guard(m);
        const T* ret = nullptr;
        if(!q.empty()) {
            ret = q.front();
            q.pop();
        }
        if(!ret) {
            throw std::runtime_error(NoPoolException().what()) ;
        }
        return ret;     // TODO nullptr の場合は、何らかの exception としたいが、やりすぎかな。
    }
private:
    mutable std::mutex m;
    mutable std::queue<const T*> q;
};

#endif
