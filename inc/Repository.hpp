#ifndef _REPOSITORY_H_
#define _REPOSITORY_H_

#include <optional>
#include <variant>

namespace tmp {

template <class ID, class Data>
class Repository {
public:
    virtual ~Repository() = default;
    virtual ID insert(Data&&) const = 0;
    virtual void update(const ID&, Data&&) const = 0;
    virtual void remove(const ID&) const = 0;
    virtual std::optional<Data> findById(const ID&) const = 0;
};

// VarNode が保持できる型のリストを定義する
using ValueType = std::variant<
    std::monostate, // 値がない状態を表す
    int64_t,
    uint64_t,
    float,
    double,
    bool,
    std::string
>;
// ValueType (std::variant) が表現する型であれば何でも保持可能な構造体
// ディクトリ構造も表現可能なもの
struct VarNode
{
    std::string key;
    ValueType data;
    VarNode* parent = nullptr;
    std::vector<std::unique_ptr<VarNode>> children;

    // コンストラクタを ValueType を受け取るように変更
    VarNode(const std::string& _key, ValueType _data, VarNode* _parent = nullptr)
        : key(_key), data(_data), parent(_parent)
    {}

    // 値の取得は std::get<T> を使う（型が違えば std::bad_variant_access を投げる）
    template <class T>
    T get() const
    {
        return std::get<T>(data);
        // std::get_if を使って安全なポインタ取得を試みる
        // return std::get_if<T>(data);
    }
    template <class T>
    bool is_type() const
    {
        return std::holds_alternative<T>(data);
    }
    VarNode* addChild(const std::string& _key, ValueType _data)
    {
        children.push_back(std::make_unique<VarNode>(_key, _data, this));
        return children.back().get();
    }
    VarNode* getChild(const std::string& _key)
    {
        // C++20 であれば std::ranges::find_if が使えます
        auto it = std::find_if(children.begin(), children.end(), 
                            [&](const std::unique_ptr<VarNode>& child){
                                return child->key == _key;
                            });
        if (it != children.end()) {
            return it->get();
        }
        return nullptr; // 見つからなかった場合は nullptr を返す
    }
    // debug関数は std::visit を使うと大幅に簡潔化できる
    static void debug(const VarNode* const _node, int indent = 0)
    {
        if (_node == nullptr) {
            std::cerr << "node is nil." << std::endl;
            return;
        }
        // インデント表示
        std::cout << std::string(indent * 2, ' ');
        std::cout << "key: " << _node->key << "\tdata: ";

        // std::visit のラムダ式内で、全ての型を明示的に処理する
        std::visit([](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, std::monostate>) {
                std::cout << "(null)";
            } else if constexpr (std::is_same_v<T, bool>) {
                std::cout << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int64_t> || 
                                 std::is_same_v<T, uint64_t> ||
                                 std::is_same_v<T, float> ||
                                 std::is_same_v<T, double> ||
                                 std::is_same_v<T, std::string>) {
                // std::cout はこれらのプリミティブ型やstd::stringを安全に出力できる
                std::cout << arg;
            } else {
                // 将来ValueTypeに新しい型（例えばカスタムオブジェクトやポインタ）が追加された場合
                // コンパイルエラーにならずに、ここで処理を止めるか、汎用的な出力を行う
                std::cout << "(Unknown/Unhandled Type)";
            }
        }, _node->data);
        
        std::cout << std::endl;

        // 子要素を再帰的に呼び出す
        for (const auto& child : _node->children) {
            debug(child.get(), indent + 1);
        }
    }
};  // VarNode
template <typename T>
std::unique_ptr<T> get_value_safely(const VarNode* const node)
{
    if (node == nullptr) {
        // node 自体が nullptr の場合は runtime_error を投げる
        throw std::runtime_error("VarNode* node is null.");
    }
    // std::get_if<T>(&node->data) が、VarNodeTypeFixer の役割を果たす
    // - T 型が格納されていれば T* を返す
    // - T 型でなければ nullptr を返す
    if (const T* value_ptr = std::get_if<T>(&node->data)) {
        // 値が見つかったので、unique_ptr でラップして返す
        return std::make_unique<T>(*value_ptr);
    } else {
        // 型が一致しない場合は nullptr を返す
        return nullptr;
    }
}

template <typename T>
std::optional<T> get_value_safely_optional(const VarNode* const node)
{
    if (node == nullptr) {
        throw std::runtime_error("VarNode* node is null.");
    }
    if (const T* value_ptr = std::get_if<T>(&node->data)) {
        return *value_ptr; // 値そのものを optional でラップして返す
    } else {
        return std::nullopt; // 値がないことを示す
    }
}

// Type Erasure パターンを利用したリポジトリの設計。
class CudConcept
{
public:
    virtual ~CudConcept() = default;
    virtual uint64_t insert() const = 0;
    virtual void update() const = 0;
    virtual void remove() const = 0;
    virtual std::unique_ptr<CudConcept> clone() const = 0;
};


template <class Data, class Model>
concept CudNeeds = requires(Data& d, Model& m) {
    m.insert(d);
    m.update(d);
    m.remove(d);
};
template<class Data, class Model>
requires CudNeeds<Data, Model>
class CudModel final : public CudConcept
{
private:
    Data data;  // ここにデータを持ちたくない。不変にもできない場合は困る。
    Model model;
public:
    CudModel(Data _data, Model _model): data{std::move(_data)}, model{std::move(_model)}
    {}
    virtual uint64_t insert() const override
    {
        return model.insert(data);
    }
    virtual void update() const override
    {
        model.update(data);
    }
    virtual void remove() const override
    {
        model.remove(data);
    }
    std::unique_ptr<CudConcept> clone() const override {
        return std::make_unique<CudModel>(*this);
    }
};

}   // namespace tmp



namespace tmp::r1
{


// Type Erasure としては不完全なのか？
// 使い勝手とこのコンセプトをどのように考えるかによる。
// このコンセプトを独立したインタフェースとしたい場合は
// 必ず、Data を必要とするため不便かもしれないが、結局
// 具象化クラスではData を必要としているし。
// CRUD をひとつのコンセプトとしたい場合は、結局この形に
// 落ち着く。判断が難しい。

template <class Data>
class CrudConcept
{
public:
    virtual ~CrudConcept() = default;
    virtual uint64_t insert() const = 0;
    virtual void update() const = 0;
    virtual void remove() const = 0;
    virtual std::unique_ptr<CrudConcept> clone() const = 0;
    virtual std::optional<Data> findById() const = 0;
};


template <class Data, class Model>
concept CrudNeeds = requires(Data& d, Model& m) {
    m.insert(d);
    m.update(d);
    m.remove(d);
    m.findById(d);
};
template<class Data, class Model>
requires CrudNeeds<Data, Model>
class CrudModel final : public CrudConcept<Data>
{
private:
    Data data;  // ここにデータを持ちたくない。不変にもできない場合は困る。
    Model model;
public:
    CrudModel(Data _data, Model _model): data{std::move(_data)}, model{std::move(_model)}
    {}
    virtual uint64_t insert() const override
    {
        return model.insert(data);
    }
    virtual void update() const override
    {
        model.update(data);
    }
    virtual void remove() const override
    {
        model.remove(data);
    }
    std::unique_ptr<CrudConcept<Data>> clone() const override {
        return std::make_unique<CrudModel>(*this);
    }
    virtual std::optional<Data> findById() const override
    {
        return model.findById(data);
    }
};

}   // namespace tmp::r1
#endif
