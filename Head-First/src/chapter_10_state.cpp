/**
 * 10 章 State パターン
 * 
 * 物事の状態
 * 
 * State パターンの定義
 * State パターンでは、オブジェクトの内部状態が変化した際にオブジェクトが振る舞いを変更できます。
 * オブジェクトはそのクラスを変更したように見えます。
 * 
 * このパターンは状態を別々のクラスにカプセル化し、現在の状態を表すオブジェクトに移譲するため、
 * 振る舞いは内部状態によって変わります。
 * 
 * 今回のサンプルは「ガムボール」の仕組み（ガムのガチャ）。
 * 
 * e.g. compile
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_10_state.cpp -o ../bin/main
 * 
*/
#include <iostream>
#include <cassert>

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

class GumballMachine;
class SoldOutState final : public virtual State {
private:
    GumballMachine* gumballMachine;
    SoldOutState() {}
public:
    SoldOutState(GumballMachine& gm) {
        gumballMachine = &gm;
    }
    SoldOutState(const SoldOutState& own) {*this = own;}
    ~SoldOutState() {}
    virtual void insertQuarter() const override;
    virtual void ejectQuarter() const override;
    virtual void turnCrank() const override;
    virtual void dispense() const override;
};
class NoQuarterState final : public virtual State {
private:
    GumballMachine* gumballMachine;
    NoQuarterState() {}
public:
    NoQuarterState(GumballMachine& gm) {
        gumballMachine = &gm;
    }
    NoQuarterState(const NoQuarterState& own) {*this = own;}
    ~NoQuarterState() {}
    virtual void insertQuarter() const override;
    virtual void ejectQuarter() const override;
    virtual void turnCrank() const override;
    virtual void dispense() const override;
};
class HasQuarterState final : public virtual State {
private:
    GumballMachine* gumballMachine;
    HasQuarterState() {}
public:
    HasQuarterState(GumballMachine& gm) {
        gumballMachine = &gm;
    }
    HasQuarterState(const HasQuarterState& own) {*this = own;}
    ~HasQuarterState() {}
    virtual void insertQuarter() const override;
    virtual void ejectQuarter() const override;
    virtual void turnCrank() const override;
    virtual void dispense() const override;
};
class SoldState final : public virtual State {
private:
    GumballMachine* gumballMachine;
    SoldState() {}
public:
    SoldState(GumballMachine& gm) {
        gumballMachine = &gm;
    }
    SoldState(const SoldState& own) {*this = own;}
    ~SoldState() {}
    virtual void insertQuarter() const override;
    virtual void ejectQuarter() const override;
    virtual void turnCrank() const override;
    virtual void dispense() const override;
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
        soldOutState = new SoldOutState(*this);
        noQuarterState = new NoQuarterState(*this);
        hasQuarterState = new HasQuarterState(*this);
        soldState = new SoldState(*this);
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

int test_GumballMachine() {
    puts("--- test_GumballMachine");
    try {
        GumballMachine gumballMachine(2);
        gumballMachine.insertQuarter();
        gumballMachine.turnCrank();
        int count = -1;
        ptr_lambda_debug<const char*,const int&>("count is ",count = gumballMachine.getCount());
        assert(count == 1);
        gumballMachine.insertQuarter();
        gumballMachine.turnCrank();
        count = -1;
        ptr_lambda_debug<const char*,const int&>("count is ",count = gumballMachine.getCount());
        gumballMachine.insertQuarter();
        return 0;
    } catch(exception& e) {
        cout << e.what() << endl;
        return -1;
    }
}

int main(void) {
    puts("START 10 章 State パターン ===");
    if(0.01) {
        double pi = 3.14159;
        ptr_lambda_debug<const char*, const double&>("pi is ",pi);
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_GumballMachine());
    }
    puts("=== 10 章 State パターン END");
    return 0;
}

    void SoldOutState::insertQuarter() const {
        // この状態に対する不適切なアクション
        puts("売り切れました、 25 セントを投入することはできません");
    }
    void SoldOutState::ejectQuarter() const {
        // この状態に対する不適切なアクション
        puts("売り切れました、 25 セントは投入されていません、したがって返金もできません");
    }
    void SoldOutState::turnCrank() const {
        // この状態に対する不適切なアクション
        puts("売り切れました、 ハンドルは回せません");
    }
    void SoldOutState::dispense() const {
        // この状態に対する不適切なアクション
        puts("売り切れました、 販売できません");
    }


    void NoQuarterState::insertQuarter() const {
        puts("25 セントが投入されました");
        gumballMachine->setState(gumballMachine->getHasQuarterState());
    }
    void NoQuarterState::ejectQuarter() const {
        // この状態に対する不適切なアクション
        puts("25 セントが投入されていません");
    }
    void NoQuarterState::turnCrank() const {
        // この状態に対する不適切なアクション
        puts("25 セントが投入されていません、ハンドルは回せません");
    }
    void NoQuarterState::dispense() const {
        // この状態に対する不適切なアクション
        puts("25 セントが投入されていません、販売できません");
    }


    void HasQuarterState::insertQuarter() const {
        // この状態に対する不適切なアクション
        puts("もう一度 25 セントを投入することはできません");
    }
    void HasQuarterState::ejectQuarter() const {
        puts("25 セントを返却しました");
        gumballMachine->setState(gumballMachine->getNoQuarterState());
    }
    void HasQuarterState::turnCrank() const {
        puts("ハンドルを回しました");
        gumballMachine->setState(gumballMachine->getSoldState());
    }
    void HasQuarterState::dispense() const {
        // この状態に対する不適切なアクション
        puts("ガムボールがでません");
    }


    void SoldState::insertQuarter() const {
        // この状態での不適切なアクション
        puts("お待ちください、ガムボールを出す準備をしています");
    }
    void SoldState::ejectQuarter() const {
        // この状態での不適切なアクション
        puts("申し訳ありません、すでにハンドルを回しています");
    }
    void SoldState::turnCrank() const {
        // この状態での不適切なアクション
        puts("2 回回しても、もうひとつガムボールを手に入れることはできません：）");
    }
    void SoldState::dispense() const {
        gumballMachine->releaseBall();
        if(gumballMachine->getCount() > 0) {
            gumballMachine->setState(gumballMachine->getNoQuarterState());
        } else {
            puts("おっと、ガムボールがなくなりました！");
            gumballMachine->setState(gumballMachine->getSoldOutState());
        }
    }
