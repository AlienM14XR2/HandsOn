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
    virtual ~FlyBehavior() {}
    virtual void fly() const = 0;
};

/**
 * 鳴き方のインタフェース
*/
class QuackBehavior {
public:
    virtual ~QuackBehavior() {}
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
 * Duck（カモ） 抽象クラス
*/
class Duck {
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