/**
 * キャスト ========================================================
キャストは型の変換に用いられます。以下の4つと C 言語スタイルのキャストが
用意されています。

- static_cast
- dynamic_cast
- const_cast
- reinterpret_cast

アップキャスト、ダウンキャストとクロスキャスト

クラス型のオブジェクトから、基底クラス部分を取り出すキャストをアップキャストと
呼びます。逆に、そこから派生クラス部分を取り出すキャストをダウンキャストと呼びます。
また、多重継承しているクラスのオブジェクトにおいて、ある基底クラス部分から別の基底
クラス部分へのキャストをクロスキャストと呼びます。
これらは、すべてクラスへのポインタ型または参照型を対象として行います。

アップキャストが可能なことは、コンパイル時に判定できるので、キャスト演算子を使わず
暗黙的に型変換できます。また、static_cast でも変換できます。
一方、ダウンキャストとクロスキャストはコンパイル時に判定できません。
キャスト元に指定される ポインタ ／ 参照 がどんな型のオブジェクトなのか、一般的に実行時
までわからないからです。このため、実行時に判定を行う dynamic_cast があります。

なお、ダウンキャストに成功すると予め分かっている場合、static_cast で実行時判定を
ぜずダウンキャストを行えます。

Abstract Factory

うん、沢山クラスがあった方が色々とキャストができるんじゃないかと。
そんな理由です。

あまり、作るパーツが増えると破綻する工場ね、いらなくね。
オーダーメイドに敵うものはない。
要は既成品工場だと。

AbstractAutoMobile
- AbstractTires createTires()
- AbstractBody createBody()
- AbstractEngine createEngine()
- AbstractAeroDynamics createAero()

*/
#include <iostream>
#include <list>
#include <algorithm>

#define DEFAULT_ALICE_TIRE_FRICTION_FORCE 64
#define ALICE_TIRE "Alice Good Tire."
#define ALICE_CAR_FACTORY_NAME "Alice Good Speed Factory."

using namespace std;

template<class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

class Maker {
protected:
    string maker="none";
public:
    string getMaker() {
        return maker;
    }
};
class AbstractTire : public Maker {
protected:
    int frictionForce;
public:
    AbstractTire():frictionForce{-1} {}
    AbstractTire(const int& frictF){
        frictionForce = frictF;
    }
    int getFrictionForce() {
        return frictionForce;
    }
    virtual ~AbstractTire() {
        ptr_lambda_debug<string,int>("AbstractTire Destructor === ", frictionForce);
    } 
};
class AliceTire : public AbstractTire {
    AliceTire(const int& frictionF) {
        frictionForce = frictionF;
    }
public:
    AliceTire(){
        maker = ALICE_TIRE;
        frictionForce = DEFAULT_ALICE_TIRE_FRICTION_FORCE;
    }
};
class AbstractBody : public Maker {
protected:
public:
    AbstractBody(const string& mname) {
        maker = mname;
    }
    string getMaker() {
        return maker;
    }
    virtual ~AbstractBody() {}
};
class AbstractEngine : public Maker {
protected:
public:
    AbstractEngine(const string& mname) {
        maker = mname;
    }
    string getMaker() {
        return maker;
    }
    virtual ~AbstractEngine() {}
};
class AbstractAero : public Maker {
protected:
public:
    AbstractAero(const string& mname){
        maker = mname;
    }
    string getMaker() {
        return maker;
    }
    virtual ~AbstractAero() {}
};
class Car {
    // 自動車の各パーツは外部で作る。
    // 自動車の各パーツはポインタとする。（初期化はコンストラクタで行うこと。デストラクタで解放忘れるな。
    AbstractTire* tires;
    AbstractBody* body;
    AbstractEngine* engine;
    AbstractAero* aero;
public:
    Car(const AbstractTire& t, const AbstractBody& b, const AbstractEngine& e, const AbstractAero& a) {
        tires = new AbstractTire[4];
        body = new AbstractBody("");
        engine =  new AbstractEngine("");
        aero = new AbstractAero("");
        tires[0] = t;
        tires[1] = t;
        tires[2] = t;
        tires[3] = t;
        *body = b;
        *engine = e;
        *aero = a;
    }
    virtual ~Car() {
        delete [] tires;
        delete body;
        delete engine;
        delete aero;
        ptr_lambda_debug<string,int>("Car Destructor.",0);
    }

    string toString() {
        char fforce[50];
        string s;
        s = tires[0].getMaker();
        s += '\n';
        sprintf(fforce, "friction force is %d\n", tires[0].getFrictionForce());
        s += fforce;
        s += body->getMaker();
        s += '\n';
        s += engine->getMaker();
        s += '\n';
        s += aero->getMaker();
        s += '\n';
        return s;
    }
    
};
class AbstractCarFactory {
public:
    virtual AbstractTire createTire() const = 0;
    virtual AbstractBody createBody() const = 0;
    virtual AbstractEngine createEngine() const = 0;
    virtual AbstractAero createAero() const = 0;
    virtual Car madeInCarFactory() const = 0;
};

class AliceCarFactory : public AbstractCarFactory {
public:
    AbstractTire createTire() const override {
        AliceTire tire;
        return tire;
    }
    AbstractBody createBody() const override {
        AbstractBody body(ALICE_CAR_FACTORY_NAME);
        return body;
    }
    AbstractEngine createEngine() const override {
        AbstractEngine engine(ALICE_CAR_FACTORY_NAME);
        return engine;
    }
    AbstractAero createAero() const override {
        AbstractAero aero(ALICE_CAR_FACTORY_NAME);
        return aero;
    }
    Car madeInCarFactory() const override {
        Car car(createTire(),createBody(),createEngine(),createAero());
        return car;
    }
};

void test_Alice_Car_Factory() {
    cout << "------------------- test_Alice_Car_Factory" << endl;
    AliceCarFactory factory;
    factory.createAero();
    ptr_lambda_debug<string,int>("made Aero",0);
    factory.createBody();
    ptr_lambda_debug<string,int>("made Body",0);
    factory.createEngine();
    ptr_lambda_debug<string,int>("made Engine",0);
    factory.createTire();
    ptr_lambda_debug<string,int>("made Tire",0);
   Car car = factory.madeInCarFactory();
   cout << car.toString() << endl;
}

void test_array(const AbstractTire& flt) {
    cout << "---------------------------- test_array" << endl;
    AbstractTire* tires;
    tires = new AbstractTire[4];
    tires[0] = flt;
    cout << "tires[0] is " << tires[0].getFrictionForce() << endl;
    delete [] tires;
}

int main() {
    cout << "START キャスト ================ " << endl;
    // AbstractTire flt(16);
    // test_array(flt);
    test_Alice_Car_Factory();
    cout << "================ キャスト END " << endl;
    return 0;   
}