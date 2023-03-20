/**
 * コンテナとアルゴリズム
 * 
 * 既に幾つか、使ったものがあるね。前回の stack や array その前だと vector 
 * がコンテナ、そのオブジェクトにあるメンバ関数の sort() などがアルゴリズム
 * という、らしい。
 * ここで少し注意しなければいけないのが、そのコンテナのヘッダとアルゴリズム
 * のヘッダは違うことがよくあるということ。
 * 
 * 何作ろう、当然のように GoF を的に掛けるのだが。
 * 
 * Decorator
 * 
 * なんの繋がりもイメージもないが。デコレイタ、コンテナ、たぶん響きが似てる
 * だけだな。ではもう一つ。
 * 
 * Adapter
 * 
 * これは、分野が違うがコンテナ関連であるぞ、もちろんGoFのものとは違う
 * だろうが、なんかあるかもしれないし。
 * 
 * OK、GoFの的は、DecoratorとAdapterに決定。
 * 
 * 何を Deco し何を Adapt したいのかな。
 * Decorator オレの勝手な解釈では拡張とある程度担保した汎用インタフェース
 * どちらかと言えば、拡張性に重きをおいている。
 * 
 * Adapter もしかり、お互いの継承構造を崩さずに片方の機能をもう一方にも提供
 * するもの。うん、拡張なんだよな、イメージはね、どちらも。
 * 
 * デコトラでも作ろうか、拡張性を持った。
 * アダプタはデコトラとは全く関係ないもの、でも、現実世界にもあってイメージ
 * しやすいものとして、ここは自転車だな。あるもんな、昔あったよね。
 * 
 * 補足しないと分からなくなるね。おそらく、お前はこの時点で、最後まで
 * イメージできてたよな :) 端折るのよくないよ :)
 * 自転車はDecoとは異なる継承構造のものにすることがまず最初にあり、
 * Decoの機能をAdapterを介して利用するのが自転車、
 * チャリだね。ここまで説明しないと、未来から来たオレでは少し？ってなる。
 * 
 * ここまでで、だいぶ読み飛ばしてきた文字列から学習したことで
 * STARTをきってみたいぞ。
 * 
 * はい、残念。まだ、GCCでは未実装でした。
 * https://qiita.com/Yossy_Hal/items/e204c3fb8f86722ee9db
 * ぐぬぬぬ。
 * 
 * Clang libc++ と MSVC STL は実装済なので、このどちらかを使う
 * 
 * いいもん、いらないもん、GCCがいいんだもん。
 * 
*/
#include <iostream>
#include <optional>
#include <array>
#include <cassert>

// #include <format>

using namespace std;

// 見せてやるよオレやり方を
template <class C, class I, class M>
void (*ptr_lambda_message)(C,I,M) = [](auto c, auto i, auto message) -> void {
    for(int n = 0;n < i;n++) {
        cout << c;    
    }
    cout << ' ';
    cout << message << endl;
};
template<class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message,auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * こんなことをイメージするよ。
 * 
 * 自転車のフレームの装飾に Decorator を使いたい。Alice には Alice の装飾を
 * あるいは複合的な装飾もあり。それを、Adapter で実現してみたい。
 * こんなストーリー。
 * 
 * Decorator から切り出す関数は何でもいいが、次のものにした。
 * top()
 * 
 * これをAdapterにのせる。チャリはそのAdapterを経由して装飾を行う。
 * はい、仕様は決まりました :)
 * 
*/

class DecoAdapter {
public:
    virtual void decoration() const = 0;
    virtual ~DecoAdapter() {}
};

