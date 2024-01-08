/**
 * 6 章 Adapter パターン、Observer パターン、CRTP パターン
 * 
 * ガイドライン 25：通知を抽象化するには Observer パターン
 * 
 * 値セマンティクスベースの Observer パターン実装
 * 
 * オブザーバの登録と解除にはオブザーバの一意な識別子が必要です。これにはオブザーバのアドレスを
 * 利用するのがもっとも簡便な方法です。そのため、登録したオブザーバを用いる部分はポインタのまま
 * とします。但し、継承階層を上手に回避する方法として std::function が使えます。
 * 
 * Observer パターンの長所と組み合わせた値セマンティクスは（即ち、イベントとその動作を分離し、新規にオブザーバを容易に追加できる）、
 * 実に良く動作します。しかし、完璧な設計など存在せず、必ず短所があります。
 * まず言えることは、update() 関数をひとつしか持たないプルオブザーバ（Pull Observer）の場合に限られる点です（クラスから関数に単純化しているため）。
 * したがって、すでに update() 関数が複数ある場合や、将来増加する場合（プッシュオブザーバ、Push Observer）では、std::function と同じアプローチは
 * 一般に採用されません。
 * マルチスレッドにおけるObserver パターンでは attach()、detach() 登録と解除に加え、スレッドセーフなイベント処理も無視できません。 
 * また、Observer パターンを多数用いると、ソフトウェアエンティティ間通信がすぐに、また過剰に、複雑化するというリスクがあります。
 * したがって、有向非巡回グラフを意識した、正しい設計を心掛ける必要があります。
 * 
 * ガイドライン 25 の要約
 * - サブジェクトとオブザーバを一対多の関係とする目的をもって、Observer パターンを適用する。
 * - プッシュオブザーバとプルオブザーバ間のトレードオフを理解する。
 * - 値セマンティクスベースの Observer パターン実装を活用する（プルオブザーバ）。
 * 
 * e.g. compile
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_6_observer2.cpp -o ../bin/main
 * 
*/
#include <iostream>
#include <functional>
#include <string>
#include <set>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * 再定義し直した オブザーバクラス
*/

template <class Subject, class StateTag>
class Observer {
public:
    using OnUpdate = std::function<void(const Subject&, StateTag)>;
    // No virtual destructor necessary.

    // 値渡しで std::move で代入が Modern C++ なのだろうか。 最初から参照でいいと思うし問題があるとも思えないのだが（std::function ... 参照が使えないのか検証してみる）。
    explicit Observer(OnUpdate ou):onUpdate{std::move(ou)} {
        // Possibly respond on an invalid/empty std::function instance.
    }

    // No-virtual update function
    void update(const Subject& subject, StateTag property) {
        onUpdate(subject,property);
    }
private:
    OnUpdate onUpdate;
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

/**
 * フリー関数として std::function で実行されるもの
 * 最初に作成した NameObserver クラスの役割と同じ、しかしこちらはただの関数で済む。
 * 関数でよいのだから ラムダ式でも無論可能ということか。
*/

void propertyChanged(const Person& person, Person::StateChange property) {
    if( property == Person::forenameChanged || property == Person::surnameChanged ) {
            // ... respond to changed name.
            puts("--- changed name.");
            ptr_lambda_debug<const char*,Person::StateChange&>("property is ",property);
            ptr_lambda_debug<const char*,const string&>("forename is ",person.getForename());
            ptr_lambda_debug<const char*,const string&>("surname is ",person.getSurname());
    }
}

int test_Observer() {
    puts("--- test_Observer");
    try {
        using PersonObserver = Observer<Person,Person::StateChange>;
        PersonObserver nameObserver(propertyChanged);      // Observer のインスタンス化には std::function で実行される、関数が必要。

        PersonObserver addressObserver(
            // ラムダ式を利用したパターン。
            [/* captured state */](const Person& person, Person::StateChange property) {
                if( property == Person::addressChanged ) {
                    // ... respond to changed address.
                    puts("--- changed address.");
                    ptr_lambda_debug<const char*,Person::StateChange&>("property is ",property);
                    ptr_lambda_debug<const char*,const string&>("address is ",person.getAddress());
                }
            }
        );

        Person homer("Homer","Simpson");
        Person marge("Marge","Simpson");
        Person monty("Montgomery","Burns");

        homer.attach(&nameObserver);
        marge.attach(&addressObserver);
        monty.attach(&addressObserver);

        homer.setForename("Homer Jay");     // Adding his middle name.
        marge.setAddress("712 Red Bark Lane, Henderson, Clark County, Nevada 09011");
        monty.setAddress("Springfield Nuclear Power Plant");

        homer.detach(&nameObserver);

        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

int main(void) {
    puts("START 値セマンティクスベースの Observer パターン ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const int&>("pi is ",pi);
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_Observer());
    }
    puts("=== 値セマンティクスベースの Observer パターン END");
    return EXIT_SUCCESS;    // return 0
}