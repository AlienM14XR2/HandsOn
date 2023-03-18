/**
エラーハンドリング ------------------------------------------------

- 古来からの方法、戻り値を利用したもの
- exception throw runtime_exception("foo exception.") の try catch
- thread_local 変数の利用、古来のパターンの最進化かな、結局、
  コードは古来と似通ったものになるはず。

noexcept を関数につけらてたら、処理の高速化につながる可能性がある。
断言はされていない。

デストラクタでは例外を送出してはいけない、例外が発生しうる場合は
デストラクタ内で処理すること、マナーですかね、リークするし。

エラー処理、例外処理とはある程度の知識がないと成立しないよな。
うん、難しいな。駆出しのオイラには荷が重いな。
Java ならまだしも C++ ではその知見がまったく足りていないのが事実だ。

うん、コーディングのスキルアップを兼ねたエラーハンドリングの基本
という所に目標を定めて、進めるかな。

状態が変化する、うん、state パターンでいっかな、今回は。
OK 、GoF は State パターンでいこう。

正常、異常、（終了）。終了はともかく、最初の2つの状態は必須だ。
状態とともに苦手なメモリ管理とその解放もやってみるか。
例えば、命令、関数呼び出しでメモリ領域を確保、異常状態では確保しない。
終了時に全てを解放する。new と delete 、コンストラクタあるいは、関数
でメモリ領域を動的に確保し、デストラクタで解放と、明確にすれば、問題は
起こらないはず :)
標準ライブラリのstack か quewue を使ってみたいな。
今回は stack で。
なんとなく、仕様というより、要求が固まったので、作ってみる。 

- optional を利用する。
- 例外処理はおしなべて、処理の高速化とは相反するらしい。
- assert

*/
#include <iostream>
#include <stack>
#include <string>
#include <string.h>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * 冗長な設計だと思う。
 * 次のことを明確にすることが目的。
 * 
 * 型、ポインタ、参照、メモリ、const の有無。
 * 上記を意識せずに、派生クラスの実装は不可能だから。
 * 
 * また、利用に際しても変数とは何かを考えること。
*/
template <class T>
class EatType {
protected:
    int size;
    string label;   // ドックタグかな。
public:
    EatType():size{-1},label{"none"} {}
    /**
     * labelの参照ができる。
    */
    virtual const string& getLabel() const = 0;
    /**
     * setter
    */
    virtual void eat(T t) const = 0;
    /**
     * gettter
    */
    virtual T spitout() const = 0;
    virtual ~EatType() {}
};

