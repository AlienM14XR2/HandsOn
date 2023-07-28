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
    virtual int pro_b() const = 0;
public:
    virtual int pub_a() const = 0;
    virtual int pub_b() const = 0;
    virtual ~IFace() {};
};
class Foo : public IFace {
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
    virtual int pro_b() const override {
        ptr_lambda_debug<const string&,const int&>("Foo ... pro_b.",0);
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
    virtual int pub_b() const override {
        ptr_lambda_debug<const string&,const int&>("Foo ... pub_b.",0);
        pro_b();
        return 0;
    }
};
class Bar final : public Foo {
public:
    Bar() {}
    Bar(const Bar& own) {
        (*this) = own;
    }
    ~Bar() {}
    virtual int pub_b() const override {
        ptr_lambda_debug<const string&,const int&>("Bar ... pub_b.",0);
        pro_b();
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
        ptr_lambda_debug<const string&,const int&>("Play and Result ... foo.pub_a()",foo.pub_a());
        IFace* iface = static_cast<IFace*>(&foo);
        ptr_lambda_debug<const string&,const int&>("Play and Result ... iface->pub_a()",iface->pub_a());
        ptr_lambda_debug<const string&,const int&>("Play and Result ... iface->pub_b()",iface->pub_b());
        Bar bar;
        ptr_lambda_debug<const string&,const int&>("Play and Result ... bar.pub_a()",bar.pub_a());  // 予想通りだよね：）
        iface = static_cast<IFace*>(&bar);
        ptr_lambda_debug<const string&,const int&>("Play and Result ... iface->pub_a()",iface->pub_a());
        ptr_lambda_debug<const string&,const int&>("Play and Result ... iface->pub_b()",iface->pub_b());
        // うん、気持ちがいいね、よくわかった。
        // 基底クラスは置いた方が何かといいのかもしれないな：）
        // インタフェース、基底クラス、派生クラス、これが C++ における OOP の基本的なかたちだな、やはり。
        // 蛇足だけど Java でも同じだと思う。

    }
    cout << "=============== handson_b END" << endl;
    return 0;
}

