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
  - bridge

  直近の3つにした。

  【気付き】
  this とはその型のコンストのポインタである。

*/
#include <iostream>
#include <algorithm>
#include <cassert>
#include <string>
#include "stdio.h"

using namespace std;

template<class M,class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << message << '\t' << debug << endl;
};

//
// GoF Visitor
//

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
class AcceptorB final : public virtual Acceptor<const ConcreteVisitor*> {
public:
    AcceptorB(){}
    AcceptorB(const AcceptorB& own) {
        *this = own;
    }
    ~AcceptorB() {}
    virtual void accept(const ConcreteVisitor* visitor) const override {
        visitor->visit(this);
    }
    virtual void doSomething() const override {
        cout << "do something ... AcceptorB." << endl;
    }
};
int test_visitor_step_1() {
    cout << "------------------------------------- test_visitor_step_1" << endl;
    ConcreteVisitor visitor;
    const ConcreteVisitor* pvis = &visitor;
    AcceptorA acceptorA;
    acceptorA.accept(pvis);
    AcceptorB acceptorB;
    acceptorB.accept(pvis);
    return 0;
}
int test_visitor_step_2() {
    cout << "------------------------------------- test_visitor_step_2" << endl;
    AcceptorA a;
    Acceptor<const ConcreteVisitor*>* interface = static_cast<Acceptor<const ConcreteVisitor*>*>(&a);
    ConcreteVisitor visitor;
    const ConcreteVisitor* pvis = &visitor;
    interface->accept(pvis);
    AcceptorB b;
    interface = static_cast<Acceptor<const ConcreteVisitor*>*>(&b);
    interface->accept(pvis);
    return 0;
}

//
// GoF Chain of Responsibility
//

class Handler {
protected:
    const Handler* next = nullptr;
public:
    virtual ~Handler() {}
    virtual int request() const = 0;
};
class ProcA final : public virtual Handler {
public:
    ProcA() {
        this->next = nullptr;
    }
    ProcA(const Handler& handler) {
        this->next = &handler;
    }
    ProcA(const ProcA& own) {
        *this = own;
        this->next = own.next;
    }
    ~ProcA() {}
    virtual int request() const override {
        cout << "called request of ProcA ... " << endl;
        if(this->next == nullptr) {
            return -1;
        } else {
            return 0;
        }
    }
};
class ProcB final : public virtual Handler {
public:
    ProcB() {
        this->next = nullptr;
    }
    ProcB(const Handler& handler) {
        this->next = &handler;
    }
    ProcB(const ProcB& own) {
        *this = own;
        this->next = own.next;
    }
    ~ProcB() {}
    virtual int request() const override {
        cout << "called request of ProcB ... " << endl;
        if(this->next == nullptr) {
            return -1;
        } else {
            return 0;
        }
    }
};
class ProcC final : public virtual Handler {
public:
    ProcC() {
        this->next = nullptr;
    }
    ProcC(const Handler& handler) {
        this->next = &handler;
    }
    ProcC(const ProcC& own) {
        *this = own;
        this->next = own.next;
    }
    ~ProcC() {}
    virtual int request() const override {
        cout << "called request of ProcC ... " << endl;
        if(this->next == nullptr) {
            return -1;
        } else {
            return 0;
        }
    }
};
class ProcD final : public virtual Handler {
public:
    ProcD() {
        this->next = nullptr;
    }
    ProcD(const Handler& handler) {
        this->next = &handler;
    }
    ProcD(const ProcD& own) {
        *this = own;
        this->next = own.next;
    }
    ~ProcD() {}
    virtual int request() const override {
        cout << "called request of ProcD ... " << endl;
        if(this->next == nullptr) {
            return -1;
        } else {
            return 0;
        }
    }
};
int test_chain_of_responsibility() {
    cout << "------------------------------- test_chain_of_responsibility" << endl;
    ProcD d;
    ProcC c(d);
    ProcB b(c);
    ProcA a(b);
    Handler* interface = static_cast<Handler*>(&a);
    int i = 1;
    while( interface->request() == 0 ) {
        switch(i) {
            case 1:
                interface = static_cast<Handler*>(&b);
                break;
            case 2:
                interface = static_cast<Handler*>(&c);
                break;
            case 3:
                interface = static_cast<Handler*>(&d);
                break;            
        }
        i++;
    }
    return 0;
}

//
// GoF Bridge
//

/**
  Implementer の派生クラスはリファレンスの「ユーティリティ」から数値の比較、max() 関数をラップしたクラスにして、
  Abstraction の派生クラスはやはり、リファレンスの「文字列」から何かを使う：）
*/

