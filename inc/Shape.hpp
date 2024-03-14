#ifndef _SHAPE_H_
#define _SHAPE_H_

namespace shape {

class Shape {
public:
    virtual ~Shape() = default;
    virtual void draw() = 0;
};

}   // namespace shape

#endif
