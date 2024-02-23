/**
 * Lost Chapter OR Mapping
 * 
 * Coffee Break 版がコンパイル時間を増大させるので、別ソースファイルに切り出した。
 * 今後は、ここで考え、試作を行う予定だ。アーキテクチャの上位（変更が少ない）下位（変更が多い）が明確になったら、ヘッダファイルにまとめる
 * ことを考えるかもしれない。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror coffee_break_orm.cpp -o ../bin/main
*/
#include <iostream>
#include <cassert>
#include <vector>
#include <memory>
#include <optional>
#include <set>
#include <chrono>

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
 * ORM（O/RM）
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
 * 
 * 以前作成したものは pair を利用したものだった、動的に DBMS 側の型の決定が可能だと考えるためであり、またできると思ったからだが。
 * タプルを利用すれば、確実にDBMS 側の型を明示的に持つことができると思った。
 * 
 * プロジェクト化の第一歩として、ヘッダとソースの分離を行う必要がある。併せて、ディレクト階層も考えること。
 * 例えば。 
 * ORM-Cheshire
 *             |
 *            src (data, model, repository)
 *             |
 *            inc
 * 
*/

/**
 * DataField ヘッダ
*/

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

int test_DataField_2() {
    puts("=== test_DataField_2");
    try {
        // タプルの動作確認
        DataField<int> d1("id", 3, "integer");
        auto[name, value, type] = d1.bindTuple();
        ptr_lambda_debug<const char*, const decltype(name)&>("name is ", name);
        ptr_lambda_debug<const char*, const decltype(value)&>("value is ", value);
        ptr_lambda_debug<const char*, const decltype(name)&>("type is ", type);
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

/**
 * RdbData ヘッダ
*/

class RdbData {
public:
    virtual ~RdbData() = default;
    virtual std::vector<std::string> getColumns() const = 0;
};

/**
 * RdbStrategy ヘッダ
*/

template<class T>
class RdbStrategy {
public:
    virtual ~RdbStrategy() = default;
    virtual std::vector<std::string> getColumns(const T&) const = 0;
};

/**
 * PersonData ヘッダ
*/

class PersonData final : public RdbData {
public:
    PersonData(std::unique_ptr<RdbStrategy<PersonData>> _strategy
    , const DataField<std::string>& _name
    , const DataField<std::string>& _email 
    , const DataField<int>& _age);

    PersonData(std::unique_ptr<RdbStrategy<PersonData>> _strategy
    , const DataField<std::string>& _name
    , const std::optional<DataField<std::string>>& _email 
    , const std::optional<DataField<int>>& _age);

    // ..
    virtual std::vector<std::string> getColumns() const override;

    const std::string                     getTableName() const;
    DataField<std::size_t>                getId() const;
    DataField<std::string>                getName() const;
    std::optional<DataField<std::string>> getEmail() const;
    std::optional<DataField<int>>         getAge() const;
private:
    const std::string TABLE_NAME;
    std::unique_ptr<RdbStrategy<PersonData>> strategy = nullptr;
    DataField<std::size_t> id;
    DataField<std::string> name;
    // 必須ではないデータは optional を利用するといいかもしれない。
    std::optional<DataField<std::string>> email;
    std::optional<DataField<int>>         age;
};

/**
 * PersonData 定義
*/

PersonData::PersonData(std::unique_ptr<RdbStrategy<PersonData>> _strategy
    , const DataField<std::string>& _name
    , const DataField<std::string>& _email 
    , const DataField<int>& _age) 
    : TABLE_NAME{std::move(std::string("person"))}
    , strategy{std::move(_strategy)}, id{std::move(DataField<std::size_t>("id",0))}, name{_name}, email{_email}, age{_age}
{
    // 必要ならここで Validation を行う、妥当性検証のオブジェクトをコンポジションして利用するのもあり。
}
PersonData::PersonData(std::unique_ptr<RdbStrategy<PersonData>> _strategy
    , const DataField<std::string>& _name
    , const std::optional<DataField<std::string>>& _email 
    , const std::optional<DataField<int>>& _age) 
    : TABLE_NAME{std::move(std::string("person"))}
    , strategy{std::move(_strategy)}, id{std::move(DataField<std::size_t>("id",0))}, name{_name}, email{_email}, age{_age}
{
    // 必要ならここで Validation を行う、妥当性検証のオブジェクトをコンポジションして利用するのもあり。
}

// ... 
std::vector<std::string> PersonData::getColumns() const {   // override
    puts("------ PersonData::getColums");
    return strategy.get()->getColumns(*this);
}

const std::string                     PersonData::getTableName()  const { return TABLE_NAME; }
DataField<std::size_t>                PersonData::getId()         const { return id; }
DataField<std::string>                PersonData::getName()       const { return name; }
std::optional<DataField<std::string>> PersonData::getEmail()      const { return email; }
std::optional<DataField<int>>         PersonData::getAge()        const { return age; }

/**
 * PersonStrategy ヘッダ
*/

class PersonStrategy final : public RdbStrategy<PersonData> {
public:
    virtual std::vector<std::string> getColumns(const PersonData& data) const override;
};

/**
 * PersonStrategy 定義
*/

std::vector<std::string> PersonStrategy::getColumns(const PersonData& data) const {         // override
    puts("TODO implementation ------ PersonStrategy::getColumns");
    std::vector<std::string> cols;
    // auto[id_name, id_value] = data.getId().bind();   // TODO プライマリキの Auto Increment あり／なし の判断が必要。 
                                                        // そればバリエーションポイントなので 別 Strategy になるかな。
    // Nullable の概念は必要かもしれない。
    auto[name_name, name_value] = data.getName().bind();
    cols.emplace_back(name_name);
    if(data.getEmail().has_value()) {
        auto[email_name, email_value] = data.getEmail().value().bind();
        cols.emplace_back(email_name);
    }
    if(data.getAge().has_value()) {
        auto[age_name, age_value] = data.getAge().value().bind();
        cols.emplace_back(age_name);
    }
    return cols;
}

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
    std::string cols(" (");
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

/**
 * UPDATE (表名) SET (カラム名1) = (値1) WHERE id = ?
 * 
 * UPDATE table_reference
    SET col_name1 = value1 [, col_name2 = value2, ...]
    [WHERE where_condition]
 * 
 * UPDATE test SET name = ?, label = ? WHERE id = ?
 * 
 * 更新すべき対象は 1行 としたい、Pkey を条件にする。
*/

std::string makeUpdateSql(const std::string& tableName, const std::string& pkName, const std::vector<std::string>& colNames ) {
    std::string sql("UPDATE ");
    sql.append(tableName);
    std::string set(" SET ");
    std::string condition(" WHERE ");
    condition.append(pkName).append(" = ?");
    for(std::size_t i=0; i<colNames.size(); i++) {
        set.append(colNames.at(i)).append(" = ?");
        if( i < colNames.size()-1 ) {
            set.append(", ");
        } 

    }
    sql.append(set).append(condition);
    return sql;    
}

int test_makeUpdateSql() {
    puts("=== test_makeUpdateSql");
    try {
        std::unique_ptr<RdbStrategy<PersonData>> strategy = std::make_unique<PersonStrategy>(PersonStrategy());
        DataField<std::string> name("name", "Derek");
        DataField<std::string> email("email", "derek@loki.org");
        DataField<int> age("age", 21);
        PersonData derek(std::move(strategy),name,email,age);
        auto[pk_nam, pk_val] = derek.getId().bind();
        auto sql = makeUpdateSql(derek.getTableName(),pk_nam, derek.getColumns());
        ptr_lambda_debug<const char*,const decltype(sql)&>("sql: ", sql);

        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * DELETE FROM table_name WHERE id = ?
 * 
 * 削除すべき対象は 1行 としたい、Pkey を条件にする。
*/

std::string makeDeleteSql(const std::string& tableName, const std::string& pkName) {
    std::string sql("DELETE FROM ");
    sql.append(tableName).append(" WHERE ").append(pkName).append(" = ?");
    return sql;
}

int test_makeDeleteSql() {
    puts("=== test_makeDeleteSql");
    try {
        std::unique_ptr<RdbStrategy<PersonData>> strategy = std::make_unique<PersonStrategy>(PersonStrategy());
        DataField<std::string> name("name", "Derek");
        DataField<std::string> email("email", "derek@loki.org");
        DataField<int> age("age", 21);
        PersonData derek(std::move(strategy),name,email,age);
        auto[pk_nam, pk_val] = derek.getId().bind();
        auto sql = makeDeleteSql(derek.getTableName(),pk_nam);
        ptr_lambda_debug<const char*,const decltype(sql)&>("sql: ", sql);

        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * SELECT col1, col2, col3 FROM table WHERE primary-key = ?
 * SELECT primary-key, col1, col2, col3 FROM table WHERE primary-key = ?
 * 
 * 仕様の問題、現状は pkey は RDBMS の Auto Increment を利用することを想定している、これは、次の実装に影響が及ぶ。
 * - PersonStrategy::getColumns メンバ関数に pkey を含めるか否かということ。含めた場合は、別途シーケンステーブルと pkey の取得処理が必要になる。
 * - makeFindOne の SELECT 句 の pkey の取り扱い方法について、上記に依存するため、この関数も本質的には 2 つ必要と思われる。
 * 
 * colNames に pkey は存在しないものとして次の関数は実装する。
 * 
*/

std::string makeFindOneSql(const std::string& tableName, const std::string& pkeyName, const std::vector<std::string>& colNames) {
    std::string sql("SELECT ");
    std::string cols(pkeyName);
    cols.append(", ");
    for(std::size_t i = 0; i < colNames.size(); i++) {
        cols.append(colNames.at(i));
        if( i < colNames.size()-1 ) {
            cols.append(", ");
        } 
    }
    sql.append(cols).append(" FROM ").append(tableName).append(" WHERE ").append(pkeyName).append(" = ?");
    return sql;
}

int test_makeFindOneSql() {
    puts("=== test_makeFindOneSql");
    try {
        std::unique_ptr<RdbStrategy<PersonData>> strategy = std::make_unique<PersonStrategy>(PersonStrategy());
        DataField<std::string> name("name", "Derek");
        DataField<std::string> email("email", "derek@loki.org");
        DataField<int> age("age", 21);
        PersonData derek(std::move(strategy),name,email,age);
        auto[pk_nam, pk_val] = derek.getId().bind();
        auto sql = makeFindOneSql(derek.getTableName(), pk_nam, derek.getColumns());
        ptr_lambda_debug<const char*, const decltype(sql)&>("sql: ", sql);

        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

// ... End Coffee Break.


int main(void) {
    puts("START Lost Chapter OR Mapping ===");
    if(0.01) {
        int ret = 0;
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", ret = test_debug_and_error());
        assert(ret == 1);   // エラーが発生したことを期待している。
    }
    if(0.02) {
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_DataField());
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_PersonData());
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_makeInsertSql());
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_makeUpdateSql());
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_makeDeleteSql());
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_DataField_2());
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_makeFindOneSql());
    }    
    puts("=== Lost Chapter OR Mapping END");
}