template<class T,class... ArgTypes>
class Implementer {
public:
    virtual ~Implementer() {}
    virtual T compute(ArgTypes...) const = 0;
    virtual T reverseCompute(ArgTypes...) const = 0;
};
class CompTwoDigit final : public virtual Implementer<int,const int&,const int&> {
public:
    CompTwoDigit() noexcept {}
    CompTwoDigit(const CompTwoDigit& own) noexcept {
        *this = own;
    }
    ~CompTwoDigit() noexcept {}
    virtual int compute(const int& a,const int& b) const override {
        return max(a,b);
    }
    virtual int reverseCompute(const int& a,const int& b) const override {
        return max(a,b,std::greater{});
    }
};
class CompThreeDigit final : public virtual Implementer<int,const int&,const int&,const int&> {
public:
    CompThreeDigit() noexcept {}
    CompThreeDigit(const CompThreeDigit& own) noexcept {
        *this = own;
    }
    ~CompThreeDigit() noexcept {}
    virtual int compute(const int& a,const int& b,const int& c) const override {
        return max({a,b,c});
    }
    virtual int reverseCompute(const int& a,const int& b,const int& c) const override {
        return max({a,b,c},std::greater{});
    }
};
int test_Implementer_Comps() {
    cout << "----------------------------------------- test_Implementer_Comps" << endl;
    CompTwoDigit comp2;
    Implementer<int,const int&,const int&>* interface = static_cast<Implementer<int,const int&,const int&>*>(&comp2);
    int ret = interface->compute(6,3);
    ptr_lambda_debug<const string&,const int&>("ret is ",ret);
    assert(ret == 6);
    ret = interface->reverseCompute(6,3);
    ptr_lambda_debug<const string&,const int&>("ret is ",ret);
    assert(ret == 3);
    CompThreeDigit comp3;
    // これは、インタフェースの意味なくなっちゃうのか orz
    Implementer<int,const int&,const int&,const int&>* if3 = static_cast<Implementer<int,const int&,const int&,const int&>*>(&comp3);
    ret = if3->compute(6,3,9);
    ptr_lambda_debug<const string&,const int&>("ret is ",ret);
    assert(ret == 9);
    ret = if3->reverseCompute(6,3,9);
    ptr_lambda_debug<const string&,const int&>("ret is ",ret);
    assert(ret == 3);
    return 0;
}
template<class... ArgTypes>
class Abstraction {
protected:
    Implementer<int,ArgTypes...>* implementer;
public:
    virtual ~Abstraction() {}
    virtual int format(ArgTypes...) const = 0;
};
class StringFormatterType2 final : public virtual Abstraction<const int&,const int&> {
    StringFormatterType2() noexcept {
        implementer = nullptr;
    }
public:
    StringFormatterType2(Implementer<int,const int&,const int&>* imple) noexcept {
        implementer = imple;
    }
    StringFormatterType2(const StringFormatterType2& own) noexcept {
        *this = own;
        implementer = own.implementer;
    }
    ~StringFormatterType2() noexcept {}
    virtual int format(const int& a,const int& b) const override {
        try {
            int ret = implementer->compute(a,b);
            printf("a is %d and b is %d\tmax is %d\n",a,b,ret);
            ret = implementer->reverseCompute(a,b);
            printf("a is %d and b is %d\tmin is %d\n",a,b,ret);
            return 0;
        } catch(exception& e) {
            cerr << e.what() << endl;
            return -1;
        }
    }
};
int test_StringFormatterType2() {
    cout << "---------------------------------------- test_StringFormatterType2" << endl;
    CompTwoDigit comp2;
    Implementer<int,const int&,const int&>* if2 = static_cast<Implementer<int,const int&,const int&>*>(&comp2);
    StringFormatterType2 type2(if2);
    return type2.format(8,4);
}
class StringFormatterType3 final : public virtual Abstraction<const int&,const int&,const int&> {
    StringFormatterType3() noexcept {
        implementer = nullptr;
    }
public:
    StringFormatterType3(Implementer<int,const int&,const int&,const int&>* imple) noexcept {
        implementer = imple;
    }
    StringFormatterType3(const StringFormatterType3& own) noexcept {
        *this = own;
        implementer = own.implementer;
    }
    ~StringFormatterType3() noexcept {}
    virtual int format(const int& a,const int& b,const int& c) const override {
        try {
            int ret = implementer->compute(a,b,c);
            printf("a is %d ,b is %d and c is %d\tmax is %d\n",a,b,c,ret);
            ret = implementer->reverseCompute(a,b,c);
            printf("a is %d ,b is %d and c is %d\tmin is %d\n",a,b,c,ret);
            return 0;
        } catch(exception& e) {
            cerr << e.what() << endl;
            return -1;
        }
    }
};
int test_StringFormatterType3() {
    cout << "---------------------------------------- test_StringFormatterType3" << endl;
    CompThreeDigit comp3;
    Implementer<int,const int&,const int&,const int&>* if3 = static_cast<Implementer<int,const int&,const int&,const int&>*>(&comp3);
    StringFormatterType3 type3(if3);
    return type3.format(8,16,4);
}

