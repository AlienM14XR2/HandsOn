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

class DuckSimulator {
public:
    void simulate() {
        Quackable* mallardDuck = new MallardDuck();
        Quackable* duckCall = new DuckCall();
        Quackable* rubberDuck = new RubberDuck();
        Goose goose;
        Quackable* gooseAdapter = new GooseAdapter(goose);

        puts("\nカモシミュレータ\n");
        simulate(mallardDuck);
        simulate(duckCall);
        simulate(rubberDuck);
        simulate(gooseAdapter);

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
    puts("--- test_Ducks");
    try {
        DuckSimulator sim;
        sim.simulate();
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
    puts("=== 12 章 Compound パターン END");
    return 0;
}