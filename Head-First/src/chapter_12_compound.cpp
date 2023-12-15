/**
 * 12 章 Compound パターン
 * 
 * パターンのパターン
 * パターンの連携
 * Compound とは 複合という意味
 * 
 * Compound パターンは複数のパターンを組み合わせ、繰り返し起こる問題や
 * 一般的な問題を解決するための解決方法。
 * MVCがそれに該当する。
 * 
 * 
 * e.g. compile
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_12_compound.cpp -o ../bin/main
*/
#include <iostream>
#include <cassert>
#include <vector>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug)-> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * 個々のカモを管理する必要がる。
 * リアルタイムに個々のカモの鳴き声を管理する方法はとは。
 * Observer パターンを利用する。
*/

class QuackObservable;
/**
 * Observer パターンにおける観察者クラス
*/
class Observer {
public:
    virtual void update(QuackObservable* duck) const =0;
};

/**
 * QuackObservable は、Quackable を監視したい場合に Quackable が実装すべき
 * インタフェースです。
*/
class QuackObservable {
public:
    virtual ~QuackObservable() {}
    virtual void registerObserver(Observer*) {
        throw runtime_error("ERROR: Not Supported Exception.");
    }
    virtual void notifyAll() {
        throw runtime_error("ERROR: Not Supported Exception.");
    }
    virtual string getName() const = 0;
    // オブザーバを削除するメソッドは今回は不要。
};

/**
 * 再びカモ インタフェース（鳴き声）
*/
class Quackable : public virtual QuackObservable {
protected:
    string name = "";
public:
    virtual ~Quackable() {}
    virtual void quack() const = 0;
    virtual string getName() const override {
        throw runtime_error("ERROR: Not Supported Exception.");
    }
};

/**
 * Quackable のサブクラスは QuackObservable にも対応する必要が出てくる。
 * 今回は、ひとつの QuackObservable のサブクラスを定義し、各 Quackable のサブクラスは
 * それをメンバ変数として定義、コンポジションを利用して、コードの重複を避けることにする。
 * つまり、メンバ変数へ処理を移譲する。
 * その クラス Observable
*/
class Observable final : public virtual QuackObservable {
private:
    vector<Observer*> observers;
    QuackObservable* duck;
    Observable():duck{nullptr} {}
public:
    Observable(QuackObservable* d) {duck = d;}
    Observable(const Observable& own) {*this = own;}
    ~Observable() {}
    virtual void registerObserver(Observer* o) {
        observers.push_back(o);
    }
    virtual void notifyAll() {
        size_t size = observers.size();
        for(size_t i=0 ; i < size ; i++) {
            Observer* observer = observers.at(i);
            observer->update(duck);
        }
    }
    virtual string getName() const override {
        return duck->getName();
    }
};

class Ologist final : public virtual Observer {
public:
    virtual void update(QuackObservable* duck) const override {
        cout << "カモの鳴き声学者：" << duck->getName()  << "が鳴きました。" << endl;
    }
};

/**
 * ガチョウ クラス（鳴き声）
*/
class Goose {
public:
    Goose() {}
    Goose(const Goose& own) {*this = own;}
    ~Goose() {
        ptr_lambda_debug<const char*,const int&>("DONE ... Goose Destructor.",0);
    }
    void honk() {
        puts("ガーー（ガチョウ）");
    } 
};

/**
 * マガモの鳴き声クラス
*/
class MallardDuck final : public virtual Quackable {
private:
    Observable* observable = nullptr;
public:
    MallardDuck() {
        name = "マガモ";
        observable = new Observable(this);
    }
    MallardDuck(const MallardDuck& own) {*this = own;}
    ~MallardDuck() {
        delete observable;
    }
    virtual void quack() const override {
        puts("ガーガー");
        notifyAll();
    }
    virtual string getName() const override {
        return name;
    }
    virtual void registerObserver(Observer* observer) {
        observable->registerObserver(observer);
    }
    virtual void notifyAll() const {
        observable->notifyAll();
    }
};
/**
 * 鴨笛クラス
*/
class DuckCall final : public virtual Quackable {
private:
    Observable* observable = nullptr;
public:
    DuckCall() {
        name = "鳩笛";
        observable = new Observable(this);
    }
    DuckCall(const DuckCall& own) {*this = own;}
    ~DuckCall() {
        delete observable;
    }
    virtual void quack() const override {
        puts("ガァガァ");
        notifyAll();
    }
    virtual string getName() const override {
        return name;
    }
    virtual void registerObserver(Observer* observer) {
        observable->registerObserver(observer);
    }
    virtual void notifyAll() const {
        observable->notifyAll();
    }
};
/**
 * ゴム製のアヒル クラス
*/
class RubberDuck final : public virtual Quackable {
private:
    Observable* observable = nullptr;
public:
    RubberDuck() {
        name = "ゴム製のアヒル";
        observable = new Observable(this);
    }
    RubberDuck(const RubberDuck& own) {*this = own;}
    ~RubberDuck() {
        delete observable;
    }
    virtual void quack() const override {
        puts("キューキュー");
        notifyAll();
    }
    virtual string getName() const override {
        return name;
    }
    virtual void registerObserver(Observer* observer) {
        observable->registerObserver(observer);
    }
    virtual void notifyAll() const {
        observable->notifyAll();
    }
};

