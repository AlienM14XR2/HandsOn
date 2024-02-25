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

/**
 * DataField 定義
*/

template <class T>
DataField<T>::DataField(const std::string& _name, const T& _value): name(_name), value(_value), type{std::move(std::string(""))}
{}
template <class T>
DataField<T>::DataField(const std::string& _name, const T& _value, const std::string& _type): 
                        name(_name), value(_value), type(_type)
{}
template <class T>
std::pair<std::string,T> DataField<T>::bind() const {
    return {name, value};
}
template <class T>
std::tuple<std::string, T, std::string> DataField<T>::bindTuple() const {
    return {name, value, type};
}

#endif
