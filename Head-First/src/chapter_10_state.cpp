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

class GumballMachine {
private:
    // int SOLD_OUT = 0;
    // int NO_QUARTER = 1;
    // int HAS_QUARTER = 2;
    // int SOLD = 3;
    // mutable int state = SOLD_OUT;

    State* soldOutState;    // 売りけれ
    State* noQuarterState;  // 25 セント未投入
    State* hasQuarterState; // 25 セント投入
    State* soldState;       // 販売
    State* state;           // 現在の状態
    int count;              // ガムボールの数
    GumballMachine():soldOutState{nullptr},noQuarterState{nullptr},hasQuarterState{nullptr},soldState{nullptr},state{nullptr},count{0} {}
public:
    GumballMachine(const int& numberGumballs) {
        count = numberGumballs;
        soldOutState = new SoldOutState();
        noQuarterState = new NoQuarterState();
        hasQuarterState = new HasQuarterState();
        soldState = new SoldState();
        if(count > 0) {
            state = noQuarterState;
        } else {
            state = soldOutState;
        }
    }
    GumballMachine(const GumballMachine& own) {*this = own;}
    ~GumballMachine() {
        delete soldOutState;
        delete noQuarterState;
        delete hasQuarterState;
        delete soldState;
    }

    void insertQuarter() {
        state->insertQuarter();
    }
    void ejectQuater() {
        state->ejectQuarter();
    }
    void turnCrank() {
        state->turnCrank();
        state->dispense();
    }
    void releaseBall() {
        puts("ガムがスロットから出てきます");
        if(count != 0) {
            count = count - 1;
        }
    }
    State* getState() noexcept {
        return state;
    }
    void setState(State* s) noexcept {
        state = s;
    }
    int getCount() noexcept {
        return count;
    }
    State* getSoldOutState() noexcept {
        return soldOutState;
    }
    State* getNoQuarterState() noexcept {
        return noQuarterState;
    }
    State* getHasQuarterState() noexcept {
        return hasQuarterState;
    }
    State* getSoldState() noexcept {
        return soldState;
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