/**
 * カモを使いたい場所で、ガチョウを使えるようにしたい。
 * Adapter パターンを利用する。
*/
class GooseAdapter final : public virtual Quackable {
private:
    Goose* goose;
    GooseAdapter():goose{nullptr} {}
public:
    GooseAdapter(Goose& g) {
        name = "ガチョウ";
        goose = &g;
    }
    GooseAdapter(const GooseAdapter& own) {*this = own;}
    ~GooseAdapter() {}

    virtual void quack() const override {
        goose->honk();
    }
    virtual string getName() const override {
        return name;
    }
};

/**
 * カモの鳴き声をカウントするには？
 * quackable インタフェースを継承し、そのサブクラスをコンポジションした
 * Decorator パターン を利用する。
*/
static int quacks = 0;
class QuackCounter final : public virtual Quackable {
private:
    Quackable* duck;
    QuackCounter():duck{nullptr} {}
public:
    QuackCounter(Quackable* q) {
        duck = q;
    }
    QuackCounter(const QuackCounter& own) {*this = own;}
    ~QuackCounter() {}

    virtual void quack() const override {
        duck->quack();
        quacks++;
    }
    virtual string getName() const override {
        return duck->getName();
    }
};

/**
 * カモの作成を取り出して１か所に局所化しませんか？
 * カモの作成と修飾を取り出して、カプセル化するのです。
 * 
 * Factory パターン を利用する。
*/
class IDuckFactory {
public:
    virtual ~IDuckFactory() {}
    virtual Quackable* createMallardDuck() const = 0;
    virtual Quackable* createDuckCall() const = 0;
    virtual Quackable* createRubberDuck() const = 0;
};

class ICountingDuckFactory {
public:
    virtual ~ICountingDuckFactory() {}
    virtual Quackable* createMallardDuck(Quackable*) const = 0;
    virtual Quackable* createDuckCall(Quackable*) const = 0;
    virtual Quackable* createRubberDuck(Quackable*) const = 0;
};

class DuckFactory final : public virtual IDuckFactory {
public:
    DuckFactory() {}
    DuckFactory(const DuckFactory& own) {*this = own;}
    ~DuckFactory() {}

    virtual Quackable* createMallardDuck() const override {
        return new MallardDuck();
    }
    virtual Quackable* createDuckCall() const override {
        return new DuckCall();
    }
    virtual Quackable* createRubberDuck() const override {
        return new RubberDuck();
    }
};

class CountingDuckFactory final : public virtual ICountingDuckFactory {
public:
    CountingDuckFactory() {}
    CountingDuckFactory(const CountingDuckFactory& own) {*this = own;}
    ~CountingDuckFactory() {}
    virtual Quackable* createMallardDuck(Quackable* mallardDuck) const override {
        return new QuackCounter(mallardDuck);
    }
    virtual Quackable* createDuckCall(Quackable* duckCall) const override {
        return new QuackCounter(duckCall);
    }
    virtual Quackable* createRubberDuck(Quackable* rubberDuck) const override {
        return new QuackCounter(rubberDuck);
    }
};

/**
 * カモのグループ全体にも操作を適用させる。
 * Composite パターンを利用する。
 * Flock（群れ）クラス。
*/
class Flock final : public virtual Quackable {
private:
    vector<Quackable*> quackers;
public:
    Flock() {}
    Flock(const Flock& own) {*this = own;}
    ~Flock() {}

