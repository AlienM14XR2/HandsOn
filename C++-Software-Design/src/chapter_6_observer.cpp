/**
 * 6 章 Adapter パターン、Observer パターン、CRTP パターン
 * 
 * CRTP（Curiously Recurring Template Pattern）... 直訳 ... 奇妙なことに繰り返し発生するテンプレート パターン。
 * 
 * ガイドライン 25：通知を抽象化するには Observer パターン
 * 
 * サブジェクト（観察対象または単に対象、観察されるソフトウェアエンティティ、状態変化が発生する箇所）と
 * 複数存在するかもしれない、オブザーバ（状態変化に応じた通知を受け取るコールバック）との分離こそ、
 * Observer パターンの目的です。
 * 
 * Observer パターン
 * 目的：あるオブジェクトが状態を変えたときに、それに依存するすべてのオブジェクトに自動的にそのことが知らされ、また、
 *      それらが更新されるように、オブジェクト間に一対多の依存関係を定義する。
 * 
 * すべてのデザインパターンがそうですが、Observer パターンも、変更もしくはその予定がある部分をパリエーションポイント（Variation Point）
 * として切り出し、なんらかの形で抽象化します。
 * 
 * e.g. compile
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_6_observer.cpp -o ../bin/main
*/
#include <iostream>
#include <string>
#include <set>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * Observer パターンの古典的実装
 * 
 * 複数存在する状態変化へ対応するためにテンプレートを利用するように改変した。
*/

template<class Subject, class StateTag>
class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(const Subject& subject, StateTag property) = 0;
    /**
     * 次のようにも定義できる。
     * 
     * virtual void update1( arguments representing the updated state ) = 0
     * virtual void update2( arguments representing the updated state ) = 0
    */

};

/**
 * 観察対象となる人を表す Person クラス
*/

class Person {
public:
    enum StateChange {
        forenameChanged,
        surnameChanged,
        addressChanged,
    };

    using PersonObserver = Observer<Person,StateChange>;    // サブジェクトは複数のオプザーバを集約して持つ。

    // 値渡しして move() より const string& の方がいいと個人的には思う。
    explicit Person(string fname, string sname): forename{move(fname)},surname{move(sname)} {}
    Person(const Person& own) {*this = own;}
    ~Person() {}

    bool attach(PersonObserver* po) {
        auto [pos,success] = observers.insert(po);  // この書き方初めて見る、pair を返却するからか。
        return success;
    }
    bool detach(PersonObserver* po) {
        return (observers.erase(po) > 0U);          // 最後の 0U 興味深い。
    }
    void notify(StateChange property) {
        for(auto iter = begin(observers); iter != end(observers); ) {
            const auto pos = iter++;
            (*pos)->update(*this,property);         // Observer（観察者）に通知している。
        }
    }
    void setForename(string newForename) {  // 値渡しして move() より const string& の方がいいと個人的には思う。
        forename = move(newForename);
        notify(forenameChanged);
    }
    void setSurname(string newSurname) {
        surname = move(newSurname);
        notify(surnameChanged);
    }
    void setAddress(string newAddress) {
        address = move(newAddress);
        notify(addressChanged);
    }
    const string& getForename() const { return forename; }
    const string& getSurname() const { return surname; }
    const string& getAddress() const { return address; }
private:
    string forename;
    string surname;
    string address;
    set<PersonObserver*> observers;
};

class NameObserver final : public virtual Observer<Person,Person::StateChange> {
public:
    void update(const Person& person, Person::StateChange property) override {
        if( property == Person::forenameChanged || property == Person::surnameChanged ) {
            // ... respond to changed name.
            puts("--- changed name.");
            ptr_lambda_debug<const char*,Person::StateChange&>("property is ",property);
            ptr_lambda_debug<const char*,const string&>("forename is ",person.getForename());
            ptr_lambda_debug<const char*,const string&>("surname is ",person.getSurname());
        }
    }
};

class AddressObserver final : public virtual Observer<Person,Person::StateChange> {
public:
    void update(const Person& person, Person::StateChange property) override {
        if( property == Person::addressChanged ) {
            // ... respond to changed address.
            puts("--- changed address.");
            ptr_lambda_debug<const char*,Person::StateChange&>("property is ",property);
            ptr_lambda_debug<const char*,const string&>("address is ",person.getAddress());
        }
    }
};

int test_NameObserver_AddressObserver() {
    puts("--- test_NameObserver_AddressObserver");
    try {
        NameObserver nameOb;
        AddressObserver addressOb;

        Person homer("Homer","Simpson");
        Person marge("Marge","Simpson");
        Person monty("Montgomery","Burns");

        homer.attach(&nameOb);
        marge.attach(&addressOb);
        monty.attach(&addressOb);

        homer.setForename("Homer Jay");     // Adding his middle name.
        marge.setAddress("712 Red Bark Lane, Henderson, Clark County, Nevada 09011");
        monty.setAddress("Springfield Nuclear Power Plant");

        homer.detach(&nameOb);
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

int main(void) {
    puts("START Observer パターン ===");
    if(0.01) {
        double pi = 3.14159;
        ptr_lambda_debug<const char*,const int&>("pi is ",pi);
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_NameObserver_AddressObserver());
    }
    puts("=== Observer パターン END");
    return 0;
}
