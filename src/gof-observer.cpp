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

template<class M,class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << message << '\t' << debug << endl;
};

class Observer {
public:
    virtual ~Observer() {}
    virtual int update() const = 0; 
};
class ConcreteObserverA final : public virtual Observer {
    // 久々に出てきた mutable うん、これがないとオーバーライドライドした関数内で値の変更できない。
    mutable int status;     // これが、別の定義されたクラスオブジェクトであってもいいはず。
public:
    ConcreteObserverA() {
        status = 0;
    }
    ConcreteObserverA(const ConcreteObserverA& own) {
        *this = own;
        this->status = own.status;
    }
    ~ConcreteObserverA() {}
    virtual int update() const override {
        if(status == 0) {
            status = 1;
        } else if(status == 1) {
            status = 0;
        }
        return status;
    }
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
    void addObserver(Observer* ob) {
        observerList.push_back(ob);
    }
    void notifyObservers() {
        cout << "notifyObservers ... Subject." << endl;
        for(Observer* o: observerList) {
            cout << "update is " << o->update() << endl;
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
        // これで隠蔽して楽できると思ったが、コンパイルエラーになった。
        // 時間のある時に調べる。this の候補が複数あるのでダメだと叱られた。
        // Java の super に相当するものが、c++ にはない：）
        // 通知、この場合 notifyObservers 関数だが、それが正しく行われた場合に限り、
        // done 関数を実行したい。
        // notifyObservers();
        return 0;
    }
};
int test_Basic_Observer() {
    cout << "--------------------------------- test_Basic_Observer" << endl;
    ConcreteObserverA obA;
    Observer* interface = static_cast<Observer*>(&obA);
    ConcreteSubjectA subA;
    subA.addObserver(interface);
    try {
        subA.notifyObservers();
        subA.done();
    } catch(exception& e) {
        cerr << e.what() << endl;
        return -1;
    }
    try {
        subA.notifyObservers();
        subA.done();
    } catch(exception& e) {
        cerr << e.what() << endl;
        return -1;
    }
    return 0;
}
int main() {
    cout << "START Observer ===============" << endl;
    if(1) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ", test_Basic_Observer());
    }
    cout << "=============== Observer END" << endl;
    return 0;
}