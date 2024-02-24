#ifndef _DATA_FIELD_H_
#define _DATA_FIELD_H_

template <class T>
class DataField {
public:
    explicit DataField(const std::string& _name, const T& _value);
    explicit DataField(const std::string& _name, const T& _value, const std::string& _type);
    // ...
    std::pair<std::string,T> bind() const;
    std::tuple<std::string, T, std::string> bindTuple() const;
private:
    std::string name;
    T value;
    std::string type;
};


#endif