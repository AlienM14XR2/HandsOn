#ifndef _REPOSITORY_H_
#define _REPOSITORY_H_

#include <optional>

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
    virtual Data findById() const = 0;
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
    virtual Data findById() const override
    {
        return model.findById(data);
    }
};

}   // namespace tmp::r1
#endif
