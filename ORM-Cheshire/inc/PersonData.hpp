#ifndef _PERSONDATA_H_
#define _PERSONDATA_H_

#include <memory>
#include <string>
#include <vector>
#include <optional>
#include "RdbData.hpp"
#include "DataField.hpp"
#include "RdbDataStrategy.hpp"
#include "/usr/include/mysql-cppconn-8/mysql/jdbc.h"

class PersonData final : public RdbData {
private:
    // ダミーとして使うこと
    PersonData();
public:
    PersonData(RdbDataStrategy<PersonData>* _strategy
    , const DataField<std::string>& _name
    , const DataField<std::string>& _email 
    , const DataField<int>& _age);

    PersonData(RdbDataStrategy<PersonData>* _strategy
    , const DataField<std::size_t>& _id
    , const DataField<std::string>& _name
    , const DataField<std::string>& _email 
    , const DataField<int>& _age);

    PersonData(RdbDataStrategy<PersonData>* _strategy
    , const DataField<std::size_t>& _id
    , const DataField<std::string>& _name
    , const DataField<std::string>& _email 
    , std::optional<DataField<int>>& _age);

    PersonData(RdbDataStrategy<PersonData>* _strategy
    , const DataField<std::string>& _name
    , const DataField<std::string>& _email 
    , const std::optional<DataField<int>>& _age);


    // ..
    static PersonData dummy();
    static PersonData factory(sql::ResultSet* rs, RdbDataStrategy<PersonData>* strategy);
    static PersonData factoryNoAge(sql::ResultSet* rs, RdbDataStrategy<PersonData>* strategy);
    static PersonData factory(
          std::string _name
        , std::string _email
        , int         _age
        , RdbDataStrategy<PersonData>* _strategy);
    static PersonData factory(
          std::string _name
        , std::string _email
        , RdbDataStrategy<PersonData>* _strategy);
        
    virtual std::vector<std::string> getColumns() const override;
    virtual std::vector<std::tuple<std::string, std::string, std::string>> getTableInfo() const override;

    const std::string                     getTableName() const;
    DataField<std::size_t>                getId() const;
    DataField<std::string>                getName() const;
    DataField<std::string>                getEmail() const;
    std::optional<DataField<int>>         getAge() const;
    RdbDataStrategy<PersonData>*          getDataStrategy() const;
    void                                  setDataStrategy(RdbDataStrategy<PersonData>*);
    // 次の setter メンバ関数には DataField を介さず直接値（DataField::T にあたる value）を設定できた方が便利だが、カプセル化の兼ね合いも悩ましい。
    void                                  setName(DataField<std::string> _name);
    void                                  setEmail(DataField<std::string> _email);
    void                                  setAge(DataField<int> _age);
private:
    const std::string TABLE_NAME;
    // std::unique_ptr を 単純なデータ構造を保持するクラスに持つと、コピーできないという制限が強すぎて扱いづらくなる。クラス内では raw ポインタの方が都合がいいと思った。
    RdbDataStrategy<PersonData>* strategy;
    DataField<std::size_t> id;          // 必須
    DataField<std::string> name;        // 必須
    DataField<std::string> email;       // 必須
    // 必須ではないデータは optional を利用するといいかもしれない。
    std::optional<DataField<int>>         age;
};

namespace ormx {
class PersonData {
private:
    // ダミーとして使うこと
    PersonData();
public:
    PersonData(const std::size_t& _id
                                , const std::string& _name
                                , const std::string& _email
                                , const int&         _age
    );
    PersonData(const std::size_t& _id
                            , const std::string& _name
                            , const std::string& _email
    );
    PersonData(const std::string& _name
                            , const std::string& _email
                            , const int&         _age
    );
    PersonData(const std::string& _name
                            , const std::string& _email
    );
    // ...
    static ormx::PersonData dummy();
    std::size_t        getId()    const;
    std::string        getName()  const;
    std::string        getEmail() const;
    std::optional<int> getAge()   const;
private:
    std::size_t id;
    std::string name;
    std::string email;
    std::optional<int> age;
};
}   // namespace ormx

#endif
