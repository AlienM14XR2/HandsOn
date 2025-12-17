#ifndef _OBJECTPOOL_H_
#define _OBJECTPOOL_H_

#include <queue>
#include <memory>
#include <mutex>
#include <stdexcept> // std::runtime_error を使用

template <class T, class SmartPointer = std::unique_ptr<T>>
class ObjectPool final {
public:
    // --- コネクションガードクラスの定義 (RAII) ---
    class ConnectionGuard {
    private:
        friend class ObjectPool;
        SmartPointer ptr;
        ObjectPool* pool;

        // プールからのみ生成可能
        ConnectionGuard(SmartPointer&& p, ObjectPool* parent_pool)
            : ptr(std::move(p)), pool(parent_pool) {}

    public:
        // ムーブコンストラクタとムーブ代入演算子を定義 (コピーは禁止)
        ConnectionGuard(ConnectionGuard&& other) noexcept
            : ptr(std::move(other.ptr)), pool(other.pool) {
            other.pool = nullptr; // 所有権を移動
        }
        ConnectionGuard& operator=(ConnectionGuard&& other) noexcept {
            if (this != &other) {
                // 現在のコネクションをプールに返却
                if (pool) { pool->push(std::move(ptr)); }
                ptr = std::move(other.ptr);
                pool = other.pool;
                other.pool = nullptr;
            }
            return *this;
        }

        // デストラクタで自動的にプールに返却する
        ~ConnectionGuard() {
            if (pool && ptr) {
                pool->push(std::move(ptr));
            }
        }
        
        // オブジェクトへのアクセスを提供
        T* get() const { return ptr.get(); }
        T* operator->() const { return ptr.operator->(); }
        // 明示的な型変換を禁止し、get() か operator->() を強制する
        // operator T*() const { return ptr.get(); } 
    };
    // ---------------------------------------------


    ObjectPool() : credit(std::move("none.")) {}
    ObjectPool(const std::string& _credit) : credit(std::move(_credit)) {}
    ~ObjectPool() {
        // unique_ptr が自動的にメモリを管理するため、手動解放コードは不要
        std::string message(R"(...... Done ObjectPool Destructor credit is )");
        message.append(credit);
        puts(message.c_str());
    }

    bool empty() const { // const を追加してステートレス性を強調
        std::lock_guard<std::mutex> guard(m);
        return q.empty();
    }

    // SmartPointer をプールに戻す
    void push(SmartPointer&& pt) const {
        std::lock_guard<std::mutex> guard(m);
        q.push(std::forward<SmartPointer>(pt));
    }

    // プールからオブジェクトを取得し、ConnectionGuard でラップして返す
    ConnectionGuard pop() const {
        std::lock_guard<std::mutex> guard(m);
        if(q.empty()) {
            throw std::runtime_error("No objects available in the pool.");
        }
        // std::move を修正箇所に追加
        SmartPointer ret = std::move(q.front());
        q.pop();
        
        // ガードオブジェクトを構築して所有権を移譲
        return ConnectionGuard(std::move(ret), const_cast<ObjectPool*>(this));
    }

private:
    const std::string credit;
    mutable std::mutex m;
    mutable std::queue<SmartPointer> q;

    // コピー禁止
    ObjectPool(const ObjectPool&) = delete;
    ObjectPool& operator=(const ObjectPool&) = delete;
};

#endif