    void add(Quackable* q) {
        quackers.push_back(q);
    }
    virtual void quack() const override {
        // loop 内で メンバ変数 quackers の quack() を呼ぶ。
        size_t size = quackers.size();
        for(size_t i=0 ; i < size; i++) {
            Quackable* quackable = quackers.at(i);
            quackable->quack();
        }
    }
};

/**
 * ダックシミュレータ クラス
*/
class DuckSimulator {
public:
    void simulate() {
        Quackable* mallardDuck = new MallardDuck();
        Quackable* duckCall = new DuckCall();
        Quackable* rubberDuck = new RubberDuck();
        Goose goose;
        Quackable* gooseAdapter = new GooseAdapter(goose);

        Ologist ologist;
        mallardDuck->registerObserver(&ologist);
        duckCall->registerObserver(&ologist);
        rubberDuck->registerObserver(&ologist);

        Quackable* mdCounter = new QuackCounter(mallardDuck);
        Quackable* dcCounter = new QuackCounter(duckCall);
        Quackable* rdCounter = new QuackCounter(rubberDuck);

        puts("\nカモシミュレータ\n");
        simulate(mdCounter);
        simulate(dcCounter);
        simulate(rdCounter);
        simulate(gooseAdapter);
        printf("カモが鳴いた回数：%d\n",quacks);
        assert(quacks == 3);        // ガチョウアダプタ はカウントしない。

        delete mdCounter;
        delete dcCounter;
        delete rdCounter;

        delete mallardDuck;
        delete duckCall;
        delete rubberDuck;
        delete gooseAdapter;
    }
    void simulate(IDuckFactory& duckFactory, ICountingDuckFactory& countingDuckFactory) {
        Quackable* mallardDuck = duckFactory.createMallardDuck();
        Quackable* duckCall = duckFactory.createDuckCall();
        Quackable* rubberDuck = duckFactory.createRubberDuck();        
        Ologist ologist;
        // ここでオブザーバを追加してもいいが、Flock クラス内で設定することもおそらく可能だと考える。
        mallardDuck->registerObserver(&ologist);
        duckCall->registerObserver(&ologist);
        rubberDuck->registerObserver(&ologist);
        Goose goose;
        Quackable* gooseAdapter = new GooseAdapter(goose);
        // C++ だとあまり代わり映えしない、サンプルは Java なので次のものだけで表現できるが、個人的にメモリ開放が気になったので結局 Factory のインタフェースを２つ用意した。
        Quackable* mdCounter = countingDuckFactory.createMallardDuck(mallardDuck);
        Quackable* dcCounter = countingDuckFactory.createDuckCall(duckCall);
        Quackable* rdCounter = countingDuckFactory.createRubberDuck(rubberDuck);

        Flock flockOfDucks;
        flockOfDucks.add(mdCounter);
        flockOfDucks.add(dcCounter);
        flockOfDucks.add(rdCounter);

        puts("\nカモシミュレータ V2 Factory パターンと Composite パターンを適用した\n");
        simulate(&flockOfDucks);
        simulate(gooseAdapter);
        printf("カモが鳴いた回数：%d\n",quacks);
        assert(quacks == 3);        // ガチョウアダプタ はカウントしない。

        delete mdCounter;
        delete dcCounter;
        delete rdCounter;

        delete mallardDuck;
        delete duckCall;
        delete rubberDuck;
        delete gooseAdapter;
    }
    void simulate(Quackable* duck) {
        duck->quack();
    }
};

int test_DuckSimulator() {
    puts("--- test_DuckSimulator");
    try {
        quacks = 0; // グローバル変数のカウンタをリセット
        DuckSimulator sim;
        sim.simulate();
        return 0;
    } catch(exception& e) {
        cout << e.what() << endl;
        return -1;
    }
}

int test_DuckSimulator_V2() {
    puts("test_DuckSimulator_V2");
    try {
        quacks = 0; // グローバル変数のカウンタをリセット
        DuckSimulator sim;
        DuckFactory duckFactory;
        CountingDuckFactory countingDuckFactory;
        sim.simulate(duckFactory, countingDuckFactory);
        return 0;
    } catch(exception& e) {
        cout << e.what() << endl;
        return -1;
    }
}

int main(void) {
    puts("START 12 章 Compound パターン ===");
    if(0.01) {
        double pi = 3.14159;
        ptr_lambda_debug<const char*,const double&>("pi is ", pi);
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_DuckSimulator());
    }
    if(1.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_DuckSimulator_V2());
    }
    puts("=== 12 章 Compound パターン END");
    return 0;
}