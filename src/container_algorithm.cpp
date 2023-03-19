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
    AliceTruck(){}
    AliceTruck(const Truck& truck) {
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
    TDecorator* base;

    JabberTruck():base{nullptr} {}
public:
    JabberTruck(TDecorator& truck){
        base = &truck;
    }
    ~JabberTruck(){
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
int main() {
    // cout << format(":=^10","START") << endl;
    // ザクとは違うのだよ、ザクとはっ。幸先のいい走り出しじゃないか :)
    ptr_lambda_message<const char&,const int&,const string&>('=',15,"コンテナとアルゴリズム START");
    test_Truck();
    test_Alice_Truck();
    test_Cheshire_Truck_Base_Normal();
    test_Cheshire_Truck_Base_Alice();
    test_Jabber_Truck();
    ptr_lambda_message<const char&,const int&,const string&>('=',15,"コンテナとアルゴリズム END");
    return 0;
}

