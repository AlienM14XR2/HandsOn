#ifndef OBJECT_POOL_H_
#define OBJECT_POOL_H_

#include <queue>
#include <memory>
#include <mutex>
#include <string>
#include <iostream>
#include <stdexcept>
#include <functional>
#include <Repository.hpp>

template <class T>
class ObjectPool final : public std::enable_shared_from_this<ObjectPool<T>> {
private:
    // 1. コンストラクタを private に設定
    struct PrivateTag {}; 
public:
    // コンストラクタを PrivateTag 経由にすることで、外部からの直接呼び出しを禁止
    ObjectPool(PrivateTag, std::string _credit) : credit(std::move(_credit)) {}

    // 2. static ファクトリ関数によるインスタンス生成の強制
    // これにより、常に shared_ptr で管理されたインスタンスが生成される
    static std::shared_ptr<ObjectPool<T>> create(std::string _credit = "none.") {
        return std::make_shared<ObjectPool<T>>(PrivateTag{}, std::move(_credit));
    }

    using Ptr = std::unique_ptr<T, std::function<void(T*)>>;

    ~ObjectPool() {
        std::cout << "...... Done ObjectPool Destructor credit is " << credit << std::endl;
    }

    void push(std::unique_ptr<T> pt) {
        if (!pt) return;
        std::lock_guard<std::mutex> guard(m);
        int status;
        tmp::print_debug("push ... ", abi::__cxa_demangle(typeid(*this).name(),0,0,&status));
        q.push(std::move(pt));
    }

    Ptr pop() {
        std::lock_guard<std::mutex> guard(m);
        if (q.empty()) {
            return nullptr;
        }

        std::unique_ptr<T> raw_ptr = std::move(q.front());
        q.pop();

        // 3. shared_from_this を安全に実行可能
        std::weak_ptr<ObjectPool<T>> weak_pool = this->shared_from_this();

        return Ptr(raw_ptr.release(), [weak_pool](T* p) {
            if (auto pool = weak_pool.lock()) {
                pool->push(std::unique_ptr<T>(p));
            } else {
                delete p;
            }
        });
    }

private:
    const std::string credit;
    mutable std::mutex m;
    std::queue<std::unique_ptr<T>> q;

    // コピー・ムーブ禁止
    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;
};

#endif