/**
 * 5 章 右辺値参照、ムーブセマンティクス、完全転送
 * 
 * 項目 27 ：転送参照をとるオーバーロードの代替策を把握する
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_5_move_forward_27.cpp -o ../bin/main
*/
#include <iostream>
#include <cassert>
#include <vector>
#include <memory>
#include <optional>

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](const auto message, const auto debug) -> void {
    std::cout << "DEBUG: " << message << '\t' << debug << std::endl;
};

template <class Error>
concept ErrReasonable = requires(Error& e) {
    e.what();
};
template <class Error>
requires ErrReasonable<Error>
void (*ptr_print_error)(Error) = [](const auto e) -> void {
    std::cout << "ERROR: " << e.what() << std::endl;
};

int test_debug_and_error() {
    puts("=== test_debug_and_error");
    try {
        // ptr_print_now();
        auto pi = 3.141592;
        ptr_lambda_debug<const char*,const decltype(pi)&>("pi is ", pi);
        ptr_lambda_debug<const char*,const decltype(pi)*>("pi addr is ", &pi);
        throw std::runtime_error("It's test error.");
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        printf("e type is ... %s\n", typeid(e).name());
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * Coffee Break
 * 
 * O/R Mapping (ORM) を考えてみる
 * まずは最低限必要と思われる情報をクラスにする。
 * - 型情報
 * - 名前（カラム名）
 * - 値
 * 
 * 課題
 * - ヌル値の扱い
 * - ID の AutoSequence
*/

template <class T>
class DataField {
public:
    DataField(const std::string& _name, const T& _value): name(_name), value(_value)
    {}

    // ...
    std::pair<std::string,T> bind() {
        return {name, value};
    }
    // void setValue(T&& _value) {
    //     value = std::forward<T>(_value);
    // }

private:
    std::string name;
    T value;
};

/**
 * 上例のクラスを std::vector なりのコンテナに詰めたものが、レコードの 1 行を表現するのではないのか？
*/

int test_DataField() {
    puts("=== test_DataField");
    try {
        DataField<int> d1("id",1);
        auto[name, value] = d1.bind();
        ptr_lambda_debug<const char*,const decltype(value)&>("value is ", value);
        ptr_lambda_debug<const char*,const std::string&>("value type is ", typeid(value).name());   // Boost のライブラリの方が正確との情報があった。
        ptr_lambda_debug<const char*,const decltype(name)&>("name is ", name);

        DataField<std::string> d2("name", "Alice");
        auto[name2, value2] = d2.bind();
        ptr_lambda_debug<const char*,const decltype(value2)&>("value2 is ", value2);
        ptr_lambda_debug<const char*,const std::string&>("value2 type is ", typeid(value2).name());   // Boost のライブラリの方が正確との情報があった。
        ptr_lambda_debug<const char*,const decltype(name2)&>("name2 is ", name2);
        /**
         * TODO Jack
         * 時間を見て、MySQL にある型と一致する C++ の型を網羅させる。
        */
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * ORM の本質を、或いは自分が必要と思う機能を列挙してみる。
 * - ORM オブジェクトからの動的 SQL の生成、最低限のもので良い（CRUD）
 * - RDBMS から取得したレコードの ORM オブジェクトへの代入、あるいは初期化。
 * 
 * テーブルを表現するクラスを考えてみる。
*/

class RdbData {
public:
    virtual ~RdbData() = default;
    virtual std::vector<std::string> getColumns() const = 0;
};

template<class T>
class RdbStrategy {
public:
    virtual ~RdbStrategy() = default;
    virtual std::vector<std::string> getColumns(const T&) const = 0;
};

class PersonData final : public RdbData {
public:
    PersonData(std::unique_ptr<RdbStrategy<PersonData>> _strategy
    , const DataField<std::string>& _name
    , const DataField<std::string>& _email 
    , const DataField<int>& _age) 
    : TABLE_NAME{std::move(std::string("person"))}
    , strategy{std::move(_strategy)}, id{std::move(DataField<std::size_t>("id",0))}, name{_name}, email{_email}, age{_age}
    {
        // 必要ならここで Validation を行う、妥当性検証のオブジェクトをコンポジションして利用するのもあり。
    }

    PersonData(std::unique_ptr<RdbStrategy<PersonData>> _strategy
    , const DataField<std::string>& _name
    , const std::optional<DataField<std::string>>& _email 
    , const std::optional<DataField<int>>& _age) 
    : TABLE_NAME{std::move(std::string("person"))}
    , strategy{std::move(_strategy)}, id{std::move(DataField<std::size_t>("id",0))}, name{_name}, email{_email}, age{_age}
    {
        // 必要ならここで Validation を行う、妥当性検証のオブジェクトをコンポジションして利用するのもあり。
    }

    // ..
    virtual std::vector<std::string> getColumns() const override {
        puts("------ PersonData::getColums");
        return strategy.get()->getColumns(*this);
    }

    const std::string getTableName() const { return TABLE_NAME; }
    DataField<std::size_t> getId() const { return id; }
    DataField<std::string> getName() const { return name; }
    std::optional<DataField<std::string>> getEmail() const { return email; }
    std::optional<DataField<int>>         getAge() const { return age; }
private:
    const std::string TABLE_NAME;
    std::unique_ptr<RdbStrategy<PersonData>> strategy = nullptr;
    DataField<std::size_t> id;
    DataField<std::string> name;
    // 必須ではないデータは optional を利用するといいかもしれない。
    std::optional<DataField<std::string>> email;
    std::optional<DataField<int>>         age;
};

class PersonStrategy final : public RdbStrategy<PersonData> {
public:
    virtual std::vector<std::string> getColumns(const PersonData& data) const override {
        puts("TODO implementation ------ PersonStrategy::getColumns");
        std::vector<std::string> cols;
        // auto[id_name, id_value] = data.getId().bind();   // TODO プライマリキの Auto Increment あり／なし の判断が必要
        // Nullable の概念は必要かもしれない。
        auto[name_name, name_value] = data.getName().bind();
        if(data.getEmail().has_value()) {
            auto[email_name, email_value] = data.getEmail().value().bind();
            cols.emplace_back(email_name);
        }
        if(data.getAge().has_value()) {
            auto[age_name, age_value] = data.getAge().value().bind();
            cols.emplace_back(age_name);
        }
        cols.emplace_back(name_name);
        return cols;
    }
};

int test_PersonData() {
    puts("=== test_PersonData");
    try {
        std::unique_ptr<RdbStrategy<PersonData>> strategy = std::make_unique<PersonStrategy>(PersonStrategy());
        DataField<std::string> name("name", "Derek");
        DataField<std::string> email("email", "derek@loki.org");
        DataField<int> age("age", 21);
        PersonData derek(std::move(strategy),name,email,age);

        ptr_lambda_debug<const char*,const std::string&>("table is ", derek.getTableName());
        auto[nam, val] = derek.getName().bind();
        ptr_lambda_debug<const char*, const decltype(nam)&>("name is ", nam);        
        ptr_lambda_debug<const char*, const decltype(val)&>("value is ", val);        
        auto[nam2, val2] = derek.getEmail().value().bind();
        ptr_lambda_debug<const char*, const decltype(nam2)&>("name is ", nam2);        
        ptr_lambda_debug<const char*, const decltype(val2)&>("value is ", val2);
        auto[nam3, val3] = derek.getAge().value().bind();
        ptr_lambda_debug<const char*, const decltype(nam3)&>("name is ", nam3);        
        ptr_lambda_debug<const char*, const decltype(val3)&>("value is ", val3);

        /**
         * 次はこのテーブル情報を持ったクラスから、動的に SQL 生成ができればよい、具体的には以下。
         * - INSERT 文
         * - UPDATE 文
         * - DELETE 文
         * - SELECT 文 これは、findOne() のみでよい。
         * これらは定型文なので自動で SQL を生成する仕組みがあった方が絶対にいい。
         * 
        */
        auto cols = derek.getColumns();
        for(auto name: cols) {
            ptr_lambda_debug<const char*, const decltype(name)&>("name is ", name);
        }
        assert(cols.size() == 3U);
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

template <class Data, class PKEY>
class Repository {
    bool insert(const Data&) = 0;
    bool update(const Data&) = 0;
    bool remove(const PKEY&) = 0;
    Data findOne(const PKEY&) = 0;
};

/**
 * SQL 文の構築に関する考察
 * 
 * INSERT INTO テーブル名 (列1, 列2, ...)
 * VALUES (値1, 値2, ...);
 *
 * INSERT INTO customer (name, route, saved_date, price) 
 * VALUES ('松田', 'ad2', '2023-05-24 19:49:28', 2500);
 *
 * prep_stmt = con->prepareStatement("INSERT INTO test(id, label) VALUES (?, ?)");      // MySQL の Prepared Statements の例
 * 
 * SQL インジェクションの可能性を考慮すれば、Prepared Statements の利用は必須と考える。
 * よって、今回の 動的 SQL 文の最終型は 『"INSERT INTO test (id, label) VALUES (?, ?)"』とする。
 * 
 * カラム名とその数分の ? の出力ということ。
 * 
*/

std::string makeInsertSql(const std::string& tableName, const std::vector<std::string>& colNames) {
    std::string sql("INSERT INTO ");
    sql.append(tableName);
    // カラム
    std::string cols("(");
    // 値
    std::string vals("VALUES (");
    for(std::size_t i=0 ; i<colNames.size(); i++) {
        cols.append(colNames.at(i));
        vals.append("?");
        if( i < colNames.size()-1 ) {
            cols.append(", ");
            vals.append(", ");
        } 
    }
    cols.append(") ");
    vals.append(")");
    sql.append(std::move(cols));
    sql.append(std::move(vals));
    return sql;
}

int test_makeInsertSql() {
    puts("=== test_makeInsertSql");
    try {
        std::unique_ptr<RdbStrategy<PersonData>> strategy = std::make_unique<PersonStrategy>(PersonStrategy());
        DataField<std::string> name("name", "Derek");
        DataField<std::string> email("email", "derek@loki.org");
        DataField<int> age("age", 21);
        PersonData derek(std::move(strategy),name,email,age);

        auto sql = makeInsertSql(derek.getTableName(), derek.getColumns());
        ptr_lambda_debug<const char*,const decltype(sql)&>("sql: ", sql);

        std::unique_ptr<RdbStrategy<PersonData>> strategy2 = std::make_unique<PersonStrategy>(PersonStrategy());
        DataField<std::string> name2("name", "Cheshire");
        std::optional<DataField<std::string>> empty_email;
        std::optional<DataField<int>> empty_age;
        PersonData cheshire(std::move(strategy2),name2,empty_email,empty_age);

        auto sql2 = makeInsertSql(cheshire.getTableName(), cheshire.getColumns());
        ptr_lambda_debug<const char*,const decltype(sql2)&>("sql2: ", sql2);
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

// ... End Coffee Break.

/**
 * 本項は特にコーディングは少ない、あるいはない可能性もある。
 * 
 * 『オーバーロードしない』
 * 
 * 『const T& を渡す』
 * 
 * 『値を渡す』
 * 
 * 『タグディスパッチを用いる』
 * 値渡しも、const オブジェクトの左辺値参照渡しも、完全転送には対応していません。転送参照を用いた目的が完全転送にあるならば、
 * 転送参照を使用せざるを得ません。他には選択はありません。しかし、それでもオーバーロードは諦めたくありません。さて、オーバー
 * ロードも転送参照も諦めずに転送参照をとるオーバーロードにまつわる問題を回避する方法はあるでしょうか？
 * 
*/

int main(void) {
    puts("START 項目 27 ：転送参照をとるオーバーロードの代替策を把握する ===");
    if(0.01) {
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_debug_and_error());
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_DataField());
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_PersonData());
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_makeInsertSql());
    }
    puts("=== 項目 27 ：転送参照をとるオーバーロードの代替策を把握する END");
    return 0;
}