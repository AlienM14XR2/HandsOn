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
concept ModelNeeds = requires(Data& d, Model& m) {
    m.insert(d);
    m.update(d);
    m.remove(d);
};
template<class Data, class Model>
requires ModelNeeds<Data, Model>
class CudModel final : public CudConcept
{
private:
    Data data;  // ここにデータを持ちたくない。不変にもできない場合は困る。
    Model model;
public:
    CudModel(Data _data, Model _model): data{std::move(_data)}, model{std::move(_model)}
    {}
    virtual uint64_t insert() const
    {
        return model.insert(data);
    }
    virtual void update() const
    {
        model.update(data);
    }
    virtual void remove() const
    {
        model.remove(data);
    }
    std::unique_ptr<CudConcept> clone() const override {
        return std::make_unique<CudModel>(*this);
    }
};

}   // namespace tmp

#endif
