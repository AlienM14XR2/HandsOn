/**
 * Head First デザインパターン
 * １章 デザインパターン入門
 * 
 * ```
 * e.g. コンパイル
 * g++ -O3 -std=c++20 -pedantic-errors -Wall -Werror chapter_1.cpp -o ../bin/main
 * ```
*/
#include <iostream>

using namespace std;

template< class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * 飛び方のインタフェース
*/
class FlyBehavior {
public:
    virtual ~FlyBehavior() {
        ptr_lambda_debug<const string&,const int&>("Done FlyBehavior destructor.",0);
    }
    virtual void fly() const = 0;
};

/**
 * 鳴き方のインタフェース
*/
class QuackBehavior {
public:
    virtual ~QuackBehavior() {
        ptr_lambda_debug<const string&,const int&>("Done QuackBehavior destructor.",0);        
    }
    virtual void quack() const = 0;
};

/**
 * 飛び方の具象化クラス
 * 
 * 翼で飛べる。
*/
class FlyWithWing final : public virtual FlyBehavior {
public:
    FlyWithWing() noexcept {}
    FlyWithWing(const FlyWithWing& own) noexcept {
        (*this) = own;
    }
    ~FlyWithWing() noexcept {}
    virtual void fly() const override {
        puts("飛んでいます！！");
    }
};

/**
 * 飛び方の具象化クラス
 * 
 * 飛べない。
*/
class FlyNoWay final : public virtual FlyBehavior {
public:
    FlyNoWay() noexcept {}
    FlyNoWay(const FlyNoWay& own) noexcept {
        (*this) = own;
    }
    ~FlyNoWay() noexcept {}
    virtual void fly() const override {
        puts("飛べません");
    }
};

/**
 * 鳴き方の具象化クラス
 * 
 * ガーガー鳴く。
*/
class Quack final : public virtual QuackBehavior {
public:
    Quack() noexcept {}
    Quack(const Quack& own) noexcept {
        (*this) = own;
    }
    ~Quack() noexcept {}
    virtual void quack() const override {
        puts("ガーガー");
    }
};

/**
 * 鳴き方の具象化クラス
 * 
 * 沈黙（Mute）。
*/
class MuteQuack final : public virtual QuackBehavior {
public:
    MuteQuack() noexcept {}
    MuteQuack(const MuteQuack& own) noexcept {
        (*this) = own;
    }
    ~MuteQuack() noexcept {}
    virtual void quack() const override {
        puts("<<沈黙>>");
    }
};

/**
 * 鳴き方の具象化クラス
 * 
 * キューキュー鳴く。
*/
class Squeak final : public virtual QuackBehavior {
public:
    Squeak() noexcept {}
    Squeak(const Squeak& own) noexcept {
        (*this) = own;
    }
    ~Squeak() noexcept {}
    virtual void quack() const override {
        puts("キューキュー");
    }
};

/**
 * Duck（カモ） 抽象クラス
*/
class Duck {
protected:
    FlyBehavior* flyBehavior;
    QuackBehavior* quackBehavior;
public:
    Duck() noexcept {}
    Duck(const Duck& own) {
        (*this) = own;
    }
    virtual ~Duck() {
    }
    virtual void display() const = 0;
    void swim() {
        puts("すべてのカモは浮きます、おとりのカモでも！");
    }
    void setFlyBehavior(FlyBehavior* const fb) noexcept {
        flyBehavior = fb;
    }
    void setQuackBehavior(QuackBehavior* const qb) noexcept {
        quackBehavior = qb;
    }
    void performFly() {
        flyBehavior->fly();
    }
    void performQuack() {
        quackBehavior->quack();
    }
};

/**
 * マガモ クラス
 * カモの具象化クラス
*/
class MallardDuck final : public virtual Duck {
public:
    MallardDuck() {}
    MallardDuck(const MallardDuck& own) {
        (*this) = own;
    }
    ~MallardDuck() {}
    virtual void display() const override {
        puts("私は本物のマガモです");
    }
};

int testMallardDuck() {
    puts("--- testMallardDuck");
    try {
        MallardDuck mallardDuck;
        mallardDuck.display();

        FlyWithWing* flyWithWing = new FlyWithWing();
        FlyBehavior* flyBehavior = static_cast<FlyBehavior*>(flyWithWing);
        Quack quack;
//        QuackBehavior* quackBehavior = static_cast<QuackBehavior*>(&quack);
        mallardDuck.setFlyBehavior(flyBehavior);
        mallardDuck.setQuackBehavior(&quack);   // インタフェースを利用しなくても問題ない。

        mallardDuck.performFly();
        mallardDuck.performQuack();

        delete flyWithWing;
    } catch(exception& e) {
        cout << e.what() << endl;
        return 1;
    }
    return 0;
}

int main(void) {
    puts(" ========= START １章 デザインパターン入門");
    if(1) {
        int n = 3;
        double pi = 3.141592;
        ptr_lambda_debug<const char*, const int&>("n is ",n);
        ptr_lambda_debug<const char*, const double&>("pi is ",pi);
    }
    if(1.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result .... ",testMallardDuck());
    }
    puts("１章 デザインパターン入門 END =========");
}