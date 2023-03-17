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
 * string& を memory に確保する。
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
 * int& を memory に確保する。
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
    MemoryEasyEater(const StringEat& se, const IntEat& ie) {
        stringEat = se;
        intEat = ie;
    }
    ~MemoryEasyEater(){
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
    cout << "========== エラーハンドリング END" << endl;
    return 0;
}