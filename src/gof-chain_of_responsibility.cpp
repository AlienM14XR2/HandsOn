/**
  GoF Chain of Responsibility パターン

  Chain of Responsibility うん、これも知らない、というか以降やるものはほぼ知らないものがほどんどか：）
  直訳すれば「責任の鎖」。うん、面白そうだ。

  Handler インタフェース（基底抽象クラス）が1つ。
  Handler の派生クラスConcreteHanler が複数個。
  request を行うClientが1つ。

  Handler インタフェースは next として、次の責任者、Handler オブジェクトをメンバ変数に持つ。
  Handler インタフェースはメンバ関数として request を持つ。

  オレの解釈、動的Template Method とFactory Method が合わさった感じ。
  使い方は沢山あるように思える。ではいつものように基本から。
*/
#include <iostream>
#include <cassert>

using namespace std;

template<class M,class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << message << '\t' << debug << endl;
};

class Handler {
protected:
    Handler* next;
public:
    virtual ~Handler() {}
    virtual int request() const = 0;
};
class ConcreteHandlerA final : public virtual Handler {
    // この初期化定義ができるのは自身のメンバ変数のみだと知る。
    // ConcreteHandlerA():foo{nullptr}bar{-1} {}
public:
    ConcreteHandlerA() {
        this->next = nullptr;
    }
    ConcreteHandlerA(Handler& handler) {
        this->next = &handler;
    }
    ConcreteHandlerA(const ConcreteHandlerA& own) {
        *this = own;
        this->next = own.next;
    }
    ~ConcreteHandlerA() {}
    virtual int request() const override {
        cout << "called request of ConcreteHandlerA ... " << endl;
        if( this->next != nullptr ) {
            return 0;
        } else {
            return -1;
        }
    }
};
class ConcreteHandlerB final : public virtual Handler {
public:
    ConcreteHandlerB() {
        this->next = nullptr;
    }
    ConcreteHandlerB(Handler& handler) {
        this->next = &handler;
    }
    ConcreteHandlerB(const ConcreteHandlerB& own) {
        *this = own;
        this->next = own.next;
    }
    ~ConcreteHandlerB() {}
    virtual int request() const override {
        cout << "called request of ConcreteHandlerB ... " << endl;
        if( this->next != nullptr ) {
            return 0;
        } else {
            return -1;
        }
    }
};
class ConcreteHandlerC final : public virtual Handler {
public:
    ConcreteHandlerC(){
        this->next = nullptr;
    }
    ConcreteHandlerC(Handler& handler) {
        this->next = &handler;
    }
    ConcreteHandlerC(const ConcreteHandlerC& own) {
        *this = own;
        this->next = own.next;
    }
    ~ConcreteHandlerC() {}
    virtual int request() const override {
        cout << "called request of ConcreteHandlerC ... " << endl;
        if( this->next != nullptr ) {
            return 0;
        } else {
            return -1;
        }
    }
};
int test_basic_chain_of_responsibility() {
    cout << "------------------------------------ test_basic_chain_of_responsibility" << endl;
    ConcreteHandlerC handlerC;
    ConcreteHandlerB handlerB(handlerC);
    ConcreteHandlerA handlerA(handlerB);
    Handler* handler = static_cast<Handler*>(&handlerA);
    int ret  = handler->request();
    ptr_lambda_debug<const string&,const int&>("ret is ",ret);
    handler = static_cast<Handler*>(&handlerB);
    ret = handler->request();
    ptr_lambda_debug<const string&,const int&>("ret is ",ret);
    handler = static_cast<Handler*>(&handlerC);
    ret = handler->request();
    ptr_lambda_debug<const string&,const int&>("ret is ",ret);
    // これも大切な気づき、勉強になった。インタフェース（抽象基底クラス）で操作する場合、そのクラスはポインタにすること。
    // でないと、意図した動きにならない。色々と試してみないと分からないね。
    // インタフェースはポインタ、これが新たなルール。

    return 0;
}
int main() {
    cout << "START GoF Chain of Responsibility ===============" << endl;
    if(1) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_basic_chain_of_responsibility());
    }
    cout << "=============== GoF Chain of Responsibility END" << endl;
    return 0;
}