class TDecorator {
public:
    virtual void front() const = 0;
    virtual void side() const = 0;
    virtual void rear() const = 0;
    virtual void top() const = 0;
    virtual void info() const = 0;
    virtual ~TDecorator(){}
};
class Truck final : public virtual TDecorator {
    string type;
    int decoLevel;
public:
    Truck(): type{"Default"}, decoLevel{-1} {}    // const でもこれはいける。なんとなく確認したくなったけど、これはOKなんだね。
    Truck(const string& t, const int& lev) {
        type = t;
        decoLevel = lev;
    }
    virtual void front() const override {
        cout << "普通のフロント" << endl;
    }
    virtual void side() const override {
        cout << "普通のサイド" << endl;
    }
    virtual void rear() const override {
        cout << "普通のリア" << endl;
    }
    virtual void top() const override {
        cout << "普通のトップ" << endl;
    }
    virtual void info() const override {
        cout << "type is \t" << type << endl;
        cout << "level is \t" << decoLevel << endl;
    }
    ~Truck(){}
};
void test_Truck() {
    ptr_lambda_message<const char&,const int&,const string&>('-',10,"test_Truck");
    Truck t;
    t.info();
    t.front();
    t.side();
    t.rear();
    t.top();
}
class AliceTruck final : public virtual TDecorator {
    Truck base;
public:
//    [[deprecated("please use AliceTruck(const Truck& truck)")]]
    AliceTruck(){}  // これをプロブラマが直接呼ぶことは非推奨。コンパイラのために開けたもの。
    AliceTruck(const Truck& truck) noexcept {
        base = truck;
    }
    virtual void front() const override {
        base.front();
        cout << "に見えて、アリスのデスマスク" << endl;

    }
    virtual void side() const override {
        cout << "ALICE LOGO のサイド" << endl;
    }
    virtual void rear() const override {
        cout << "大きく振りかぶって魅せるアリス" << endl;
    }
    virtual void top() const override {
        base.top();
        cout << "に見えて、アリスの全身姿のフォト" << endl;
    }
    virtual void info() const override {
        base.info();
    }
    ~AliceTruck(){}
};
void test_Alice_Truck() {
    ptr_lambda_message<const char&,const int&,const string&>('-',10,"test_Alice_Truck");
    Truck base("Alice",64);
    AliceTruck alice(base);
    alice.info();
    alice.front();
    alice.side();
    alice.rear();
    alice.top();
}
template<class T>
concept Truckable = requires (T& x) {
  x.front();x.side();x.rear();x.top();x.info(); // 型Tに要求する操作をセミコロン区切りで列挙する。
};
template<Truckable T>
class CheshireTruck final : public TDecorator {
    T base;
    CheshireTruck(){}
public:
    CheshireTruck(T& truck) {
        base = truck;
    }
    ~CheshireTruck(){}
    virtual void front() const override {
        base.front();
        cout << "と、チェシャ猫の不敵な笑み" << endl;
    }
    virtual void side() const override {
        base.side();
        cout << "と、肉球" << endl;
    }
    virtual void rear() const override {
        base.rear();
        cout << "と、そのボールを狙うチェシャ猫" << endl;
    }
    virtual void top() const override {
        base.top();
        cout << "と、大きな猫の影" << endl;
    }
    virtual void info() const override {
        base.info();
    }
};
void test_Cheshire_Truck_Base_Normal() {
    ptr_lambda_message<const char&,const int&,const string&>('-',10,"test_Cheshire_Truck_Base_Normal");
    Truck base("Cheshire",32);
    CheshireTruck cat(base);
    cat.info();
    cat.front();
    cat.side();
    cat.rear();
    cat.top();
}
void test_Cheshire_Truck_Base_Alice() {
    ptr_lambda_message<const char&,const int&,const string&>('-',10,"test_Cheshire_Truck_Base_Alice");
    Truck base("Cheshire",256);
    AliceTruck alice(base);
    CheshireTruck cat(alice);
    cat.info();
    cat.front();
    cat.side();
    cat.rear();
    cat.top();
}
class JabberTruck final : public TDecorator {
    const TDecorator* base;     // const には const で、この方が安全だよ。

    JabberTruck():base{nullptr} {}
public:
    JabberTruck(const TDecorator& truck) {
        base = &truck;
    }
    ~JabberTruck() {
    }
    virtual void front() const override {
        base->front();
        cout << "と、JabberWockyの大きな頭の影" << endl;
    }
    virtual void side() const override {
        base->side();
        cout << "と、大きな爪" << endl;
    }
    virtual void rear() const override {
        base->rear();
        cout << "と、襲いかかるJabberwocky" << endl;
    }
    virtual void top() const override {
        base->top();
        cout << "と、Jabberwockyの巨躯の影" << endl;
    }
    virtual void info() const override {
        base->info();
    }
};
void test_Jabber_Truck() {
    ptr_lambda_message<const char&,const int&,const string&>('-',10,"test_Jabber_Truck");
    Truck base("Jabberwocky",512);
    AliceTruck alice(base);
    JabberTruck jabber(alice);
    jabber.info();
    jabber.front();
    jabber.side();
    jabber.rear();
    jabber.top();
}

