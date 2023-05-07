/**
  Observer パターン

  はい、知りません：）なんだっけ？

  Observer パターンを学びます。Observer とは、英語で「観察者」を意味する単語です。
  Observer パターンとは、状態の変化を観察することを目的としたものですが、どちらかというと
  「観察」よりも「通知」に重点の置かれたものになっています。

  だそうです。

  Subject、 observerList をメンバ変数に持つ。Observer を集約している。
  addObserver メンバ関数、notifyObservers メンバ関数、done メンバ関数を持つ。
  Observer、 update メンバ関数を宣言したインタフェース。
  Subject Observer それぞれに派生クラスが複数存在する。

  そんな、クラス図。
  やっぱり、分からないから基本のクラスを作っていく。
*/
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

class Observer {
public:
    virtual ~Observer() {}
    virtual int update() const = 0; 
};
class Subject {
protected:
    vector<Observer*> observerList;
public:
    Subject() {}
    Subject(const Subject& own) {
        *this = own;
        this->observerList = own.observerList;
    }
    virtual ~Subject() {}
    void addObserver(Observer* ob) noexcept {
        observerList.push_back(ob);
    }
    void notifyObservers() noexcept {
        for(Observer* o: observerList) {
            o->update();
        }
    }
    virtual int done() const = 0;
};
class ConcreteSubjectA final : public virtual Subject {
public:
    ConcreteSubjectA() {}
    ConcreteSubjectA(const ConcreteSubjectA& own) {
        *this = own;
        this->observerList = own.observerList;
    }
    ~ConcreteSubjectA() {}
    virtual int done() const override {
        cout << "done ... ConcreteSubjectA." << endl;
        return 0;
    }
};

int main() {
    cout << "START Observer ===============" << endl;
    cout << "=============== Observer END" << endl;
    return 0;
}