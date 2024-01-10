/**
 * 7 章 Bridge パターン、Prototype パターン、External Polymorphism パターン
 * 
 * Pimpl( Pointer-to-implementation ) イディオム
 * Bridge パターンにはずっち簡潔な形があり、C 及び C++ では数十年にわたり非常に広く使用されています。
 * Person クラスを例に考えてみましょう。
 * 
 * e.g. compile
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_7_bridge2.cpp -o ../bin/main
*/
#include <iostream>
#include <string>
#include <memory>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

class Person {
public:
    Person();
    ~Person();
    Person(const Person& other);
    Person& operator=(const Person& other);
    Person(Person&& other);
    Person& operator=(Person&& other);

    string getForename() const;
    string getSurname() const;
    string getAddress() const;
    int getYear_of_birth() const;
private:
    struct Impl;
    std::unique_ptr<Impl> const pimpl;
};

struct Person::Impl {
private:
    std::string forename;
    std::string surname;
    std::string address;
    int year_of_birth;
    // ... Potentially many more data members
public:
    string getForename() const {
        return forename;
    }
    void setForename(const string& fn) {
        forename = fn;
    }
    string getSurname() const {
        return surname;
    }
    void setSurname(const string& sn) {
        surname = sn;
    }
    string getAddress() const {
        return address;
    }
    void setAddress(const string& addr) {
        address = addr;
    }
    int getYear_of_birth() const {
        return year_of_birth;
    }
    void setYear_of_birth(const int& y) {
        year_of_birth = y;
    }
    
    // ... Many more access functions
};

/**
 * 上例の Person 実装はもっとも単純な形の Bridge パターンです。このようにローカルでも多態性も
 * 持たない形の Bridge パターンを、Pimpl イディオムと言います。
 * Bridge パターンの分離する長所はそのまま活きていますが、その単純さとは裏腹に、Person クラスの実装
 * がやや複雑になってしまう点はあまり変わっていません。
*/

    Person::Person():pimpl{std::make_unique<Impl>()} 
    {}
    Person::~Person() = default;
    Person::Person(const Person& other):pimpl{std::make_unique<Impl>(*other.pimpl)} 
    {}
    Person& Person::operator=(const Person& other) {
        *pimpl = *other.pimpl;
        return *this;
    }
    Person::Person(Person&& other):pimpl{std::make_unique<Impl>(std::move(*other.pimpl))}
    {}
    Person& Person::operator=(Person&& other) {
        *pimpl = std::move(*other.pimpl);
        return *this;
    }
    string Person::getForename() const {
        return pimpl->getForename();
    }
    string Person::getSurname() const {
        return pimpl->getSurname();
    }
    string Person::getAddress() const {
        return pimpl->getAddress();
    }
    int Person::getYear_of_birth() const {
        return pimpl->getYear_of_birth();
    }

/**
 * 上記のような Person クラスのメンバ変数を隠蔽し、ABI の安定性を維持するには、Person の実装詳細に Bridge パターンを適用します。
 * しかし、この例に限っては基底クラスという形の抽象化は不要です。Person の実装は 1 つしか存在しないため、private なネストした Impl と
 * いう名前のクラスを導入すれば良いのです。
*/


int main(void) {
    puts("START Pimpl( Pointer-to-implementation ) イディオム ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    puts("=== Pimpl( Pointer-to-implementation ) イディオム END");
    return 0;
}