/**
 * TDecorator の top() 関数の機能を提供すること。
 * さっき決めました。
*/
class JabberDecoAdapter : public DecoAdapter {
public:
    void decoration() const override {
        Truck base("Jabberwocky",512);
        AliceTruck alice(base);
        JabberTruck jabber(alice);
        // オイ、機能を提供してくれ、ください。
        ptr_lambda_debug<const string&,const int&>("offered by Jabber top.",0);
        jabber.top();
    }
};


// handle
// flame
// transmission (optional)
// Wheel

class AbstractHandle {          // 注意メンバ関数を pure virtual にするとコンパイルエラーになるよ。
protected:
    int weight;
public:
    AbstractHandle():weight{-1} {}
    virtual ~AbstractHandle() {}
    virtual int getWeight() noexcept {  // これなら、流石に noexcept つけられるよね。
        return weight;
    }
    // virtual int getWeight() const = 0;
};
class AbstractFlame {
protected:
    int weight;
public:
    AbstractFlame():weight{-1} {}
    virtual ~AbstractFlame() {}
    virtual int getWeight() noexcept {
        return weight;
    }
};
class AbstractTransmission {
protected:
    int weight;
public:
    AbstractTransmission():weight{-1} {}
    virtual ~AbstractTransmission() {}
    virtual int getWeight() noexcept {
        return weight;
    }
};
class AbstractWheel {
protected:
    int weight;
public:
    AbstractWheel():weight{-1} {}
    virtual ~AbstractWheel() {}
    virtual int getWeight() {
        return weight;
    }
};

class AbstractBicycle {     // 状態は別クラスで管理したい。
public:
    AbstractBicycle() {}
    virtual ~AbstractBicycle() {}
    virtual void ride() const = 0;
    virtual void accelerate() const = 0;
    virtual void turnLeft() const = 0;
    virtual void turnRight() const = 0;
    virtual void brake() const = 0;
    virtual void back() const = 0;

    // virtual void ride() const {
    //     cout << "自転車をこぐよ。" << endl;
    // }
    // virtual void accelerate() const {
    //     cout << "自転車を加速させるよ。" << endl;
    // }
    // virtual void turnLeft() const {
    //     cout << "自転車を左に曲げるよ。" << endl;
    // }
    // virtual void turnRight() const {
    //     cout << "自転車を右に曲げるよ。" << endl;
    // }
    // virtual void brake() const {
    //     cout << "ブレーキをかけるよ。" << endl;
    // }
};
template<class T>
class AbstractBicycleFactory {
public:
    virtual AbstractHandle createHandle() const = 0;
    virtual AbstractFlame createFlame() const = 0;
    virtual optional<AbstractTransmission> createTransmission() const = 0;
    virtual AbstractWheel createWheel() const = 0;
    virtual T madeInFactory() const = 0;
    virtual ~AbstractBicycleFactory() {}
};

