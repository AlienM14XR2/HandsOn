/**
 * 7 章 Adapter パターンと Facade パターン
 * 
 * 適合可能にする。
 * 
 * Strategy パターン（1 章）で登場した Duck インタフェースとカモクラスの簡易版を利用する。
 * 
 * カモのように歩き、カモのようにガーガー鳴くものがいたら、それはカモに違いない（打ち消し）
 * カモアダプタでラップされた七面鳥かもしれない。
 * 
 * e.g. compile
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_7_adapter_facade.cpp -o ../bin/main
*/
#include <iostream>
#include <random>
#include <cassert>
#include <vector>
#include <stdarg.h>

using namespace std;

template<class M,class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * Duck インタフェース
 * 既存のもの
*/
class Duck {
public:
    virtual ~Duck() {
        ptr_lambda_debug<const char*,const int&>("DONE. Duck Destructor.",0);
    }
    virtual void quack() const = 0;
    virtual void fly() const = 0; 
};

/**
 * カモクラス、Duck インタフェースの具象クラス
 * 既存のもの
*/
class MallardDuck final : public virtual Duck {
public:
    MallardDuck() {}
    MallardDuck(const MallardDuck& own) {*this = own;}
    ~MallardDuck() {
        ptr_lambda_debug<const char*,const int&>("DONE. MallardDuck Destructor.",0);
    }
    virtual void quack() const override {
        puts("ガーガー");
    }
    virtual void fly() const override {
        puts("飛んでいます");
    }
};

/**
 * Turkey インタフェース
 * 新しく追加されたもの
*/
class Turkey {
public:
    virtual ~Turkey() {
        ptr_lambda_debug<const char*,const int&>("DONE. Turkey Destructor.",0);
    }
    virtual void gobble() const = 0;    // 七面鳥はガーガーとは鳴かずに、ゴロゴロと鳴きます（gobble）
    virtual void fly() const = 0;       // 七面鳥は飛べますが、短い距離しか飛べません。
};

/**
 * 七面鳥クラス、Turkey インタフェースの具象クラス
 * 新しく追加されたもの
*/
class WildTurkey final : public virtual Turkey {
public:
    WildTurkey() {}
    WildTurkey(const WildTurkey& own) {*this = own;}
    ~WildTurkey() {
        ptr_lambda_debug<const char*,const int&>("DONE. WildTurkey Destructor.",0);
    }
    virtual void gobble() const override {
        puts("ゴロゴロ");
    }
    virtual void fly() const override {
        puts("短い距離を飛んでいます");
    }
};

int test_MallardDuck_WildTurkey() {
    puts("--- test_MallardDuck_WildTurkey");
    try {
        MallardDuck mallardDuck;
        mallardDuck.quack();
        mallardDuck.fly();

        WildTurkey wildTurkey;
        wildTurkey.gobble();
        wildTurkey.fly();
        return 0;
    } catch(exception& e) {
        cout << e.what() << endl;
        return -1;
    }
}

/**
 * ここで、Duck オブジェクトでは不十分なので、代わりに Turkey オブジェクトを使いたいとします。
 * しかし、当然七面鳥は別インタフェースを持っているので、そのまま使うことはできない。
 * そこで、アダプタを書くのです。
*/
class TurkeyAdapter final : public virtual Duck {
private:
    mutable Turkey* turkey = nullptr;
    TurkeyAdapter() {}
public:
    TurkeyAdapter(Turkey& t) {turkey = &t;}
    TurkeyAdapter(const TurkeyAdapter& own) {*this = own;}
    ~TurkeyAdapter() {}

    virtual void quack() const override {
        turkey->gobble();
    }
    virtual void fly() const override {   // 短い距離しか飛べない七面鳥はループで 5 回羽ばたく必要がある（とした場合：）。
        for(size_t i = 0; i < 5; i++) {
            turkey->fly();
        }
    }
};

