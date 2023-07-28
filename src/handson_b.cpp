/**
    継承に関する疑問をはっきりとさせることが第一課題。
    インタフェース（Pure Virtual）、基底クラス、派生クラスにおける protected public の各メンバ関数の
    呼び出しを確認してみる。
*/
#include <iostream>

using namespace std;

template<class M,class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << message << '\t' << debug << endl;
};

class IFace {
protected:
    virtual int pro_a() const = 0;
public:
    virtual int pub_a() const = 0;
    virtual ~IFace() {};
};

class Foo final : public IFace {
private:
    int pri_a() {
        ptr_lambda_debug<const string&,const int&>("Foo ... pri_a.",0);
        return 0;
    }
    int pri_b() {
        ptr_lambda_debug<const string&,const int&>("Foo ... pri_b.",0);
        return 0;
    }
protected:
    virtual int pro_a() const override {
        ptr_lambda_debug<const string&,const int&>("Foo ... pro_a.",0);
//        pri_b();        // やっぱりこれもコンパイルエラー、理由は同じだ。
        return 0;
    }
public:
    Foo() {
        pri_a();        // 無論これはコンパイルエラーじゃないよ：）
    }
    Foo(const Foo& own) {
        (*this) = own;
    }
    ~Foo() {}
    virtual int pub_a() const override {
        ptr_lambda_debug<const string&,const int&>("Foo ... pub_a.",0);
//        pri_a();        // これはコンパイルエラーなんだよ：）
        pro_a();
        return 0;
    }
};

int main(void) {
    cout << "START handson_b ===============" << endl;
    if(1) {
        ptr_lambda_debug<const string&,const int&>("Yeah Happy Summer :) Let's play cording :)",999);
    }
    if(2) {
        Foo foo;
        ptr_lambda_debug<const string&,const int&>("Play and Result ... foo",foo.pub_a());
        IFace* iface = static_cast<IFace*>(&foo);
        ptr_lambda_debug<const string&,const int&>("Play and Result ... iface",iface->pub_a());
    }
    cout << "=============== handson_b END" << endl;
    return 0;
}

