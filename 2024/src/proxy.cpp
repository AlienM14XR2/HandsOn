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

template <class DATA, class PKEY>
class Repository {
public:
    virtual ~Repository() = default;
    // ... 
    virtual DATA insert(const DATA&) const = 0;
    virtual DATA update(const DATA&) const = 0;
    virtual void remove(const PKEY&) const = 0;

};

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
    puts("START Proxy パターン（であっているのかそれは分からない ===");
    if(0.01) {
        auto ret = 0;
        ptr_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_debug_error());
        assert(ret == 0);
    }
    if(1.00) {
        auto ret = 0;
        ptr_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_map());
        assert(ret == 0);
    }
    puts("=== Proxy パターン（であっているのかそれは分からない） END");
}