/**
 * クライアントは次のようにアダプタを使う。
 * 
 * クライアントがターゲットインタフェース（この場合はDuck）を使ってアダプタのメソッド
 * を呼び出し、アダプタにリクエストを行う
 * 
 * アダプタは、アダプティインタフェース（Turkey）を使ってリクエスをアダプティに
 * 1 つ以上の呼び出しに変換する。
 * 
 * クライアントは呼び出し結果を受け取るが、変換を行ったアダプタの存在については
 * 全く関知しない。
 * 
*/
int test_TurkeyAdapter() {
    puts("--- test_TurkeyAdapter");
    try {
        WildTurkey wildTurkey;
        TurkeyAdapter adapter(wildTurkey);
        adapter.quack();
        adapter.fly();
        return 0;
    } catch(exception& e) {
        cout << e.what() << endl;
        return -1;
    }
}

int test_random(unsigned int pattern) {
    puts("--- test_random");
    // メルセンヌ・ツイスター法による擬似乱数生成器を、
    // ハードウェア乱数をシードにして初期化
    std::random_device seed_gen;
    std::mt19937 engine(seed_gen());
    // 一様実数分布
    // [-1.0, 1.0)の値の範囲で、等確率に実数を生成する
    std::uniform_real_distribution<> dist1(-1.0, 1.0);
    double r1 = dist1(engine);
    cout << r1 << endl;

    int n = (r1*10);
    cout << n << endl;
    int a = n%pattern;
    if(a < 0) {
        a *= -1;
    }
    cout << a << endl;
    return a;    
}

/**
 * Duck を Turkey に変換する、DuckAdapter が必要という問題。
 * ※Turkey より長く飛ぶ Duck をどのようにコーディングするかという問がある。
 * 私なら、Duck を使わずにアダプタのfly() でオーバーライドしたものを使う（コンポジットされたDuck のfly() を呼ばない）。
*/
class DuckAdapter final : public virtual Turkey {
private:
    mutable Duck* duck = nullptr;
    DuckAdapter() {}
public:
    DuckAdapter(Duck& d) {duck = &d;}
    DuckAdapter(const DuckAdapter& own) {*this = own;}
    ~DuckAdapter() {}

    virtual void gobble() const override {
        duck->quack();
    }
    virtual void fly() const override {
        puts("短い距離しか飛べないようにした：）");
        /**
         * サンプルの答えは上記のようなランダム関数より、５回に一回しか飛ばないように調節して
         * 意地でも duck の fly() を利用していた：）
         * 結局オーバーライドするのだから、利用しなければ良いと私は思ったのだがね：）
         * */
    }
};

int test_DuckAdapter() {
    puts("--- test_DuckAdapter");
    try {
        MallardDuck mallardDuck;
        DuckAdapter adapter(mallardDuck);
        adapter.gobble();
        adapter.fly();
        return 0;
    } catch(exception& e) {
        cout << e.what() << endl;
        return -1;
    }
}

/**
 * ここまでのサンプルは「オブジェクトアダプタ」と呼ばれるものであり
 * Adapter パターンには「クラスアダプタ」と呼ばれるものもある。
 * 
 * オブジェクトアダプタはターゲットインタフェースを実装し、アダプタはアダプティで構成される。
 * オブジェクトコンポジションを使い、異なるインタフェースでアダプティをラップする。
 * 
 * クラスアダプタは多重継承を使い表現される（以前学習した際はこっちをやったと思う、C++ は多重継承できるから：）。
*/
class DuckTurkeyAdapter final : public virtual Duck, public virtual Turkey {
public:
    DuckTurkeyAdapter() {}
    DuckTurkeyAdapter(const DuckTurkeyAdapter& own) {*this = own;}
    ~DuckTurkeyAdapter() {}

    virtual void quack() const override {
        puts("ガーガー（新規作成：）");
    }
    virtual void gobble() const override {
        puts("ゴロゴロ（新規作成：）");
    }
    virtual void fly() const override {
        puts("どのように飛ぶかは自由だ：）");
    }
};

int test_DuckTurkeyAdapter() {
    puts("--- test_DuckTurkeyAdapter");
    try {
        DuckTurkeyAdapter adapter;
        adapter.quack();
        adapter.gobble();
        adapter.fly();
        return 0;
    } catch(exception& e) {
        cout << e.what() << endl;
        return -1;
    }
}

