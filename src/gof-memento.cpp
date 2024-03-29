/**
  GoF Memento パターン

  「記念品」、思い出のスナップショットね、これは分かりやすい。
  Virtual Box のスナップショットの概念がそのまんまかな、どうだろ。

  クラス図を言葉で説明してみる。（UML クラス図の理解が甘いので鵜呑みにするな。
  https://dofactory.com/net/memento-design-pattern

  - Caretaker クラス（留守番）があり、Memento クラスはそれに集約されている。
    - メンバ変数に C++ でいうコンテナオブジェクトなりを持つ（vector, map 等）。

  - Memento クラス（記念品）
    - メンバ変数に state を持つ。
    - メンバ関数に getState()を持つ。
    - メンバ関数に setState()を持つ。 参考では実装となっているが、これが気にいらない。カプセル化、コンストラクタで生成し、一元性、一意性を担保した方が美しく感じる。

  - Originator クラス（創造者）
    - Memento オブジェクトの生成を行うもの。
    - メンバ変数に state を持つ。
    - メンバ関数に createMemento()を持つ。
      - return new Memento(state)
    - メンバ関数に setMemento(in Memento)を持つ。
      - state = m.getState()
*/
#include <iostream>
#include <vector>

using namespace std;

template<class M,class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << message << '\t' << debug << endl;
};

// UML クラス図の「集約」これにはクラスの依存関係はない。まずい、こいつはミスったな：）少なくとも、Mediator は見直す必要がある。

class Memento {
    Memento(){}
protected:
    int state;  // これは、新たに定義されたクラスのオブジェクトでも問題ない。
public:
    Memento(const int& state_) {
        state = state_;
    }
    Memento(const Memento& own) {
        *this = own;
        this->state = own.state;
    }
    virtual ~Memento() {}
    int getState() noexcept {
        return state;
    }
};
class Caretaker final {
    vector<Memento> snaps;
public:
    Caretaker() {}
    Caretaker(const Caretaker& own) {
        *this = own;
        this->snaps = own.snaps;
    }
    ~Caretaker() {}
    void add(Memento& m) noexcept {
        snaps.push_back(m);
    }
    vector<Memento>& getSnaps() noexcept {
        return snaps;
    }
};
class Originator final {
    int state;
public:
    Originator():state{-1}{}
    Originator(const Originator& own) {
        *this = own;
        this->state = own.state;
    }
    ~Originator() {}
    void setMemento(Memento& m) noexcept {
        state = m.getState();
    }
    Memento* createMemento(const int& num) {
        return new Memento(num);
    }
    int getState() noexcept {
        return state;
    }
};
int test_Basic_Memento() {
    cout << "--------------------------------------- test_Basic_Memento" << endl;
    Originator origin;
    Memento* pm_1 = origin.createMemento(20230515);
    origin.setMemento(*pm_1);
    ptr_lambda_debug<const string&,const int&>("Memento state is ", pm_1->getState());
    ptr_lambda_debug<const string&,const int&>("Originator state is ", origin.getState());
    Caretaker care;
    care.add(*pm_1);
    Memento* pm_2 = origin.createMemento(20230516);
    origin.setMemento(*pm_2);
    ptr_lambda_debug<const string&,const int&>("Memento state is ", pm_2->getState());
    ptr_lambda_debug<const string&,const int&>("Originator state is ", origin.getState());
    care.add(*pm_2);
    vector<Memento> snaps = care.getSnaps();
    for(Memento m: snaps) {
        ptr_lambda_debug<const string&,const int&>("snap shot is ", m.getState());
    }
    // うん、これは理解したよ：）オレのなかでは、完全にスナップショットだな、ログであり、履歴。
    return 0;
}

int main() {
    cout << "START GoF Memento ===============" << endl;
    ptr_lambda_debug<const string&,const int&>("Here we go.",0);
    if(1) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ", test_Basic_Memento());
    }
    cout << "=============== GoF Memento END" << endl;
    return 0;
}