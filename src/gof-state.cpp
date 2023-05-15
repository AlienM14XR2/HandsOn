/**
  GoF State パターン

  State パターンとは、 モノではなく、「状態」をクラスとして表現するパターンです。 

  https://ja.wikipedia.org/wiki/State_%E3%83%91%E3%82%BF%E3%83%BC%E3%83%B3#/media/%E3%83%95%E3%82%A1%E3%82%A4%E3%83%AB:State_Design_Pattern_UML_Class_Diagram.svg
  UML のクラス図をいつものように言葉で説明してみる。

  Context クラス（文脈）がある、このクラスはState クラスを集約している。
    - request() メンバ関数を持つ。（state.handle()の呼び出し） 
  State クラス（状態）がある、Context クラスに集約されている。
    - handle() メンバ関数を持つ。
  ConcreteStateA クラス、State の派生クラス、具象化。
  ConcreteStateB クラス、State の派生クラス、具象化。

  これは分かりやすい、State の具象化クラスで状態変化を起こしていく、別な言い方をすれば、これらで
  状態を管理していくのだね、このIF 以外からの状態変化は認めない。

*/
#include <iostream>

using namespace std;

template<class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << message << '\t' << debug << endl;
};

class State {
public:
    virtual ~State() {}
    virtual int handle() const = 0;
};
class Context final {
    int state;
public:
    Context():state{-1} {}
    Context(const Context& own) {
        *this = own;
        this->state = own.state;
    }
    ~Context() {}
    int request(State* ps) {
        state = ps->handle();
        return state; 
    }
    int getState() noexcept {
        return state;
    }
};
class ConcreteStateA final : public virtual State {
public:
    virtual int handle() const override {
        return 0;
    }
};
class ConcreteStateB final : public virtual State {
public:
    virtual int handle() const override {
        return 1;
    }
};
class ConcreteStateC final : public virtual State {
public:
    virtual int handle() const override {
        return 2;
    }
};
int test_Basic_State() {
    cout << "--------------------------- test_Basic_State" << endl;
    Context context;
    ptr_lambda_debug<const string&,const int&>("state of context is ",context.getState());
    ConcreteStateA stateA;
    State* interface = static_cast<State*>(&stateA);
    context.request(interface);
    ptr_lambda_debug<const string&,const int&>("state of context is ",context.getState());
    ConcreteStateB stateB;
    interface = static_cast<State*>(&stateB);
    context.request(interface);
    ptr_lambda_debug<const string&,const int&>("state of context is ",context.getState());
    ConcreteStateC stateC;
    interface = static_cast<State*>(&stateC);
    context.request(interface);
    ptr_lambda_debug<const string&,const int&>("state of context is ",context.getState());
    return 0;
}

int main() {
    cout << "START GoF State ===============" << endl;
    ptr_lambda_debug<const string&,const int&>("Here we go.",0);
    if(1) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_Basic_State());
    }
    cout << "=============== GoF State END" << endl;
    return 0;
}