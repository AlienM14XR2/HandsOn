/**
 * Proxy パターン（であっているのかそれは分からない）
 * 
 * やりたいことは次のようなこと。
 * - あるクラスでは必要最低限の設定、コーディングのみ
 * - 実際の処理を行うのは別のクラス、上記の『代理人』のような存在。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror proxy.cpp -o ../bin/main
*/
#include <iostream>
#include <cassert>
#include <chrono>
#include <optional>
#include <map>
#include <vector>
#include "mysqlx/xdevapi.h"

template <typename M, typename D>
void (*ptr_debug)(M, D) = [](const auto message, const auto debug) -> void {
    std::cout << "DEBUG: " << message << '\t' << debug << std::endl;
};

template <class ERR>
concept ErrReasonable = requires(ERR& e) {
    e.what();
};
template <class ERR>
requires ErrReasonable<ERR>
void (*ptr_error)(ERR) = [](const auto e) -> void {
    std::cout << "ERROR: " << e.what() << std::endl;
};

int test_debug_error()
{
    puts("=== test_debug_error");
    try {
        auto micro_s = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        // std::clock_t start = clock();
        auto pi = 3.141592;
        ptr_debug<const char*, const decltype(pi)>("pi is ", pi);
        auto micro_e = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        // auto micro = (milli_e-milli_s)*1000;
        auto passed = micro_e-micro_s;
        std::cout << "passed " << passed << " micro sec." << std::endl;
        // std::clock_t end = clock();
        // auto sec = (double)(end-start)/CLOCKS_PER_SEC;
        std::runtime_error("It's test exception.");
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * 設計と考察はここから。
 * 
 * Repository をインタフェースとして、ProxyRepo と SubjectRepo がある。
*/

template <class DATA>
class MySQLXData {
public:
    virtual ~MySQLXData() = default;
    virtual DATA insertQuery(mysqlx::Session*) const = 0;
    virtual DATA updateQuery(mysqlx::Session*) const = 0;
};

class PersonData final : public MySQLXData<PersonData> {
public:
    PersonData(const std::size_t& _id
            , const std::string& _name
            , const std::string& _email
            , const int& _age ): id(_id), name(_name), email(_email), age(_age)
    {}
    PersonData(const std::size_t& _id
            , const std::string& _name
            , const std::string& _email): id(_id), name(_name), email(_email), age(std::nullopt)
    {}
    PersonData(const std::string& _name
            , const std::string& _email
            , const int& _age ): id(0ul), name(_name), email(_email), age(_age)
    {}
    PersonData(const std::string& _name
            , const std::string& _email ): id(0ul), name(_name), email(_email), age(std::nullopt)
    {}
    // ...
    // virtual std::string getTableName() const override
    // {
    //     return "person";
    // }
    // virtual std::map<std::string, std::string> toMap() const override
    // {
    //     // TODO 次の点を踏まえて、PKEY の std::optional を考慮してくれ。
    //     // INSERT 文 で Auto-Increment を採用している場合は、PKEY は Key Value には不要なんだよな。
    //     std::map<std::string, std::string> m{
    //         {"id", std::to_string(id)}
    //         , {"name", name}
    //         , {"email", email}
    //     };
    //     if(age.has_value()) {
    //         m.insert(std::make_pair("age", std::to_string(age.value())));
    //     }
    //     return m;
    // }

    virtual PersonData insertQuery(mysqlx::Session* sess) const override
    {
        puts("------ PersonData::insertQuery()");
        mysqlx::Schema db = sess->getSchema("cheshire");
        mysqlx::Table person = db.getTable("person");
        mysqlx::Result res = person.insert("name", "email", "age")
                                    .values(name, email, age.value())
                                    .execute();
        PersonData result(res.getAutoIncrementValue(), name, email);
        if(age.has_value()){
            result.setAge(age.value());
        }
        return result;
    }

    virtual PersonData updateQuery(mysqlx::Session* sess) const override
    {
        puts("------ PersonData::updateQuery()");
        // TODO 実装
        mysqlx::Schema db = sess->getSchema("cheshire");
        mysqlx::Table person = db.getTable("person");
        std::string cond("id = ");
        cond.append(std::to_string(id));
        // person.update("name", "email", "age").where(cond)
        PersonData result(id, name, email);
        if(age.has_value()){
            result.setAge(age.value());
        }
        return result;
    }

    std::size_t getId()         const { return id; }
    std::string getName()       const { return name; }
    std::string getEmail()      const { return email; }
    std::optional<int> getAge() const { return age; }
    void setAge(const int& _age)      { age = _age; }
private:
    // TODO PKEY に該当するデータも std::optional の方が扱い易くないか、考えてみる。
    // DONE. データが SQL の処理を実行する設計では、意味がないと判断した。
    std::size_t        id;
    std::string        name;
    std::string        email;
    std::optional<int> age;
};

template <class DATA, class PKEY>
class Repository {
public:
    virtual ~Repository() = default;
    // ... 
    virtual DATA insert(const DATA&)  const = 0;
    virtual DATA update(const DATA&)  const = 0;
    virtual void remove(const PKEY&)  const = 0;
    virtual DATA findOne(const PKEY&) const = 0;

};

template<class DATA>
class MySQLXBasicRepository {
public:
    MySQLXBasicRepository(mysqlx::Session* _session): session(_session)
    {}
    virtual DATA insert(const DATA& data)  const
    {
        puts("------ MySQLXBasicRepository::insert()");
        const MySQLXData<DATA>* pdata = static_cast<const DATA*>(&data);
        return pdata->insertQuery(session);
    }
    // virtual DATA update(const DATA&)  const
    // {
    //     puts("------ MySQLXBasicRepository::update()");
    // }
    /**
     * 単なるテンプレート型に過ぎない DATA をどのようにインスタンス化するのか。
     * これが、できない限り私が望む理想的な Proxy にはならない。
     * 今、思いつくのは、std::map を返却し、各リポジトリで DATA をインスタンス化する方法だが
     * できれば、各リポジトリは宣言のみで、定義は行いたくない。
     * あぁ、DataConcept で factory があればいいのかな。factory の仮引数に std::map があり、
     * 各 DATA で std::map をもとに、自身を作る。そうすることで、リポジトリを汚染することは
     * 回避できるかもしれない。
     * 
     * 値の受け渡し方法、これはいい方法が浮かばなかった、std::variant の利用も考えたが RDBMS 
     * 側のビルダパターンでの実現方法が見えなかった。したがって、これは バリエーション・ポイント
     * として、DATA 側に丸投げした方が設計上きれいになると考える。DATA::toMap() は止めて、CRUD 
     * に対応したSQLのビルダを DATA に行ってもらうということだ。
     * 
     * これで、リポジトリはストレージに左右されないものにはなった（はず：）
    */

private:
    mysqlx::Session* session;
};

int test_MySQLXBasicRepository_insert() {
    puts("=== test_MySQLXBasicRepository_insert");
    try {
        std::string expectName("Alice_01");
        std::string expectEmail("alice_01@loki.org");
        int expectAge = 12;
        PersonData alice(expectName, expectEmail, expectAge);
        mysqlx::Session sess("localhost", 33060, "derek", "derek1234");

        MySQLXBasicRepository<PersonData> basicRepo(&sess);
        PersonData ret = basicRepo.insert(alice);
        assert(ret.getName()        == expectName);
        assert(ret.getEmail()       == expectEmail);
        assert(ret.getAge().value() == expectAge);
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}


/**
 * 実際に重要なのは、DATA のシグネチャやインタフェースにあると思う。
 * 結局、SQL を動的に作り出す仕組みがほしいのだから。
 * 最初に私が作成した DataField が概念的に近いものになるのかな。
 * 
 * - カラム名、その型と制約、値
 * 
 * これを std::vector 等のコンテナで返却する メンバ関数が DATA の基底クラスにあればいい。
 * それで、CRUD の基本的なメンバ関数を 1 つの Proxy オブジェクトが生成可能だと思う。
 * 
 * 具体的な処理は MySQL のドライバや PostgreSQL のドライバになるので、それはひとまず別もの
 * として考えておく。
 * 
 * 突き詰めれば、Key（カラム名）と Value（その値）の Map でいいのかな。
 * mysqlx を例に具体的な試作を行ってみる。
 * 
 * mysqlx::Result res = person.insert("name", "email", "age")               // Key
 *                 .values("Jabberwocky", "Jabberwocky@loki.org", nullptr)  // Value、 RDBMS で NULL を許可している場合は、Key にとともに含めなければいい。
 *                 .execute();
 * 
*/

/**
 * PersonData から動的に SQL を作る
 * 
 * Step 1 必要なデータの取得。
*/

int test_PersonData_Step_1() {
    puts("=== test_PersonData_Step_1");
    try {
        // PersonData alice("Alice", "alice@loki.org", 12);
        // PersonData cheshire("Cheshire", "cheshire@loki.org");
        
        // std::map<std::string, std::string> map_a = alice.toMap();
        // puts("--- Alice Data");
        // ptr_debug<const char*, const std::string&>("table is ", alice.getTableName());
        // for(auto p: map_a) {
        //     std::cout << p.first << "\t: " << p.second << std::endl;
        // }
        // puts("--- Cheshire Data");
        // ptr_debug<const char*, const std::string&>("table is ", cheshire.getTableName());
        // std::map<std::string, std::string> map_c = cheshire.toMap();
        // for(auto p: map_c) {
        //     std::cout << p.first << "\t: " << p.second << std::endl;
        // }

        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}


/**
 * C++ で map を使ってみる
 * 
 * std::map
*/

int test_map() {
    puts("=== test_map");
    try {
        std::vector<std::string> keys;
        std::map<std::string, std::string> m {
            {"id", "33"}
            , {"name", "ABC"}
            , {"address", "N.Y."}
        };
        for(const auto& obj: m) {
            std::cout << obj.first << ", " << obj.second << std::endl;
            keys.emplace_back(obj.first);
        }
        for(const auto& key: keys) {
            ptr_debug<const char*, const decltype(key)&>("key is ", key);
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}


int main(void) 
{
    puts("START Proxy パターン（にはおそらくならない：） ===");
    if(0.01) {
        auto ret = 0;
        ptr_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_debug_error());
        assert(ret == 0);
    }
    if(1.00) {
        auto ret = 0;
        ptr_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_map());
        assert(ret == 0);
        ptr_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_PersonData_Step_1());
        assert(ret == 0);
        ptr_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_MySQLXBasicRepository_insert());
        assert(ret == 0);
    }
    puts("=== Proxy パターン（にはおそらくならない：）  END");
}