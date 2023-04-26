/**
  Gof Visitor パターン

  Visitor インタフェースとAcceptor インタフェース、この２つのクラスがまずある。
  それぞれに派生クラスがあり、一般的には、Visitor の派生クラスは一つ、Acceptor 
  の派生クラスは１つ以上、２つで示されることが多い。そんなクラス図で表現されている。

  知らない。だから、調べならが進める。
  実際に基本的なものを作りながら確認してみたい。 

  通常のインスタンスはシングルディスパッチと呼ぶことがある、その場合、このパターンは
  ダブルディスパッチに分類されるらしい。うん、少し冗長なのかな。
*/
#include <iostream>
#include <cassert>

using namespace std;

template<class M,class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << message << '\t' << debug << endl;
};

/**
    Accepter インタフェース（基底クラス）
*/
class Acceptor {
public:
    virtual ~Acceptor() {}
    virtual void accept() const = 0;
};
class ConcretAcceptorA;
/**
    Visitor インタフェース（基底クラス）
*/
class Visitor {
public:
    Visitor() {}
    virtual ~Visitor() {}
    virtual void visit(Acceptor*) const = 0;
    virtual void visit(ConcretAcceptorA*) const = 0;
    // virtual void visit(ConcretAcceptorB*) const = 0;
};
/**
    ConcretAcceptorA Acceptor の派生クラス
*/
class ConcretAcceptorA final : public virtual Acceptor {
public:
    ConcretAcceptorA();
    ConcretAcceptorA(const ConcretAcceptorA& own);
    ~ConcretAcceptorA();
    virtual void accept() const override {
        ptr_lambda_debug<const string&,const int&>("ConcretAcceptorA default accept() ... ",0);
    }
    void accept(const Visitor*);
};
/**
    ConcretVisitor Visitor の派生クラス
*/
class ConcretVisitor final : public virtual Visitor {
public:
    ConcretVisitor();
    ConcretVisitor(const ConcretVisitor& own);
    ~ConcretVisitor();
    virtual void visit(Acceptor* acceptor) const override {
        acceptor->accept();
    }
    virtual void visit(ConcretAcceptorA* acceptor) const override {
        acceptor->accept(this);
    }
    // virtual void visit(ConcretAcceptorB* acceptor) const override {
    //     acceptor->accept(*this);
    // }
};
ConcretVisitor::ConcretVisitor() {}
ConcretVisitor::ConcretVisitor(const ConcretVisitor& own) {
    *this = own;
}
ConcretVisitor::~ConcretVisitor() {}
ConcretAcceptorA::ConcretAcceptorA() {}
ConcretAcceptorA::ConcretAcceptorA(const ConcretAcceptorA& own) {
    *this = own;
}
ConcretAcceptorA::~ConcretAcceptorA() {}
void ConcretAcceptorA::accept(const Visitor* visitor) {
    visitor->visit(this);
}

// class ConcretAcceptorB final : public virtual Acceptor {
// public:
//     ConcretAcceptorB() {}
//     ConcretAcceptorB(const ConcretAcceptorB& own) {
//         *this = own;
//     }
//     ~ConcretAcceptorB() {}
//     virtual void accept() const override {
//         ptr_lambda_debug<const string&,const int&>("ConcretAcceptorB default accept() ... ",0);
//     }
//     void accept(ConcretVisitor& visitor) {
//         visitor.visit(this);
//     }
// };

int main() {
    cout << "START GoF Visitor ===============" << endl;
    ptr_lambda_debug<const string&,const int&>("Here we go :)",0);
    cout << "=============== GoF Visitor END" << endl;
    return 0;
}