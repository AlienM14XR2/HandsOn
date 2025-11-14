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

}   // namespace tmp

#endif