/**
  Facade
  - 抽象化はしない、意味ない。
  - ほとんど答えがでているが、テスト関数がFacade のやるべきことに近い。
  - chain of responsibility はそのパターンが複数あるので、それをFacade で表現してみる。
  - Bridge に関してもImplementer 単体での利用とAbstraction の派生クラスである文字フォーマットクラス
    の併用等、少しだけその使い方を変えてみたり。

*/
class Facade final {
public:
    int visitA(ConcreteVisitor& vis) noexcept {
        AcceptorA a;
        const ConcreteVisitor* pvis = &vis;
        a.accept(pvis);
        return 0;
    }
    // visitB は仲間外れ：）
    int visitAll(ConcreteVisitor& vis) {
        try {
            AcceptorA a;
            // これは冗長な使い方だけど、忘れないために、復習を兼ねてのもの。
            Acceptor<const ConcreteVisitor*>* interface = static_cast<Acceptor<const ConcreteVisitor*>*>(&a);
            const ConcreteVisitor* pvis = &vis;
            interface->accept(pvis);
            AcceptorB b;
            interface = static_cast<Acceptor<const ConcreteVisitor*>*>(&b);
            interface->accept(pvis);
            return 0;
        } catch(exception& e) {
            cerr << e.what() << endl;
            return -1;
        }
    }
    int procAD() noexcept {
        ProcD d;
        ProcA a(d);
        if(a.request() == 0) {
            d.request();
        }
        return 0;
    }
    int procBCD() noexcept {
        ProcD d;
        ProcC c(d);
        ProcB b(c);
        if(b.request() == 0) {
            if(c.request() == 0) {
                d.request();
            }
        }
        return 0;
    }
    // ごめんよ、飽きてきた：）次で終わりにする。
    int formatThreeData(const int& a,const int& b,const int& c) noexcept {
        CompThreeDigit comp3;
        Implementer<int,const int&,const int&,const int&>* if3 = static_cast<Implementer<int,const int&,const int&,const int&>*>(&comp3);
        StringFormatterType3 type3(if3);
        return type3.format(a,b,c);
    }
};
int test_Facade_VisitAll() {
    cout << "---------------------------------------------- test_Facade_VisitAll" << endl;
    Facade facade;
    ConcreteVisitor vis;
    ptr_lambda_debug<const string&,const int&>("visit result is ",facade.visitAll(vis));
    return 0;
}
int test_Facade_VisitA() {
    cout << "---------------------------------------------- test_Facade_VisitA" << endl;
    Facade facade;
    ConcreteVisitor vis;
    return facade.visitA(vis);
}
int test_Facade_ProcAD() {
    cout << "---------------------------------------------- test_Facade_ProcAD" << endl;
    Facade facade;
    return facade.procAD();
}
int test_Facade_ProcBCD() {
    cout << "---------------------------------------------- test_Facade_ProcBCD" << endl;
    Facade facade;
    return facade.procBCD();
}
int test_Facade_FormatThreeData() {
    cout << "---------------------------------------------- test_Facade_FormatThreeData" << endl;
    Facade facade;
    return facade.formatThreeData(3,6,9);
}

int main() {
    cout << "START GoF Facade ===============" << endl;
    ptr_lambda_debug<const string&,const int&>("Yeah Here we go.",0);
    if(1) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_visitor_step_1());
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_visitor_step_2());
    }
    if(2) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_chain_of_responsibility());
    }
    if(3) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_Implementer_Comps());
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_StringFormatterType2());
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_StringFormatterType3());
    }
    if(4) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_Facade_VisitAll());
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_Facade_VisitA());
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_Facade_ProcAD());
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_Facade_ProcBCD());
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_Facade_FormatThreeData());
    }
    cout << "=============== GoF Facade END" << endl;
    return 0;
}