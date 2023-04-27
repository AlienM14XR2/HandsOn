/**
  Gof Visitor パターン

  Visitor インタフェースとAcceptor インタフェース、この２つのクラスがまずある。
  それぞれに派生クラスがあり、一般的には、Visitor の派生クラスは一つ、Acceptor 
  の派生クラスは１つ以上、２つで示されることが多い。そんなクラス図で表現されている。

  知らない。だから、調べならが進める。
  実際に基本的なものを作りながら確認してみたい。 

  通常のインスタンスはシングルディスパッチと呼ぶことがある、その場合、このパターンは
  ダブルディスパッチに分類されるらしい。うん、少し冗長なのかな。
  なるほど、最後までやって少し理解できたかな、visit メンバ関数を直接呼ぶのではなく、
  Acceptor のaccept メンバ関数の引数により、visit が呼ばれる。
  うん、でも理想とは少し違うな。（@see test_visitor_basic 関数で確認した時の素直な感想ね。）
  各インタフェースの働きが弱いからかな。
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
class ConcretAcceptorB;
/**
    Visitor インタフェース（基底クラス）
*/
class Visitor {
public:
    Visitor() {}
    virtual ~Visitor() {}
    virtual void visit(Acceptor*) const = 0;
    virtual void visit(ConcretAcceptorA*) const = 0;
    virtual void visit(ConcretAcceptorB*) const = 0;
};
/**
    ConcretAcceptorA Acceptor の派生クラス
*/
class ConcretVisitor;
class ConcretAcceptorA final : public virtual Acceptor {
public:
    ConcretAcceptorA();
    ConcretAcceptorA(const ConcretAcceptorA& own);
    ~ConcretAcceptorA();
    virtual void accept() const override {
        ptr_lambda_debug<const string&,const int&>("ConcretAcceptorA default accept() ... ",0);
    }
    void accept(ConcretVisitor&);
    void concretAcceptA() {
        cout << "called concretAcceptA ... " << endl;
    }
};
class ConcretAcceptorB final : public virtual Acceptor {
public:
    ConcretAcceptorB() {}
    ConcretAcceptorB(const ConcretAcceptorB& own) {
        *this = own;
    }
    ~ConcretAcceptorB() {}
    virtual void accept() const override {
        ptr_lambda_debug<const string&,const int&>("ConcretAcceptorB default accept() ... ",0);
    }
    void accept(ConcretVisitor&);
    void concretAcceptB() {
        cout << "called concretAcceptB ... " << endl;
    }
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
        cout << "default vist ... " << endl;
    }
    virtual void visit(ConcretAcceptorA* acceptor) const override {
        cout << "vist accept by ConcretAcceptorA ... " << endl;
        acceptor->concretAcceptA();
    }
    virtual void visit(ConcretAcceptorB* acceptor) const override {
        cout << "vist accept by ConcretAcceptorB ... " << endl;
        acceptor->concretAcceptB();
    }
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
void ConcretAcceptorA::accept(ConcretVisitor& visitor) {
    visitor.visit(this);
}

void ConcretAcceptorB::accept(ConcretVisitor& visitor) {
    visitor.visit(this);
}

int test_visitor_basic() {
    cout << "------------------------------------ test_visitor_basic" << endl;
    try {
        ConcretAcceptorA acceptorA;
        ConcretAcceptorB acceptorB;
        Acceptor* aInterface = &acceptorA;
        ConcretVisitor concretVisitor;
        aInterface->accept();
        aInterface = &acceptorB;
        aInterface->accept();
        acceptorA.accept(concretVisitor);
        acceptorB.accept(concretVisitor);
        // Acceptor* aInterface = static_cast<Acceptor*>(&acceptorA);
        // visitor.visit(aInterface);
    } catch(exception& e) {
        cerr << e.what() << endl;
    }
    return 0;
}

int main() {
    cout << "START GoF Visitor ===============" << endl;
    ptr_lambda_debug<const string&,const int&>("Here we go :)",0);
    if(1) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_visitor_basic());
    }
    cout << "=============== GoF Visitor END" << endl;
    return 0;
}