/**
 * 技術検証で行っている。
 * 真似しないでね。
 * こんなことしないで、素直に string& 使えばいいから、たぶん。
 * 
 * const のあり、なしは、ありは一時変数を扱えるだけ。
 * 
 * const char*の型を memory に size 分確保する。
 * 文字列を保存、吐き出すクラス。
*/
class ConstCharEat final : public virtual EatType<const char*> {
    const char* memory;
    ConstCharEat() : memory{nullptr} {}
public:
    // 覚えてたらこれを深堀ってみて、面白そうだよ。
    // void wa_are_badboys(const char* t) {
    // }
    ConstCharEat(const int& sz, const string& lab) {
        size = sz;
        label = lab;
        memory = new char[size];
    }
    const string& getLabel() const override {
        return label;
    }
    void eat(const char* t) const override {
        auto badboy = [this](const char* t) {
            cout << "badboy----- Yeah I'm a Bad Boy :)" << endl;
            cout << "src... t is " << t << endl;
            char* pt = const_cast<char*>(t);
            char* pm = const_cast<char*>(memory);
            strncpy(pm,pt,size);
            cout << "dest... pm is " << pm << endl;
        };
        badboy(t);
        cout << "memory is " << memory << endl;
    }
    const char* spitout() const override {
        return memory;
    }
    ~ConstCharEat() {
        ptr_lambda_debug<const string&,const int&>("memory size is ",size);
        ptr_lambda_debug<const string&,const string&>("label is ",label);
        delete [] memory;
        ptr_lambda_debug<const string&,const int&>(" == DONE delete. ConstCharEat.",0);
    }
};
void test_Const_Char_Eat() {
    cout << "-------------------- test_Char_Eat " << endl;
    ConstCharEat ce(100,"I'll const char* eat prototype :)");
    cout << ce.getLabel() << endl;
    ce.eat("けんかをやめて By 竹内まりや。 いやどんな心境だよ。");
    cout << ce.spitout() << endl;

    ConstCharEat ce1(28,"I'll const char* eat 1st :)");
    cout << ce1.getLabel() << endl;
    ce1.eat("シングルアゲイン。 うん、こわいねこのひと。");
    cout << ce1.spitout() << endl;
    ce1.eat("駅。 スキ。");
    cout << ce1.spitout() << endl;
}
/**
 * string& 型を memory に確保する。
 * 文字列を保存、吐き出すクラス。
*/
class StringEat final : public virtual EatType<string&> {
    string* memory;
public:
    StringEat() : memory{new string()} {}
    StringEat(const int& sz, const string& lab) {
        size = sz;
        label = lab;
        memory = new string();
    }
    StringEat(const StringEat& cpy) {   // copy constructor.
        memory = new string(*cpy.memory);
    }
    const string& getLabel() const override {
        return label;
    }
    void eat(string& t) const override {
        *memory = t;
    }
    string& spitout() const override {
        return *memory;
    }
    ~StringEat() {
        ptr_lambda_debug<const string&,const int&>("memory size is ",size);
        ptr_lambda_debug<const string&,const string&>("label is ",label);
        delete memory;
        ptr_lambda_debug<const string&,const int&>(" == DONE delete. StringEat.",0);
    }
};
void test_String_Eat() {
    cout << "-------------------- test_String_Eat " << endl;
    StringEat se(0,"StringEat proto.");
    string voice = "NUMB FROM METEORA BY LINKIN PARK.";
    // こっちは const 修飾されてないので、一時変数を扱えない、残念でした。
    // 利用側で、変数に格納する必要があるということ。
    // それが、ConstCharEat との違い。でも、ConstCharEat は悪い子ですよ。
    // これが、あるべき使い方のはず。
    se.eat(voice);
    cout << se.spitout() << endl;
    voice = "SONGS ABOUT JANE BY MAROON 5. これもスキなアルバムだね、なんか聞いちゃう。";
    se.eat(voice);
    cout << se.spitout() << endl;
}
/**
 * int& 型を memory に確保する。
 * 整数を保持し吐き出すクラス。
*/
class IntEat final : public virtual EatType<int&> {
    int* memory;
public:
    IntEat() : memory{new int()} {}
    IntEat(const int& sz, const string& lab) {
        size = sz;
        label = lab;
        memory = new int();
    }
    IntEat(const IntEat& cpy) {
        memory = new int(*cpy.memory);
    }
    const string& getLabel() const override {
        return label;
    }
    void eat(int& t) const override {
        *memory = t;
    }
    int& spitout() const override {
        return *memory;
    }
    ~IntEat() {
        ptr_lambda_debug<const string&,const int&>("memory size is ",size);
        ptr_lambda_debug<const string&,const string&>("label is ",label);
        delete memory;
        ptr_lambda_debug<const string&,const int&>(" == DONE delete. IntEat.",0);
    }
};
void test_Int_Eat() {
    cout << "-------------------- test_Int_Eat " << endl;
    IntEat ie(0,"IntEat proto.");
    int num = 90827;
    ie.eat(num);
    cout << ie.spitout() << endl;
    num = 770108;
    ie.eat(num);
    cout << ie.spitout() << endl;
}
/**
 * ここから少し GoF State の実装をやろうか。
 * インタフェースのState、その具象化クラス x 3（正常、異常、致命的）。
 * State 管理クラス。
 * 全部で 5 つかな。
 * 
 * ここまでで、次のようなことを考えているよ。
 * - 正常である場合、メモリ消費し続ける、MemoryEaterがね。
 * - 消費しているメモリは stack にのせる。
 * - 異常になったら、stack に乗っているものを吐き出す。
 * - 次が、まだ未定、吐き出す = spitout なのか、本当にメモリから消すのか。
 * 
 * OK、パターンを一つ追加して、spitout の出番を作ってあげよう。
 * - 異常になったら、spitout する。
 * - 致命的になったら、メモリ解放する。
 * 
 * state のパターンは、正常、異常、致命的の3つ。
 * Director も必要かも。名前はなんにせよ、MemoryEater と State の Action
 * を実行するクラス、じゃぁ、Executor かな。
 * イメージとしては以上だね。ここまでRoleを分ければ、問題になった時も
 * 切り貼りとテストがやりやすいはず :)
 * 
 * 相変わらずのノリに、恐縮です。 (バカにしてるな :)
 * ナメるなよ小僧、IF文を書かずに実装、実行してやる。
 * 
*/
class MemoryHardEater;
class MemoryEasyEater;
template<class T>
class State {
public:
    State(){}
    virtual void action(T t) const = 0;
    virtual ~State(){};
};
class NormalSys final : public State<stack<MemoryEasyEater>> {
public:
    /**
     * メモリを消費する。
    */
    void action(const stack<MemoryEasyEater> stk) const override {
        ptr_lambda_debug<const string&,const int&>("----- action NormalSys.",0);
    }
};
class AbNormalSys final : public State<stack<MemoryEasyEater>> {
public:
    /**
     * spitout の実行。
    */
    void action(const stack<MemoryEasyEater> stk) const override {
        ptr_lambda_debug<const string&,const int&>("----- action AbNormalSys.",0);
    }
};
class FatalSys final : public State<stack<MemoryEasyEater>> {
public:
    /**
     * メモリ の解放。
    */
    void action(const stack<MemoryEasyEater> stk) const override {
        ptr_lambda_debug<const string&,const int&>("----- action FatalSys.",0);
    }
};
// いらね。
// class RootState {
//     State* state;   // 抽象クラスは派生クラスでないここではオプジェクトの実体を持てない、うん。
// public:
//     RootState(const State& s) {
//         *state = s;
//     }
//     void whatsHapped() {
//         state->action();
//     }
//     void changeHapped(const State& s) {
//         *state = s;
//         state->action();
//     }
// };
class Executor final {
    const stack<MemoryEasyEater> eater;
    const NormalSys normal;
    const AbNormalSys abnormal;
    const FatalSys fatal;
public:
    Executor(){
    }
    void eatAction() {
        normal.action(eater);
    }
    void spitAction() {
        abnormal.action(eater);
    }
    void fatalAction() {
        fatal.action(eater);
    }
};
// ひとまず、オイラの理想的な一時実装はできたよ。
// これから、そのテストをやってみる。
// コンパイルは通した。問題、そして怖いのはコアダンプ。
// だから、慎重に進めるのだね。よし、空っぽの実行確認はできた。
// ここでコミットして、その後の変遷を見てみたい。
// この理想がどのように変化あるいは維持、はたまた見落としがあるやも。
// うん、楽しい :)
void test_Execute_States() {
    cout << "-------------------- test_Execute_States " << endl;
    Executor executor;
    executor.eatAction();
    executor.spitAction();
    executor.fatalAction();
}

