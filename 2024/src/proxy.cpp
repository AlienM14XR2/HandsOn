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

class ORMData {
public:
    virtual ~ORMData() = default;
    virtual std::string getTableName() const = 0;
    virtual std::map<std::string, std::string> toMap() const = 0;
};

class PersonData final : public ORMData {
public:
    PersonData(const std::size_t& _id
            , const std::string& _name
            , const std::string& _email
            , const int& _age ): id(_id), name(_name), email(_email), age(_age)
    {}
    PersonData(const std::string& _name
            , const std::string& _email
            , const int& _age ): id(0ul), name(_name), email(_email), age(_age)
    {}
    PersonData(const std::string& _name
            , const std::string& _email ): id(0ul), name(_name), email(_email), age(std::nullopt)
    {}
    // ...
    virtual std::string getTableName() const override
    {
        return "person";
    }
    virtual std::map<std::string, std::string> toMap() const override
    {
        // TODO 次の点を踏まえて、PKEY の std::optional を考慮してくれ。
        // INSERT 文 で Auto-Increment を採用している場合は、PKEY は Key Value には不要なんだよな。
        std::map<std::string, std::string> m{
            {"id", std::to_string(id)}
            , {"name", name}
            , {"email", email}
        };
        if(age.has_value()) {
            m.insert(std::make_pair("age", std::to_string(age.value())));
        }
        return m;
    }

    std::size_t getId()         const { return id; }
    std::string getName()       const { return name; }
    std::string getEmail()      const { return email; }
    std::optional<int> getAge() const { return age; }
private:
    // TODO PKEY に該当するデータも std::optional の方が扱い易くないか、考えてみる。
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
concept DataConcept = requires(DATA& data) {
    data.getTableName();
    data.toMap();
};
template<class DATA>
requires DataConcept<DATA>
class BasicRepository {
public:
    virtual std::string insertSql(const DATA& data)  const
    {
        puts("------ BasicRepository::insertSql()");
        ptr_debug<const char*, const std::string&>("table is ", data.getTableName());
        std::map<std::string, std::string> m = data.toMap();
        for(auto p: m) {
            std::cout << p.first << "\t: " << p.second << std::endl;
        }
        return std::string();
    }
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
    */
    // virtual std::string updateSql(const DATA& data)  const = 0;
    // virtual std::string removeSql(const DATA& data, const std::string& pkeyName)  const = 0;
    // virtual std::string findOne(const DATA& data, const std::string& pkeyName) const = 0;
};

int test_BasicRepository() {
    puts("=== test_BasicRepository");
    try {
        PersonData alice("Alice", "alice@loki.org", 12);
        BasicRepository<PersonData> basicRepo;
        basicRepo.insertSql(alice);
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
        PersonData alice("Alice", "alice@loki.org", 12);
        PersonData cheshire("Cheshire", "cheshire@loki.org");
        
        std::map<std::string, std::string> map_a = alice.toMap();
        puts("--- Alice Data");
        ptr_debug<const char*, const std::string&>("table is ", alice.getTableName());
        for(auto p: map_a) {
            std::cout << p.first << "\t: " << p.second << std::endl;
        }
        puts("--- Cheshire Data");
        ptr_debug<const char*, const std::string&>("table is ", cheshire.getTableName());
        std::map<std::string, std::string> map_c = cheshire.toMap();
        for(auto p: map_c) {
            std::cout << p.first << "\t: " << p.second << std::endl;
        }

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
    puts("START Proxy パターン（であっているのかそれは分からない） ===");
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
        ptr_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_BasicRepository());
        assert(ret == 0);
    }
    puts("=== Proxy パターン（であっているのかそれは分からない）  END");
}