class AliceHandle final : virtual public AbstractHandle {
public:
    AliceHandle(const int& w) {
        weight = w;
    }
    AliceHandle(const AliceHandle& cpy) {   // 今回は、使わないと思うけど念のため、コピーコンストラクタを用意した。
        weight = cpy.getWeight();
    }
    ~AliceHandle(){}
    int getWeight() const {     // const cv 修飾、この場合、const AliceHandle ah; ah.getWeight(); を許可している。 
        return weight;
    }
};
class AliceFlame final : public AbstractFlame {
public:
    AliceFlame(const int& w) {
        weight = w;
    }
    ~AliceFlame() {}
};
class AliceWheel final : public AbstractWheel {
public:
    AliceWheel(const int& w) {
        weight = w;
    }
    ~AliceWheel(){}
};
/**
 * アリススポーツの自転車クラス。
 * 
 * 自転車の具体的な操作ができる。
 * - こぐ
 * - 加速させる
 * - 左に曲がる
 * - 右に曲がる
 * - ブレーキ
 * - バック
 * - （Uターン    // これは何だ？）
 * 
 * 状態の変化は別で考慮する。
*/
class AliceSportBicycle final : public AbstractBicycle, JabberDecoAdapter {
    AbstractHandle* handle;
    AbstractFlame* flame;
    std::array<AbstractWheel,2> wheels;
    AliceSportBicycle():handle{nullptr},flame{nullptr},wheels{} {}
public:
    AliceSportBicycle(AbstractHandle& h, AbstractFlame& f, AbstractWheel& fw, AbstractWheel& rw) {
        handle = &h;
        flame = &f;
        wheels[0] = fw;
        wheels[1] = rw;
    }
    void ride() const override {
        cout << "アリススポーツをこぐよ。" << endl;
    }
    void accelerate() const override {
        cout << "アリススポーツを加速させるよ。" << endl;
    }
    void turnLeft() const override {
        cout << "アリススポーツを左に曲げるよ。" << endl;
    }
    void turnRight() const override {
        cout << "アリススポーツを右に曲げるよ。" << endl;
    }
    void brake() const override {
        cout << "アリススポーツ,ブレーキをかけるよ。" << endl;
    }
    void back() const override {
        cout << "アリススポーツ,バックするよ。" << endl;
    }
    /**
     * 外観の見た目が追加された。
     * JabberDecoAdapter の機能提供による独自拡張になったメンバ関数です。
    */
    void appearance() {
        decoration();
    }
};
class AliceBicycleFactory final : public AbstractBicycleFactory<AliceSportBicycle> {
public:
    ~AliceBicycleFactory(){}
    AbstractHandle createHandle() const override {
        AliceHandle handle(300);
        return handle;
    }
    AbstractFlame createFlame() const override {
        AliceFlame flame(1500);
        return flame;
    }
    optional<AbstractTransmission> createTransmission() const override {
        optional<AbstractTransmission> optionTMission;
        return optionTMission;
    }
    AbstractWheel createWheel() const override {
        AliceWheel wheel(750);
        return wheel;
    }
    AliceSportBicycle madeInFactory() const override {
        AbstractHandle h = createHandle();
        AbstractFlame f = createFlame();
        AbstractWheel fw = createWheel();
        AbstractWheel rw = createWheel();
        AliceSportBicycle aliceSport = AliceSportBicycle(h,f,fw,rw);
        return aliceSport;
    }
};
// Abstract Factory はクラスが増えるな、止めればよかった、一回やってるのに、性懲りもなくね :)
void test_Alice_Bicycle_Factory() {
    ptr_lambda_message<const char&,const int&,const string&>('-',10,"test_Alice_Bicycle_Factory");
    AliceBicycleFactory factory;
    AbstractHandle h = factory.createHandle();
    AbstractFlame f = factory.createFlame();
    optional<AbstractTransmission> optMission = factory.createTransmission();
    AbstractWheel w = factory.createWheel();

    ptr_lambda_debug<const string&,const int&>("optMission.has_value() is ", optMission.has_value());
    ptr_lambda_debug<const string&,const int&>("handle weight is ", h.getWeight());
    ptr_lambda_debug<const string&,const int&>("flame weight is ", f.getWeight());
    ptr_lambda_debug<const string&,const int&>("wheel weight is ", w.getWeight());

    // 次のアサーションに引っかかると問題がある。
    assert(h.getWeight() != -1);
    assert(f.getWeight() != -1);
    assert(w.getWeight() != -1);

    AliceSportBicycle aliceSport = factory.madeInFactory();
    aliceSport.ride();
    aliceSport.accelerate();
    aliceSport.turnLeft();
    aliceSport.turnRight();
    aliceSport.brake();
    aliceSport.back();
    // adapter による機能拡張。
    aliceSport.appearance();
}

int main() {
    // cout << format(":=^10","START") << endl;
    // ザクとは違うのだよ、ザクとはっ。幸先のいい走り出しじゃないか :)
    ptr_lambda_message<const char&,const int&,const string&>('=',15,"コンテナとアルゴリズム START");
    test_Truck();
    test_Alice_Truck();
    test_Cheshire_Truck_Base_Normal();
    test_Cheshire_Truck_Base_Alice();
    test_Jabber_Truck();
    test_Alice_Bicycle_Factory();
    ptr_lambda_message<const char&,const int&,const string&>('=',15,"コンテナとアルゴリズム END");
    return 0;
}