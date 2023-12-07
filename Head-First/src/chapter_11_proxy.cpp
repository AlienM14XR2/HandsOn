/**
 * 11 章 Proxy パターン
 * 
 * オブジェクトアクセスの制御
 * 
 * サンプルでは、10 章 State パターンで作った、ガムボールマシンを再利用するらしい：）
 * それを移植（コピー）する必要があるかな。
 * ここでの具体的なサンプルは、ガムボールマシーンをモニタリングするシステムらしい。
 * 
 * RMI (Remote Method Invocation).
 * Java のサンプルはこれを利用したものだった。
 * 現在私のスキル、知識ではこの仕組みをC++ で実装あるいは利用する術がない。
 * C++ でネットワークを構築、ソケットの利用など知らない：）
 * したがって、Proxy パターンはここで保留、終わりにする。
 * 
 * e.g. compile
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_11_proxy.cpp -o ../bin/main
*/
#include <iostream>
#include <cassert>
#include <random>

using namespace std;

template<class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug)-> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

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
    int a = n%pattern;
    if(a < 0) {
        a *= -1;
    }
    cout << a << endl;
    return a;    
}

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
/**
 * SOLD_OUT 売り切れの状態クラス
*/
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
/**
 * NO_QUARTER 25 セント未投入状態クラス
*/
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
/**
 * HAS_QUARTER 25 セント投入状態クラス
*/
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
/**
 * SOLD 販売可能状態クラス
*/
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

/**
 * WINNER 10回1回 「当たり」の状態クラス
 * 
 * このクラスでは当たりの状態ですべきことのみを行う。
 * 今回はガムボールが追加でもうひとつ貰えること。
 * ※乱数を利用した確率は GumballMachine で行う。
*/
class WinnerState final : public virtual State {
private:
    GumballMachine* gumballMachine;
    WinnerState() {}
public:
    WinnerState(GumballMachine& gm) {
        gumballMachine = &gm;
    }
    WinnerState(const WinnerState& own) {*this = own;}
    ~WinnerState() {}
    virtual void insertQuarter() const override;
    virtual void ejectQuarter() const override;
    virtual void turnCrank() const override;
    virtual void dispense() const override;
};

/**
 * ガムボールマシンクラス
 * ガムボールの販売までの一連の処理を行う。
*/
class GumballMachine {
private:
    State* soldOutState;    // 売りけれ
    State* noQuarterState;  // 25 セント未投入
    State* hasQuarterState; // 25 セント投入
    State* soldState;       // 販売
    State* winnerState;     // ゲーム要素（25 セントで2個もらえる、10 回に 1 回の確率）

    State* state;           // 現在の状態
    int count;              // ガムボールの数
    string location = "";   // 位置情報（文字列）
    GumballMachine():soldOutState{nullptr},noQuarterState{nullptr},hasQuarterState{nullptr},soldState{nullptr},winnerState{nullptr},state{nullptr},count{0} {}
    void initBaseMembers(const int& numberGumballs) {
        count = numberGumballs;
        soldOutState = new SoldOutState(*this);
        noQuarterState = new NoQuarterState(*this);
        hasQuarterState = new HasQuarterState(*this);
        soldState = new SoldState(*this);
        winnerState = new WinnerState(*this);
        if(count > 0) {
            state = noQuarterState;
        } else {
            state = soldOutState;
        }
    }
public:
    GumballMachine(const int& numberGumballs) {
        initBaseMembers(numberGumballs);
    }
    GumballMachine(const string& lcation, const int& numberGumballs) {
        initBaseMembers(numberGumballs);
        location = lcation;
    }
    GumballMachine(const GumballMachine& own) {*this = own;}
    ~GumballMachine() {
        delete soldOutState;
        delete noQuarterState;
        delete hasQuarterState;
        delete soldState;
        delete winnerState;
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
    string getLocation() noexcept {
        return location;
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
    State* getWinnerState() noexcept {
        return winnerState;
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

int test_GamballMachine_Winner() {
    puts("--- test_GamballMachine_Winner");
    try {
        GumballMachine gumballMachine(200);
        for(int i=0; i < 100; i++) {
            gumballMachine.insertQuarter();
            gumballMachine.turnCrank();
        }
        return 0;
    } catch(exception& e) {
        cout << e.what() << endl;
        return -1;
    }
}

/**
 * GumballMonitor
 * 
 * 今回のサンプルのメイン。
 * 本クラスは、GumballMachine の位置、ガムボールの在庫、現在のマシンの状態を
 * 取得し、簡潔なレポートを出力する。
*/
class GumballMonitor {
private:
    GumballMachine* machine;
    GumballMonitor():machine{nullptr} {}
public:
    GumballMonitor(GumballMachine& gm) {machine = &gm;}
    GumballMonitor(const GumballMonitor& own) {*this = own;}
    ~GumballMonitor() {}

    void report() {
        cout << "ガムボールマシーン：" << machine->getLocation() << endl;
        cout << "現在の在庫：" << machine->getCount() << endl;
        cout << "現在の状態：" << machine->getState() << endl;
    }
};

int main(void) {
    puts("START 11 章 Proxy パターン ===");
    if(0.01) {
        double pi = 3.14159;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    if(0.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_GamballMachine_Winner());
    }
    puts("=== 11 章 Proxy パターン END");
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
        int winner = test_random(10);
        if(winner == 0 && gumballMachine->getCount() >= 2) {
            gumballMachine->setState(gumballMachine->getWinnerState());
        } else {
            gumballMachine->setState(gumballMachine->getSoldState());
        }
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

    /**
     * SoldState と WinnerState の実装はほとんど同じ、また各 State のサブクラスで不要な実装を行っているのも事実。
     * TODO 時間があれば、State の各パブリックメソッドを純粋仮想関数ではなく例外実装にしてみる。
     * 
     * また、turnCrank() に例外或いは bool 値を返却するようにして dispose() を制御できないか考えてみる。
     */

    void WinnerState::insertQuarter() const {
        // この状態での不適切なアクション
        puts("お待ちください、ガムボールを出す準備をしています");
    }
    void WinnerState::ejectQuarter() const {
        // この状態での不適切なアクション
        puts("申し訳ありません、すでにハンドルを回しています");
    }
    void WinnerState::turnCrank() const {
        // この状態での不適切なアクション
        puts("2 回回しても、もうひとつガムボールを手に入れることはできません：）");
    }
    void WinnerState::dispense() const {
        // 当たりの状態が可能、残りの count が 2 個以上あることは、GumballMachine 側で調べること。
        gumballMachine->releaseBall();
        puts("当たりです、25 セントで 2 つのガムボールがもらえます");
        gumballMachine->releaseBall();
        if(gumballMachine->getCount() == 0) {
            gumballMachine->setState(gumballMachine->getSoldOutState());
        } else {
            if(gumballMachine->getCount() > 0) {
                gumballMachine->setState(gumballMachine->getNoQuarterState());
            } else {
                puts("おっと、ガムボールがなくなりました！");
                gumballMachine->setState(gumballMachine->getSoldOutState());
            }                
        }
    }
