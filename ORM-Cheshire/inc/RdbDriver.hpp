#ifndef _RDBDRIVER_H_
#define _RDBDRIVER_H_

template <class D>
class RdbDriver {
public:
    virtual ~RdbDriver() = default;
    virtual D* getDriver() const = 0; 
};

#endif
