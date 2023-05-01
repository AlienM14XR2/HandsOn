/**
  Facadeパターン

  聞いたことはあるし、もしかしたらどこかで使ったことがあるかもしれない。
  しかし、今は思い出せない、つまり知らない：）

  複雑な処理に対する「窓口」の提供を行うもの。
  「建物の正面」という意味らしい。
  因みに読み方は知っていた、「ファサード」だ。

  Facadeクラスはあくまでサブシステム内部に仕事を投げるだけで複雑な実装は持たない。

  多様な機能の塊であるサブシステムから、サブシステムを利用するユーザーの用途に
  合わせた窓口(インターフェース)を提供するだけである。

  Facadeクラスをサブシステム自体が利用する事はない。

  Facadeクラスはあくまでサブシステム末端の窓口であるため、同じサブシステムから利用される事はない。

  Facadeパターンはサブシステムの直接使用を妨げない。

  Facadeクラスの利用は強制ではなく、必要であればサブシステムの機能を直接利用できる。
  言語によっては無名名前空間やPackageスコープによりサブシステムを利用者から隔離できるが、
  Facadeパターンはそのような制限はしない。

  Facade クラスが1つ。
  Facade を介して、ClassA ClassB ClassC ClassD へのアクセスが可能。
  そんなクラス図で示される。

  なるほど、Facade 自体は非常に単純なのだね。インタフェース（仮想基底クラス）にする必要もないかな。
  サブシステムに対するAPI をメンバ関数にガンガン追加していけばいい。
  OK、ここまでの復習を兼ねて、これまでGoF として学んだパターンをサブシステムとして
  Facade を通して利用してみようか。

  - visitor pattern
  - chain of responsibility

  直近の2つにした。

  【気付き】
  this とはコンストのポインタである。

*/
#include <iostream>

using namespace std;

template<class M,class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << message << '\t' << debug << endl;
};

template<class V>
class Acceptor {
public:
    virtual ~Acceptor() {}
    virtual void accept(V) const = 0;
    virtual void doSomething() const = 0;
};
template<class A>
class Visitor {
public:
    virtual ~Visitor() {}
    virtual void visit(A) const = 0;
};
class ConcreteVisitor final : public virtual Visitor<const Acceptor<const ConcreteVisitor*>*> {
public:
    ConcreteVisitor() {}
    ConcreteVisitor(const ConcreteVisitor& own) {
        *this = own;
    }
    ~ConcreteVisitor() {}
    virtual void visit(const Acceptor<const ConcreteVisitor*>* acc) const override {
        acc->doSomething();
    }
};
class AcceptorA final : public virtual Acceptor<const ConcreteVisitor*> {
public:
    AcceptorA(){}
    AcceptorA(const AcceptorA& own) {
        *this = own;
    }
    ~AcceptorA() {}
    virtual void accept(const ConcreteVisitor* visitor) const override {
        visitor->visit(this);
    }
    virtual void doSomething() const override {
        cout << "do something ... AcceptorA." << endl;
    }
};
int test_visitor_step_1() {
    cout << "------------------------------------- test_visitor_step_1" << endl;
    ConcreteVisitor visitor;
    const ConcreteVisitor* pvis = &visitor;
    AcceptorA acceptorA;
    acceptorA.accept(pvis);
    return 0;
}
int main() {
    cout << "START GoF Facade ===============" << endl;
    ptr_lambda_debug<const string&,const int&>("Yeah Here we go.",0);
    if(1) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_visitor_step_1());
    }
    cout << "=============== GoF Facade END" << endl;
    return 0;
}