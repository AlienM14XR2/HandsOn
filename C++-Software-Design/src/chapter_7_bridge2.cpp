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
    void setForename(const string&);
    void setSurname(const string&);
    void setAddress(const string&);
    void setYear_of_birth(const int&);
private:
    struct Impl;
    std::unique_ptr<Impl> const pimpl;
};

struct Person::Impl {
    std::string forename;
    std::string surname;
    std::string address;
    int year_of_birth;
    // ... Potentially many more data members
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
        return pimpl->forename;
    }
    string Person::getSurname() const {
        return pimpl->surname;
    }
    string Person::getAddress() const {
        return pimpl->address;
    }
    int Person::getYear_of_birth() const {
        return pimpl->year_of_birth;
    }
    void Person::setForename(const string& fn) {
        pimpl->forename = fn;
    }
    void Person::setSurname(const string& sn) {
        pimpl->surname = sn;
    }
    void Person::setAddress(const string& addr) {
        pimpl->address = addr;
    }

    void Person::setYear_of_birth(const int& y) {
        pimpl->year_of_birth = y;
    }

/**
 * 上記のような Person クラスのメンバ変数を隠蔽し、ABI の安定性を維持するには、Person の実装詳細に Bridge パターンを適用します。
 * しかし、この例に限っては基底クラスという形の抽象化は不要です。Person の実装は 1 つしか存在しないため、private なネストした Impl と
 * いう名前のクラスを導入すれば良いのです。
*/

int test_Person() {
    puts("--- test_Person");
    try {
        Person person;
        person.setForename("Jack");
        person.setSurname("Johnson");
        person.setAddress("Tokyo, Japan. :)");       
        person.setYear_of_birth(2001);
        ptr_lambda_debug<const char*,const string&>("forename is ",person.getForename());
        ptr_lambda_debug<const char*,const string&>("surname is ",person.getSurname());
        ptr_lambda_debug<const char*,const string&>("address is ",person.getAddress());
        ptr_lambda_debug<const char*,const int&>("year of birth is ",person.getYear_of_birth());
        // throw runtime_error("It's a test exception.");
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

int main(void) {
    puts("START Pimpl( Pointer-to-implementation ) イディオム ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_Person());
    }
    puts("=== Pimpl( Pointer-to-implementation ) イディオム END");
    return 0;
}