/**
 * 課題、自作クラスのオブジェクトをポインタで扱ってみよう。
 * 
 * なるほど、あくまでもポインタ管理にこだわるなら、次のようなことを決める必要がある。
 * - メモリは誰が取得し解放するのか。
 * - オブジェクトをNewしたものがその責任を負うべき。
 * 
 * 現状の形なら、このクラスになるけど。
 * このクラス内のメンバオブジェクトをNewしないのであれば、二重解放にはならない。
 * 
 * FIXME EatTypeで扱うことを当初は考えていた（理想ね）。
 * 現状の自分のスキルが足りていないと思い断念した。
*/
class MemoryHardEater final {
    StringEat* stringEat;
    IntEat* intEat;
    MemoryHardEater(): stringEat{nullptr}, intEat{nullptr} {}
public:
    MemoryHardEater(const StringEat& st,const IntEat& ie) {
        stringEat = new StringEat(st);  // 無駄なことしてるんだよな。
        intEat = new IntEat(ie);        // 無駄なことしてるんだよな。
    }
    ~MemoryHardEater(){
        // はい、これがC++ メモリ管理の難しさと面白さの醍醐味だね。
        // このコメントを外すとコアダンプで中止、実行時エラーになる。
        // free(): double free detected in tcache 2
        // 以下は予想にもとづく仮設。
        // 理由はメモリの2重解放をおこなってしまうから。
        // それを回避する方法は幾つかある
        // 1. コンストラクタでオブジェクトの参照をやめてコピーを受け取る。
        // ただし、その場合はコピーコンストラクタを作らないといけない。
        // つまり、無駄なこと、処理速度の低下を招く。
        // コンパイラは非常に効率よく動くし、設計されている。
        // 勉強中だから、敢えて無駄なことをしたいのだが。

        // DONE これが有効になっているということは、二重解放のコアダンプは解消されたのだね。
        // さらに、敢えて無駄なことに挑戦すると。
        delete stringEat;
        delete intEat;
        ptr_lambda_debug<const string&,const int&>("=== DONE delete. MemoryEater...",0);
    }
};
void test_Memory_Hard_Eater() {
    cout << "-------------------- test_Memory_Hard_Eater " << endl;
    // +2個のオーバーロードとのコンパイルエラー、operator のオーバーロード
    // が必要と思われる。仮引数なしに対するありの代入だからだろうね。
    // StringEat se = new StringEat(0,"StringEat RX 78");
    // はい、どれもブッブーでした。オブジェクトをNewするとはつまり
    // ポインタで管理することです。
    StringEat* newSe = new StringEat();
    string stateMessage = "オレの扱いには気をつけろよ、解放忘れるなよ。";
    newSe->eat(stateMessage);
    cout << newSe->spitout() << endl;
    delete newSe;

    IntEat* newIe = new IntEat();
    int n = 333;
    newIe->eat(n);
    cout << newIe->spitout() << endl; 
    delete newIe;

    stateMessage = "オレは平気（兵器）だ。";
    StringEat se(0,"StringEat RX 78");
    se.eat(stateMessage);

    IntEat ie(0,"IntEat RX 93");
    int statePoint = 1000;
    ie.eat(statePoint);

    MemoryHardEater eater(se,ie);
}

/**
 * メンバ変数を参照で扱うため、実装が楽（MemoryHardEaterと比較して）。
 * クラス内で new してオブジェクトを生成していないので、delete する
 * 必要もない。
*/
class MemoryEasyEater final {
    StringEat stringEat;
    IntEat intEat;
    MemoryEasyEater() {}
public:
    MemoryEasyEater(const StringEat& se, const IntEat& ie) noexcept {
        stringEat = se;
        intEat = ie;
    }
    ~MemoryEasyEater() noexcept {
        ptr_lambda_debug<const string&,const int&>("=== DONE delete. MemoryEater...",0);
    }
};

int main() {
    cout << "START エラーハンドリング ========== " << endl;
    ptr_lambda_debug<string,int>("Yeah It's NUMB.",0);
    test_Const_Char_Eat();
    test_String_Eat();
    test_Int_Eat();
    test_Memory_Hard_Eater();
    test_Execute_States();
    cout << "========== エラーハンドリング END" << endl;
    return 0;
}