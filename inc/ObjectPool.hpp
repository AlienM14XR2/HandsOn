#ifndef _OBJECTPOOL_H_
#define _OBJECTPOOL_H_

#include <queue>
#include <memory>
#include <mutex>
#include <Exception.hpp>

template <class T, class SmartPointer = std::unique_ptr<T>>
class ObjectPool final {
public:
    ObjectPool() : credit(std::move("none."))
    {}
    ObjectPool(const std::string& _credit) : credit(std::move(_credit))
    {}
    ~ObjectPool() {
        // while(!q.empty()) {
        //     const T* pt = q.front();
        //     q.pop();
        //     delete pt;
        // }
        std::string message(R"(...... Done ObjectPool Destructor credit is )");
        message.append(credit);
        puts(message.c_str());
    }
    bool empty() {
        return q.empty();
    }
    void push(SmartPointer&& pt) const {
        std::lock_guard<std::mutex> guard(m);
        q.push(std::forward<SmartPointer>(pt));
    }
    SmartPointer pop() const {
        std::lock_guard<std::mutex> guard(m);
        SmartPointer ret = nullptr;
        if(!q.empty()) {
            ret = move(q.front());
            q.pop();
        }
        if(!ret) throw std::runtime_error(NoPoolException().what());
        return ret;
    }
private:
    const std::string credit;
    mutable std::mutex m;
    mutable std::queue<SmartPointer> q;
};

#endif
