#ifndef DATA_FIELD_H_
#define DATA_FIELD_H_

template <class T>
class DataField {
public:
    explicit DataField(const std::string& _name, const T& _value);
    explicit DataField(const std::string& _name, const T& _value, const std::string& _type);
    explicit DataField(const std::string& _name, const T& _value, const std::string& _type, const std::string& _constraint);
    // ...
    std::pair<std::string,T> bind() const;
    std::tuple<std::string, T, std::string> bindTuple() const;
    std::tuple<std::string, std::string, std::string> bindTupleTblInfo() const;
    /**
     * これは使い勝手の問題なので本当に必要と感じた際に実装すればいいが std::pair や std::tuple だけではなく
     * name や value のみを返却するメンバ関数は、あった方が良くはなかろうか。
    */
    std::string getName() const {
        return name;
    }
    T getValue() const {
        return value;
    }
private:
    std::string name;
    T value;
    std::string type;
    std::string constraint;
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
DataField<T>::DataField(const std::string& _name, const T& _value, const std::string& _type, const std::string& _constraint): 
                        name(_name), value(_value), type(_type), constraint(_constraint)
{}
// ... 
template <class T>
std::pair<std::string,T> DataField<T>::bind() const {
    return {name, value};
}
template <class T>
std::tuple<std::string, T, std::string> DataField<T>::bindTuple() const {
    return {name, value, type};
}
template <class T>
std::tuple<std::string, std::string, std::string> DataField<T>::bindTupleTblInfo() const {
    return {name, type, constraint};
}

#endif
