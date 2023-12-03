/**
 * 10 章 State パターン
 * 
 * 物事の状態
 * 
 * 今回のサンプルは「ガムボール」の仕組み（ガムのガチャ）。
 * 
 * e.g. compile
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_10_state.cpp -o ../bin/main
 * 
*/
#include <iostream>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](auto message, auto debug)-> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * State インらフェースと State クラスを定義する
*/

class State {
public:
    virtual ~State() {}
    virtual void insertQuarter() const = 0; // 25 セント投入
    virtual void ejectQuarter() const = 0;  // 25 セント取り出し
    virtual void turnCrank() const = 0;     // クランクを回す
    virtual void dispense() const = 0;      // 販売（分配）
};

/**
 * 状態をクラスにまとめる
 * - SOLD OUT
 * - NO QUARTER
 * - HAS QUARTER
 * - SOLD
*/

class SoldState final : public virtual State {
public:
    SoldState() {}
    SoldState(const SoldState& own) {*this = own;}
    ~SoldState() {}

    virtual void insertQuarter() const override {
        // TODO 実装
    }
    virtual void ejectQuarter() const override {
        // TODO 実装
    }
    virtual void turnCrank() const override {
        // TODO 実装
    }
    virtual void dispense() const override {
        // TODO 実装
    }
};
class SoldOutState final : public virtual State {
public:
    SoldOutState() {}
    SoldOutState(const SoldOutState& own) {*this = own;}
    ~SoldOutState() {}

    virtual void insertQuarter() const override {
        // TODO 実装
    }
    virtual void ejectQuarter() const override {
        // TODO 実装
    }
    virtual void turnCrank() const override {
        // TODO 実装
    }
    virtual void dispense() const override {
        // TODO 実装
    }
};
class NoQuarterState final : public virtual State {
public:
    NoQuarterState() {}
    NoQuarterState(const NoQuarterState& own) {*this = own;}
    ~NoQuarterState() {}

    virtual void insertQuarter() const override {
        // TODO 実装
    }
    virtual void ejectQuarter() const override {
        // TODO 実装
    }
    virtual void turnCrank() const override {
        // TODO 実装
    }
    virtual void dispense() const override {
        // TODO 実装
    }
};
class HasQuarterState final : public virtual State {
public:
    HasQuarterState() {}
    HasQuarterState(const HasQuarterState& own) {*this = own;}
    ~HasQuarterState() {}

    virtual void insertQuarter() const override {
        // TODO 実装
    }
    virtual void ejectQuarter() const override {
        // TODO 実装
    }
    virtual void turnCrank() const override {
        // TODO 実装
    }
    virtual void dispense() const override {
        // TODO 実装
    }
};


int main(void) {
    puts("START 10 章 State パターン ===");
    if(0.01) {
        double pi = 3.14159;
        ptr_lambda_debug<const char*, const double&>("pi is ",pi);
    }
    puts("=== 10 章 State パターン END");
    return 0;
}