/**
 * 次の課題を C++ で書いてみる。
 * 列挙（Enumeration）を反復（Iterator）に適合させる。
 * - Iterator インタフェース
 * - Enumeration インタフェース
 * - Enumeration アダプタ（EnumerationIterator）
*/
template<class T>
class Iterator {
public:
    virtual ~Iterator() {}
    virtual bool hasNext() const = 0;
    virtual T next() const = 0;
    virtual int remove() = 0;
};

template<class T>
class Enumeration {
public:
    ~Enumeration() {}
    virtual bool hasMoreElements() const = 0;
    virtual T nextElement() const = 0;
};

class Integer final {
private:
    int value = 0;
public:
    Integer() {}
    Integer(const Integer& own) {*this = own;}
    ~Integer() {}
    int getValue() noexcept {return value;}
    void setValue(const int& n) noexcept {value = n;}
};

class IntegerEnum final : public virtual Enumeration<Integer*> {
private:
    vector<Integer*> array;
public:
    // コンストラクタは少し工夫したい、考える時間がほしい。
};

// 可変引数リストを扱うのはC++ ではこんな書き方のものもある。
// @see HandsOn/src/gof-bridge.cpp or HandsOn/src/gof-facade.cpp
template<class T,class... ArgTypes>
class Implementer {
public:
    virtual ~Implementer() {}
    virtual T compute(ArgTypes...) const = 0;
    virtual T reverseCompute(ArgTypes...) const = 0;
};

template<class T>
class MyEnum final : public virtual Enumeration<T> {
private:
    vector<T> array;
    MyEnum() {}
public:
    MyEnum(size_t size, ...) {
        va_list args;
        va_start(args, size);
        for(size_t i=0; i < size; i++) {
            array.push_back(va_arg(args, T));
        }
        va_end(args);
    }
    MyEnum(const MyEnum& own) {*this = own;}
    ~MyEnum() {}

    virtual bool hasMoreElements() const override {
        // TODO current にて判断させる。
        return true;
    }
    virtual T nextElement() const override {
        // TODO current に現在の array の位置を記憶させる。
        return array.at(0);
    }

};

int test_MyEnum() {
    puts("--- test_MyEnum");
    try {
        MyEnum<int> myEnum(3,1,2,3);
        ptr_lambda_debug<const char*,const bool&>("hasMoreElements ... ",myEnum.hasMoreElements());
        ptr_lambda_debug<const char*,const bool&>("nextElement ... ",myEnum.nextElement());
        return 0;
    } catch(exception& e) {
        cout << e.what() << endl;
        return -1;
    }
}

template<class T>
class EnumerationIterator final : public virtual Iterator<T> {
private:
    Enumeration<T>* enumeration = nullptr;
    EnumerationIterator():enumeration{nullptr} {}
public:
    EnumerationIterator(Enumeration<T>& e) {
        enumeration = &e;
    }
    ~EnumerationIterator() {}
    virtual bool hasNext() const override {
        return enumeration->hasMoreElements();
    }
    virtual T next() const override {
        return enumeration->nextElement();
    }
    /**
     * Enumeration はremove() をサポートしていない。
     * 「読み取り専用」インタフェースの Enueration に、アダプタで完全に機能 remove() 
     * を実装する方法はない、Java ならその旨を表現したException を throw する。
     * C++ でもできるはず。
    */
    virtual int remove() const override {
        throw runtime_error("Error:  Unsupported Operation.");
    }
};

int test_EnumerationIterator() {
    puts("--- test_EnumerationIterator");
    try {

        return 0;
    } catch(exception& e) {
        cout << e.what() << endl;
        return -1;
    }
}

int main(void) {
    puts("START 7 章 Adapter パターンと Facade パターン ===");
    if('d') {
        double pi = 3.14159;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_MallardDuck_WildTurkey());
    }
    if(1.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_TurkeyAdapter());
    }
    if(1.02) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_DuckAdapter());
        int n = test_random(5);
        ptr_lambda_debug<const char*,const int&>("n is ",n);
    }
    if(1.03) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_DuckTurkeyAdapter());
    }
    if(2.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_MyEnum());
    }
    if(3.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_EnumerationIterator());
    }
    puts("=== 7 章 Adapter パターンと Facade パターン END");
    return 0;
}