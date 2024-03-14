#ifndef _CIRCLE_H_
#define _CIRCLE_H_

#include "Shape.hpp"

namespace shape {

class Circle final : public Shape {
public:
    virtual void draw() override;
};

}   // namespace shape


#endif
