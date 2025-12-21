#ifndef SHAPE_H_
#define SHAPE_H_

namespace shape {

class Shape {
public:
    virtual ~Shape() = default;
    virtual void draw() = 0;
};

}   // namespace shape

#endif
