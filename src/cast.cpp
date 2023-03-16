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
#define DEFAULT_CHESHIRE_CAT_FRICTION_FORCE 76
#define ALICE_TIRE "Alice Good Tire."
#define CHESHIRE_CAT_TIRE "Cheshire Cat Pad Tire."
#define ALICE_CAR_FACTORY_NAME "Alice Good Speed Factory."
#define CHESHIRE_CAT_FACTORY_NAME "Cheshire Cat Speed Factory."

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
    virtual ~Maker(){}
};
// なんとなく、分かってきたよ、ダウンキャスト。
// こんなん、ペーパーカンパニだろ。
// 本来 Maker が持ってるべきものはなにも持たず、派生クラスに作る。
class VirtualMaker {
public:
    VirtualMaker() {}
    virtual int getFrictionForce() {return -1;}; 
    virtual string getMaker() {return "";};
    virtual ~VirtualMaker() {}
};
class AbstractTire : virtual VirtualMaker {
protected:
    string maker="none";
    int frictionForce;
public:
    AbstractTire():frictionForce{-1} {}
    AbstractTire(const int& frict){
        frictionForce = frict;
    }
    int getFrictionForce() {
        return frictionForce;
    }
    string getMaker() {
        return maker;
    }
    virtual ~AbstractTire() {
        ptr_lambda_debug<string,int>("AbstractTire Destructor === ", frictionForce);
    } 
};
class AliceTire : public AbstractTire {
    AliceTire(const int& friction) {
        frictionForce = friction;
    }
public:
    AliceTire(){
        maker = ALICE_TIRE;
        frictionForce = DEFAULT_ALICE_TIRE_FRICTION_FORCE;
    }
};
class CheshireCatTire : public AbstractTire {
    CheshireCatTire(const int& friction) {
        frictionForce = friction;
    }
public:
    CheshireCatTire() {
        maker = CHESHIRE_CAT_TIRE;
        frictionForce = DEFAULT_CHESHIRE_CAT_FRICTION_FORCE;
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

class CheshireCatCarFactory : public AbstractCarFactory {
public:
    AbstractTire createTire() const override {
        CheshireCatTire tire;
        return tire;
    }
    AbstractBody createBody() const override {
        AbstractBody body(CHESHIRE_CAT_FACTORY_NAME);
        return body;
    }
    AbstractEngine createEngine() const override {
        AbstractEngine engine(CHESHIRE_CAT_FACTORY_NAME);
        return engine;
    }
    AbstractAero createAero() const override {
        AbstractAero aero(CHESHIRE_CAT_FACTORY_NAME);
        return aero;
    }
    Car madeInCarFactory() const override {
        Car car(createTire(),createBody(),createEngine(),createAero());
        return car;
    }
};

void test_array(const AbstractTire& flt) {
    cout << "---------------------------- test_array" << endl;
    AbstractTire* tires;
    tires = new AbstractTire[4];
    tires[0] = flt;
    cout << "tires[0] is " << tires[0].getFrictionForce() << endl;
    delete [] tires;
}

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

void test_Cheshire_Cat_Car_Factory() {
    cout << "------------------- test_Cheshire_Cat_Car_Factory" << endl;
    CheshireCatCarFactory factory;
    Car car = factory.madeInCarFactory();
   cout << car.toString() << endl;
}

void test_up_cast() {
    cout << "------------------- test_up_cast" << endl;
    AbstractAero aero("アップキャストエアロ");
    Maker& maker = static_cast<Maker&>(aero);
    ptr_lambda_debug<string,string>("maker is ",maker.getMaker());
    // 確かにできることは分かった。
    // しかし、アップキャストの用途はなんだ？
    // 派生クラスから基底クラスを辿れるのは普通だよね。
    // うん、まぁ今はそんなんがあるのか、ぐらいでいいや。
}

/**
 * 仮想基底クラスを介した関係がないと、dynamic_cast ができない。
 * VCode に叱られるので、そもそもやらない、できない。
 * なるほど、今回はじめてでてきたな、仮想基底クラス。
 * Maker をそれに見立てて改造してみる。
 * 
 * リファレンス先生は継承関係がない参照の場合は例外（std::bad_cast）が発生、
 * ポインタの場合はnullptrが代入されると言っている。
 * これも理解はできるが、用途が分からない。
 * 
 * サンプルのように空っぽの構造体なら上手く行くんだろうね。
 * 興味もないので、今はもういい。
*/
void test_down_cast() {
    cout << "------------------- test_down_cast" << endl;
    try {
        VirtualMaker paperCompany;
        AbstractTire& atire = dynamic_cast<AbstractTire&>(paperCompany);
        ptr_lambda_debug<string,int>("friction Force is ",atire.getFrictionForce());
    } catch(exception e) {
        cerr << e.what() << endl;
        cout << "これが出力されたってことは、ペーパーカンパニからのダウンキャストに失敗したということ。" << endl;
    }
}

/**
 * はい、悪い子ですね。
 * const 修飾と volatile 修飾を変更します。
*/
void test_const_cast() {
    cout << "------------------- test_const_cast" << endl;
    const int i = 256;
    int& j = const_cast<int&>(i);
    j = 25;
    cout << "i is " << i << endl;
    cout << "j is " << j << endl;
    int& k = (const_cast<int&>(i) = 0);
    cout << "i is " << i << endl;
    cout << "k is " << k << endl;
    int a = 10;
    int b = 11;
    const int* pa = &a;
    pa = &b;    // ポインタの向きは変えられるのね。
    // *pa = 33;   // コンパイルエラー、const int の値を変更するのと同じ。
    cout << "*pa is " << *pa << endl;
    auto foo = [pa]() {
        cout << "foo----- Yeah I'm Bad Boy :)" << endl;
        int* l = const_cast<int*>(pa);
        cout << "*l is " << *l << endl;
        *l = 2;
        cout << "*l is " << *l << endl;
        cout << "*pa is " << *pa << endl;
    };
    foo();
    cout << "*pa is " << *pa << endl;
}

/**
 * reinterpret_cast は整数型とその参照、及びポインタ間で型を変換します。
*/
void test_reinterpret_cast() {
    cout << "------------------- test_reinterpret_cast" << endl;
    intptr_t a = 8; // ポインタサイズの整数型
    cout << a << endl;
    // 整数値をポインタに変換
    void* p = reinterpret_cast<void*>(a);

    // ポインタを整数値に戻す
    intptr_t b = reinterpret_cast<intptr_t>(p);
    cout << b << endl;

    // なんでこんなことやるの？
    /**
     * このような変換は、void* もしくは整数型1つのみを仮引数にとる
     * C 言語の API とやりとりする時に必要となります。しかし、このような
     * 変換は積極的にやるべきではありません。
     * 
     * キャストに関しては static_cast 以外終始こんな調子だな。
    */

}

int main() {
    cout << "START キャスト ================ " << endl;
    // AbstractTire flt(16);
    // test_array(flt);
    test_Alice_Car_Factory();
    test_Cheshire_Cat_Car_Factory();
    //
    // ここから、本題のキャストです。
    //
    test_up_cast();
    test_down_cast();
    test_const_cast();
    test_reinterpret_cast();
    cout << "================ キャスト END " << endl;
    return 0;   
}