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
 * 再びカモ インタフェース（鳴き声）
*/
class Quackable {
public:
    virtual ~Quackable() {}
    virtual void quack() const = 0;
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
public:
    MallardDuck() {}
    MallardDuck(const MallardDuck& own) {*this = own;}
    ~MallardDuck() {}
    virtual void quack() const override {
        puts("ガーガー");
    }
};
/**
 * 鴨笛クラス
*/
class DuckCall final : public virtual Quackable {
public:
    DuckCall() {}
    DuckCall(const DuckCall& own) {*this = own;}
    ~DuckCall() {}
    virtual void quack() const override {
        puts("ガァガァ");
    }
};
/**
 * ゴム製のアヒル クラス
*/
class RubberDuck final : public virtual Quackable {
public:
    RubberDuck() {}
    RubberDuck(const RubberDuck& own) {*this = own;}
    ~RubberDuck() {}
    virtual void quack() const override {
        puts("キューキュー");
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
        goose = &g;
    }
    GooseAdapter(const GooseAdapter& own) {*this = own;}
    ~GooseAdapter() {}

    virtual void quack() const override {
        goose->honk();
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

    // int getQuacks() {
    //     return quacks;
    // }
    virtual void quack() const override {
        duck->quack();
        quacks++;
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
        // TODO for loop 内で メンバ変数 quackers の quack() を呼ぶ。
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
        Goose goose;
        Quackable* gooseAdapter = new GooseAdapter(goose);
        // C++ だとあまり代わり映えしない、サンプルは Java なので次のものだけで表現できるが、個人的にメモリ開放が気になったので結局 Factory のインタフェースを２つ用意した。
        Quackable* mdCounter = countingDuckFactory.createMallardDuck(mallardDuck);
        Quackable* dcCounter = countingDuckFactory.createDuckCall(duckCall);
        Quackable* rdCounter = countingDuckFactory.createRubberDuck(rubberDuck);

        puts("\nカモシミュレータ V2\n");
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