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
 * はい、ここから本題です。
 * Decorator の派生クラスをもう一つ量産して
 * Abstract Factory でつくられるチャリをもっと増やして（最低でもあと２つ）
 * それぞれ、数値をメンバ変数内でもってるので、その合計値で並べ替えや
 * 単純なソートでもいいので実践してみたい。
 * 
*/
#include <iostream>
#include <optional>
#include <array>
#include <cassert>
#include <vector>
#include <algorithm>

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
    virtual int getDecoLevel() const = 0;
    virtual ~TDecorator(){}
};
class Truck final : public virtual TDecorator {
    string type;
    int decoLevel;
public:
    Truck(): type{"Default"}, decoLevel{-1} {}    // メンバ変数が const でもこれはいける。なんとなく確認したくなったけど、これはOKなんだね。
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
    virtual int getDecoLevel() const override {
        return decoLevel;
    }
    virtual pair<string,int> type_decolevel() const {
        return {type,decoLevel};
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
    Truck base;     // だんだんと、これがメモリの無駄使いに見えてくるよ。そんなことない、これはこれでいいのだよ、ポインタバカは黙ってなさい。
public:
//    [[deprecated("please use AliceTruck(const Truck& truck)")]]
    AliceTruck(){}  // これをプロブラマが直接呼ぶことは非推奨。コンパイラのために開けたもの。気になるならコメントアウトしてみなさい。
    AliceTruck(const Truck& truck) noexcept {
        base = truck;
    }
    void front() const override {
        base.front();
        cout << "に見えて、アリスのデスマスク" << endl;

    }
    void side() const override {
        cout << "ALICE LOGO のサイド" << endl;
    }
    void rear() const override {
        cout << "大きく振りかぶって魅せるアリス" << endl;
    }
    void top() const override {
        base.top();
        cout << "に見えて、アリスの全身姿のフォト" << endl;
    }
    void info() const override {
        base.info();
    }
    int getDecoLevel() const override {
        return base.getDecoLevel();
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
  x.front();x.side();x.rear();x.top();x.info();x.getDecoLevel(); // 型Tに要求する操作をセミコロン区切りで列挙する。
};
template<Truckable T>
class CheshireTruck final : public TDecorator {
    T base;
    CheshireTruck(){}
public:
    CheshireTruck(T& truck) noexcept {
        base = truck;
    }
    ~CheshireTruck(){}
    void front() const override {
        base.front();
        cout << "と、チェシャ猫の不敵な笑み" << endl;
    }
    void side() const override {
        base.side();
        cout << "と、肉球" << endl;
    }
    void rear() const override {
        base.rear();
        cout << "と、そのボールを狙うチェシャ猫" << endl;
    }
    void top() const override {
        base.top();
        cout << "と、大きな猫の影" << endl;
    }
    void info() const override {
        base.info();
    }
    int getDecoLevel() const override {
        return base.getDecoLevel();
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
    JabberTruck(const TDecorator& truck) noexcept {
        base = &truck;
    }
    ~JabberTruck() {
    }
    void front() const override {
        base->front();
        cout << "と、JabberWockyの大きな頭の影" << endl;
    }
    void side() const override {
        base->side();
        cout << "と、大きな爪" << endl;
    }
    void rear() const override {
        base->rear();
        cout << "と、襲いかかるJabberwocky" << endl;
    }
    void top() const override {
        base->top();
        cout << "と、Jabberwockyの巨躯の影" << endl;
    }
    void info() const override {
        base->info();
    }
    int getDecoLevel() const override {
        return base->getDecoLevel();
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

class HumptyTruck final : public TDecorator {
    const TDecorator* base;
    HumptyTruck():base{nullptr} {}
public:
    HumptyTruck(const TDecorator& truck) noexcept {
        base = &truck;
    }
    ~HumptyTruck() {}
    virtual void front() const override {
        base->front();
        cout << "と、天使の羽をつけた巨大なEgg" << endl;
    }
    virtual void side() const override {
        base->side();
        cout << "と、白い羽が舞う" << endl;
    }
    virtual void rear() const override {
        base->rear();
        cout << "と、天使の羽をつけた巨大なEggの亀裂から光り" << endl;
    }
    virtual void top() const override {
        base->top();
        cout << "と、巨大なEggの大きな影" << endl;
    }
    virtual void info() const override {
        base->info();
    }
    virtual int getDecoLevel() const override {
        return base->getDecoLevel();
    }

};
void test_Humpty_Truck() {
    ptr_lambda_message<const char&,const int&,const string&>('-',10,"test_Humpty_Truck");
    Truck base("Humptydumpty",630);
    AliceTruck alice(base);
    HumptyTruck humpt(alice);
    humpt.info();
    humpt.front();
    humpt.side();
    humpt.rear();
    humpt.top();
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

// TODO これでダウンキャストの確認をやってみようよ。 予想だけど、多分できないかと。コンストラクタがデフォルトしかないし。
class AbstractBicycle {     // 状態は別クラスで管理したい。いや、やらないよそんなこと :)
public:
    AbstractBicycle() {}
    virtual ~AbstractBicycle() {}
    virtual void ride() const = 0;
    virtual void accelerate() const = 0;
    virtual void turnLeft() const = 0;
    virtual void turnRight() const = 0;
    virtual void brake() const = 0;
    virtual void back() const = 0;
    virtual int totalWeight() const = 0;
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

// なんでまた Abstract Factory にしたかな、もう懲りたよ。
//
// Cheshire Bicycle Parts
//

class CheshireHandle final : public AbstractHandle {
public:
    CheshireHandle(const int& w) {
        weight = w;
    }
    ~CheshireHandle() {}
};
class CheshireFlame final : public AbstractFlame {
public:
    CheshireFlame(const int& w) {
        weight = w;
    }
    ~CheshireFlame() {}
};
class CheshireWheel final : public AbstractWheel {
public:
    CheshireWheel(const int& w) {
        weight = w;
    }
    ~CheshireWheel() {}
};
class CheshireBicycle final : public AbstractBicycle {
    // 確認のため、コンテナ以外のメンバ変数は const をつけた。そして、確認もままならないうちに取ったと。
    CheshireHandle* handle;
    CheshireFlame* flame;
    std::array<CheshireWheel*,1> wheels;     // 敢えて一輪車にしてみた。軽量化に勝るチューニングはないと誰かが言っていたから。
    CheshireBicycle():handle{nullptr},flame{nullptr},wheels{} {}
public:
    CheshireBicycle(AbstractHandle& h, AbstractFlame& f, AbstractWheel& w) {
        handle = new CheshireHandle(h.getWeight());
        flame = new CheshireFlame(f.getWeight());
        wheels[0] = new CheshireWheel(w.getWeight());
    }
    ~CheshireBicycle() {
        delete handle;
        delete flame;
        delete wheels[0];
    }
    void ride() const override {
        cout << "チェシャ猫チャリをこぐよ。" << endl;
    }
    void accelerate() const override {
        cout << "チェシャ猫チャリを加速させるよ。" << endl;
    }
    void turnLeft() const override {
        cout << "チェシャ猫チャリを左に曲げるよ。" << endl;
    }
    void turnRight() const override {
        cout << "チェシャ猫チャリを右に曲げるよ。" << endl;
    }
    void brake() const override {
        cout << "チェシャ猫チャリ、ブレーキをかけるよ。" << endl;
    }
    void back() const override {
        cout << "チェシャ猫チャリ、バックするよ。" << endl;
    }
    // コンテナ処理の確認用に追加したメンバ関数。
    // Handle, Flame, Wheel 各パーツの合計を返却する。
    int totalWeight() const override {
        int sum = handle->getWeight();
        sum += flame->getWeight();
        sum += wheels[0]->getWeight();
        return sum;
    }
};
class CheshireBicycleFactory final : public AbstractBicycleFactory<CheshireBicycle> {
public:
    AbstractHandle createHandle() const override {
        CheshireHandle handle(285);
        return handle;
    }
    AbstractFlame createFlame() const override {
        CheshireFlame flame(1495);
        return flame;
    }
    optional<AbstractTransmission> createTransmission() const override {
        optional<AbstractTransmission> optionTMission;
        return optionTMission;
    }
    AbstractWheel createWheel() const override {
        CheshireWheel wheel(745);
        return wheel;
    }
    CheshireBicycle madeInFactory() const override {
        AbstractHandle h = createHandle();
        AbstractFlame f = createFlame();
        AbstractWheel w = createWheel();
        CheshireBicycle bicycle(h,f,w);
        return bicycle;
    }
};
void test_Cheshire_Bicycle_Factory() {
    ptr_lambda_message<const char&,const int&,const string&>('-',10,"test_Cheshire_Bicycle_Factory");
    CheshireBicycleFactory factory;
    optional<AbstractTransmission> optionTMission = factory.createTransmission();
    AbstractHandle handle = factory.createHandle();
    AbstractFlame flame = factory.createFlame();
    AbstractWheel wheel = factory.createWheel();

    ptr_lambda_debug<const string&,const int&>("optionTMission.has_value() is ", optionTMission.has_value());
    ptr_lambda_debug<const string&,const int&>("handle weight is ", handle.getWeight());
    ptr_lambda_debug<const string&,const int&>("flame weight is ", flame.getWeight());
    ptr_lambda_debug<const string&,const int&>("wheel weight is ", wheel.getWeight());

    CheshireBicycle bicycle = factory.madeInFactory();
    bicycle.ride();
    bicycle.accelerate();
    bicycle.turnLeft();
    bicycle.turnRight();
    bicycle.brake();
    bicycle.back();

    ptr_lambda_debug<const string&,const int&>("total weight is ",bicycle.totalWeight());
}

//
// Queen Bicycle Parts
//

class QueenHandle final : public AbstractHandle {
public:
    QueenHandle(const int& w) {
        weight = w;
    }
    ~QueenHandle() {}
};
class QueenFlame final : public AbstractFlame {
public:
    QueenFlame(const int& w) {
        weight = w;
    }
    ~QueenFlame() {}
};
class QueenTransmission final : public AbstractTransmission {
public:
    QueenTransmission(const int& w) {
        weight = w;
    }
    ~QueenTransmission() {}
};
class QueenWheel final : public AbstractWheel {
public:
    QueenWheel(const int& w) {
        weight = w;
    }
    ~QueenWheel() {}
};
class QueenOfHeartsCycle final : public AbstractBicycle {
    QueenHandle* handle;
    QueenFlame* flame;
    QueenTransmission* mission;
    std::array<QueenWheel*,2> wheels;
    QueenOfHeartsCycle():handle{nullptr},flame{},mission{nullptr},wheels{} {}
public:
    QueenOfHeartsCycle(AbstractHandle& h, AbstractFlame& f, AbstractTransmission& m, AbstractWheel& fw, AbstractWheel& rw) {
        handle = new QueenHandle(h.getWeight());
        flame = new QueenFlame(f.getWeight()); 
        mission = new QueenTransmission(m.getWeight());
        wheels[0] = new QueenWheel(fw.getWeight());
        wheels[1] = new QueenWheel(rw.getWeight());
    }
    ~QueenOfHeartsCycle() {
        delete handle;
        delete flame;
        delete mission;
        delete wheels[0];
        delete wheels[1];
    }
    void ride() const override {
        cout << "ハートの女王チャリをこぐよ。" << endl;
    }
    void accelerate() const override {
        // TODO ここも詰めたら面白そうだよ。
        cout << "ハートの女王チャリを加速させるよ。" << endl;
        cout << "ハートの女王チャリを >> さらに加速させるよ。" << endl;
        cout << "ハートの女王チャリ、 >> >> 音速を超えるよ。" << endl;     // これは人力ではないよな、誇大広告だよ。
    }
    void turnLeft() const override {
        cout << "ハートの女王チャリを左に曲げるよ。" << endl;
    }
    void turnRight() const override {
        cout << "ハートの女王チャリを右に曲げるよ。" << endl;
    }
    void brake() const override {
        cout << "ハートの女王チャリ、ブレーキをかけるよ。" << endl;
    }
    void back() const override {
        cout << "ハートの女王チャリ、バックするよ。" << endl;
    }
    int totalWeight() const override {
        int sum = handle->getWeight();
        sum += flame->getWeight();
        sum += mission->getWeight();
        sum += wheels[0]->getWeight();
        sum += wheels[1]->getWeight();
        return sum;
    }
};
class QueenBicycleFactory final : public AbstractBicycleFactory<QueenOfHeartsCycle> {
public:
    AbstractHandle createHandle() const override {
        QueenHandle handle(600);
        return handle;
    }
    AbstractFlame createFlame() const override {
        QueenFlame flame(3000);
        return flame;
    }
    optional<AbstractTransmission> createTransmission() const override {
        optional<QueenTransmission> optionTMission;
        QueenTransmission mission(1000);
        optionTMission = mission;
        return optionTMission;
    }
    AbstractWheel createWheel() const override {
        QueenWheel wheel(1050);
        return wheel;
    }
    QueenOfHeartsCycle madeInFactory() const override {
        AbstractHandle h = createHandle();
        AbstractFlame f = createFlame();
        optional<AbstractTransmission> optionTMission = createTransmission();
        AbstractWheel fw = createWheel();
        AbstractWheel rw = createWheel();
        assert(optionTMission.has_value() == true);
        cout << "optionTMission.value().getWeight() is " << optionTMission.value().getWeight() << endl;
        QueenOfHeartsCycle cycle(h,f,optionTMission.value(),fw,rw);
        return cycle;
    }
};
void test_Queen_Bicycle_Factory() {
    ptr_lambda_message<const char&,const int&,const string&>('-',10,"test_Queen_Bicycle_Factory");
    QueenBicycleFactory factory;
    optional<AbstractTransmission> optionTMission = factory.createTransmission();
    AbstractHandle handle = factory.createHandle();
    AbstractFlame flame = factory.createFlame();
    AbstractWheel wheel = factory.createWheel();

    ptr_lambda_debug<const string&,const bool&>("optionTMission.has_value() is ",optionTMission.has_value());
    assert(optionTMission.has_value() == true);
    ptr_lambda_debug<const string&,const int&>("mission weight is ",optionTMission.value().getWeight());
    ptr_lambda_debug<const string&,const int&>("handle weight is ",handle.getWeight());
    ptr_lambda_debug<const string&,const int&>("flame weight is ",flame.getWeight());
    ptr_lambda_debug<const string&,const int&>("wheel weight is ",wheel.getWeight());

    QueenOfHeartsCycle cycle = factory.madeInFactory();
    cycle.ride();
    cycle.accelerate();
    cycle.turnLeft();
    cycle.turnRight();
    cycle.brake();
    cycle.back();

    // cycle.totalWeight();
   ptr_lambda_debug<const string&,const int&>("total weight is ",cycle.totalWeight());

}

//
// Alice Bicycle Parts
//

class AliceHandle final : public AbstractHandle {
public:
    AliceHandle(const int& w) {
        weight = w;
    }
    AliceHandle(const AliceHandle& cpy) {   // 今回は、使わないと思うけど念のため、コピーコンストラクタを用意した。
        weight = cpy.getWeight();
    }
    ~AliceHandle(){}
    int getWeight() const {     // const cv修飾、この場合、const AliceHandle ah; ah.getWeight(); を許可している。 
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
class AliceSportBicycle final : public AbstractBicycle, JabberDecoAdapter { // C++ は多重継承ができる。
    AliceHandle* handle;
    AliceFlame* flame;
    std::array<AliceWheel*,2> wheels;
    AliceSportBicycle():handle{nullptr},flame{nullptr},wheels{} {}
public:
    AliceSportBicycle(AbstractHandle& h, AbstractFlame& f, AbstractWheel& fw, AbstractWheel& rw) {
        handle = new AliceHandle(h.getWeight());
        flame = new AliceFlame(f.getWeight());
        wheels[0] = new AliceWheel(fw.getWeight());
        wheels[1] = new AliceWheel(rw.getWeight());
    }
    ~AliceSportBicycle() {
        delete handle;
        delete flame;
        delete wheels[0];
        delete wheels[1];
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
    int totalWeight() const override {
        int sum = handle->getWeight();
        sum += flame->getWeight();
        sum += wheels[0]->getWeight();
        sum += wheels[1]->getWeight();
        return sum;
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
    ptr_lambda_debug<const string&,const int&>("total weight is ",aliceSport.totalWeight());
}

/**
 * いつものように、ザックリとしたイメージでここまできたが。
 * ここからが、今回のメインですよ。
 * Truck は decoLevel で普通のソートをやってみる。
 * 
 * AbstractBicycle の派生クラスは、各パーツの重量 weight の合計を計算して
 * それをソートしてみたい。
 * 
 * 関数呼び出し演算子 operator() を実装する必要があることを改めて知る。
 * もう忘れかけてる自分がこわい。truck はこれで行けそうだな。
 * 
 * Bicycle の weight 合計は一度別の何かで集計しておかないと無理っぽいな。
 * AbstractBicycle のメンバ関数でPure Virtual なメンバ関数を宣言して、
 * 派生クラスで定義してもらおうか、いや、基底クラスで一撃。。。
 * にはできないのか。AbstractBicycleはインタフェースだったか。
 * では、最初の案を採用しよう、そう仕様。（近年のおじさん化に歯止めが効かなくなってるな。
 * 
*/

class CompDecoLevel {
public:
    bool operator()(const pair<string,int>& l, const pair<string,int>& r ) const {
        return l.second < r.second;
    }
};
// TODO 飽きた、同じだもの。
class CompBicycleTotalWeight {
public:
};

void test_Vector_Container() {
    ptr_lambda_message<const char&,const int&,const string&>('-',10,"test_Vector_Container");
    Truck normal;

    Truck base4Alice("Alice",128);
    AliceTruck alice(base4Alice);
    ptr_lambda_debug<const string&,const int&>("alice decoLevel is", alice.getDecoLevel());

    Truck base4Cheshire("Cheshire",256);
    AliceTruck alice4Cheshire(base4Cheshire);
    CheshireTruck cheshire(alice4Cheshire);
    ptr_lambda_debug<const string&,const int&>("cheshire decoLevel is", cheshire.getDecoLevel());

    Truck base4Jabber("Jabberwocky",512);
    AliceTruck alice4Jabber(base4Jabber);
    JabberTruck jabber(alice4Jabber);
    ptr_lambda_debug<const string&,const int&>("jabber decoLevel is", jabber.getDecoLevel());

    Truck base4Humpt("Humptydumpty",1024);
    AliceTruck alice4Humpt(base4Humpt);
    CheshireTruck cheshire4Humpt(alice4Humpt);
    HumptyTruck humpty(cheshire4Humpt);
    ptr_lambda_debug<const string&,const int&>("humpty decoLevel is", humpty.getDecoLevel());
    humpty.front();
    humpty.side();
    humpty.rear();
    humpty.top();

    // std::vector<pair<string,int>> trucks {
    //     base4Alice.type_decolevel(),
    //     base4Jabber.type_decolevel(),
    //     base4Cheshire.type_decolevel(),
    //     base4Humpt.type_decolevel(),
    //     normal.type_decolevel()
    // };
    std::vector<pair<string,int>> trucks;
    trucks.emplace_back(base4Alice.type_decolevel());
    trucks.emplace_back(base4Jabber.type_decolevel());
    trucks.emplace_back(base4Cheshire.type_decolevel());
    trucks.emplace_back(base4Humpt.type_decolevel());
    trucks.emplace_back(normal.type_decolevel());

    ptr_lambda_debug<const string&,const int&>("Before sort...",0);
    std::for_each(trucks.begin(), trucks.end(), [](pair<string,int> x) {
        cout << x.first << '\t' << x.second << endl;
    });
    CompDecoLevel compDecoLevel;
    std::sort(trucks.begin(),trucks.end(),compDecoLevel);
    ptr_lambda_debug<const string&,const int&>("After sort...",0);
    std::for_each(trucks.begin(), trucks.end(), [](pair<string,int> x) {
        cout << x.first << '\t' << x.second << endl;
    });
    // うん、もう飽きた。

    // ここで構造化束縛もねじ込む。
    // これでやりたいよな、そう考えてのメンバ変数だと思ってるよ。
    // std::vector<pair<string,int>> trucks;


}

int main() {
    // cout << format(":=^10","START") << endl;
    // ザクとは違うのだよ、ザクとはっ。幸先のいい走り出しじゃないか :) 皮肉だな :)
    ptr_lambda_message<const char&,const int&,const string&>('=',15,"コンテナとアルゴリズム START");
    test_Truck();
    test_Alice_Truck();
    test_Cheshire_Truck_Base_Normal();
    test_Cheshire_Truck_Base_Alice();
    test_Jabber_Truck();
    test_Humpty_Truck();
    test_Cheshire_Bicycle_Factory();
    test_Queen_Bicycle_Factory();
    test_Alice_Bicycle_Factory();
    // ここから本題です。
    test_Vector_Container();
    ptr_lambda_message<const char&,const int&,const string&>('=',15,"コンテナとアルゴリズム END");
    return 0;
}
// ソースを眺めてて、クラス、オブジェクト、その参照とポインタで
// Abstract Factoryで作られるチャリのパーツを なぜ new したポインタで管理しな
// ければいけないか、理由が少し見えた。
// TDecorator の派生クラスである各トラックは自分の意図したもので、なぜ、
// Abstract Factory では意図したものにならなかったのか。
// Abstract Factory は途中でキャストされる。ファクトリで作られるものは
// アブストラクトではなく、その派生クラスのオブジェクトだからだ。
// これが、Decorator と Abstract Factory で生じた差異。
// ソースからはそう見える。
// 
// Abstract Factory で各パーツをその派生クラスのオブジェクトにしているのは
// 勿論自分の意図なのだが。だって、変でしょ各工場内で作られるものが同じなんて。
// 別のオブジェクトを作ってなんぼ、ただし、それらのパーツは基底クラスを
// 持っている。それがインタフェースなだけで中身は個々に別のオブジェクト。
// このあたりの自分の解釈が、結果、であれば、そのように作りなさいよと
// コンパイラに注意されまくったと。コンパイラとオイラのエゴの共存点が今回の 
// new したポインタだったと、今はそう思ってる。うん、Abstract Factory いらね。
