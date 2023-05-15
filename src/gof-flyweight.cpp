/**
  GoF Flyweight パターン

  インスタンスを共有することでリソースを無駄なく使うことに焦点を当てたパターンです。 
  うん、シングルトンとは違うのだね、シングルトンはOOP ではアンチパターンとしての文脈で語られることがあるが。

  UML クラス図を言葉で説明してみる。
  Flyweight クラス（軽量）がある。中身、メンバは何でも良い。
   - このクラスはFlyweightFactory クラスに集約されている。

  FlyweightFactory クラス。Flyweight クラスを集約している。
   - pool: Flyweight[] 
     メンバ変数にFlyweight を格納するコンテナを持つ。
   - getFlyweight: Flyweight
     メンバ関数にFlyweight を生成する関数を持つ。

  OK 理解した。

*/
#include <iostream>
#include <vector>
#include "stdio.h"

using namespace std;

template<class M,class D>
void (*ptr_lambda_debug)(M,D) = [](auto message,auto debug) -> void {
    cout << message << '\t' << debug << endl;
};

class Flyweight final {
    string name;
    Flyweight():name{nullptr} {}
public:
    Flyweight(const string& n) {
        name = n;
    }
    Flyweight(const Flyweight& own) {
        *this = own;
        this->name = own.name;
    }
    ~Flyweight() {
        ptr_lambda_debug<const string&,const int&>("Flyweight Destructor.",0);
    }
    string& getName() noexcept {
        return name;
    }
};
class FlyweightFactory final {
    vector<Flyweight*> pool;
public:
    FlyweightFactory() {}
    FlyweightFactory(const FlyweightFactory& own) {
        *this = own;
        this->pool = own.pool;
    }
    ~FlyweightFactory() {
        for(Flyweight* f: pool) {
            delete f;
        }
    }
    Flyweight* getFlyweight() {
        if(pool.empty()) {
            Flyweight* f = new Flyweight("jack");
            pool.push_back(f);
        }
        return pool[0];
    }
};
int test_Basic_Flyweight() {
    cout << "---------------------------------- test_Basic_Flyweight" << endl;
    FlyweightFactory factory;
    Flyweight* pf = factory.getFlyweight();
    ptr_lambda_debug<const string&,const string&>("pf->getName() is ",pf->getName());
    printf("Flyweight pointer is %p\n",pf);
    Flyweight* pf_2 = factory.getFlyweight();
    ptr_lambda_debug<const string&,const string&>("pf_2->getName() is ",pf_2->getName());
    printf("Flyweight pointer is %p\n",pf_2);
    // うん、これはいい、シングルトンは止めて、Flyweight を使うべきなのか：）
    return 0;
}
int main(void) {
    cout << "START GoF Flyweight ===============" << endl;
    ptr_lambda_debug<const string&,const int&>("Here we go.",0);
    if(1) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_Basic_Flyweight());
    }
    cout << "=============== GoF Flyweight END" << endl;
    return 0;
}
