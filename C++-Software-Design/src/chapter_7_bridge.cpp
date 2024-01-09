/**
 * 7 章 Bridge パターン、Prototype パターン、External Polymorphism パターン
 * 
 * ガイドライン 28 ：物理的依存関係を排除するには Bridge パターン
 * 
 * 解明 Bridge パターン
 * Bridge パターンは 1994 年に発行された GoF 本にある古典的デザインパターンの 1 つで、
 * その目的は抽象化により実装詳細をカプセル化し、物理的依存関係を最小化することにあり
 * ます。C++ では、将来の変更を容易にする、コンパイル時ファイアウォールとして機能します。
 * 
 * Bridge パターン
 * 目的：抽出されたクラスと実装を分離して、それらを独立に変更できるようにする。
 * 
 * e.g. compile
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_7_bridge.cpp -o ../bin/main
*/
#include <iostream>
#include <memory>
#include <cassert>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * サンプルは ElectricCar とその Engine で説明している。
*/

/**
 * エンジン 基底クラス
*/
class Engine {
public:
    virtual ~Engine() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
    // more engine-specific functions.
private:
    // ...
};

/**
 * 電気自動車のエンジンクラス
 * エンジンの派生クラス
*/
class ElectricEngine final : public Engine {
public:
    void start() override {
        puts("EE start engine.");
    }
    void stop() override {
        puts("EE stop engine.");
    }
private:
    // ...
};

/**
 * 自動車を表現する基底クラス
*/
class Car {
private:
    std::unique_ptr<Engine> pimpl_;     // Pointer-to-implementation
    // ... more car-specific data members (wheel, drivetrain ... etc)
protected:
    explicit Car(std::unique_ptr<Engine> engine):pimpl_(std::move(engine)) {}
    Engine*       getEngine()       { return pimpl_.get(); }
    const Engine* getEngine() const { return pimpl_.get(); }
public:
    virtual ~Car() = default;
    virtual void drive() = 0;
    virtual void getoff() = 0;
};

/**
 * 電気自動車クラス
*/
class ElectricCar : public Car {
private:
    // std::unique_ptr<Engine> engine_;
public:
//    explicit ElectricCar():engine_{std::make_unique<ElectricEngine>()} {}
    explicit ElectricCar(/* maybe some engine arguments */): Car(std::make_unique<ElectricEngine>(/* engine arguments */)) {}
    void drive() override {
        puts("--- ElectricCar.drive()");
        getEngine()->start();
    }
    void getoff() override {
        puts("--- ElectricCar.getoff()");
        getEngine()->stop();
    }
};

int test_ElectricCar() {
    puts("--- test_ElectricCar");
    try {
        ElectricCar eCar;
        eCar.drive();
        eCar.getoff();
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

/**
 * Strategy パターンとの違いがいまひとつ分かりづらいかな。
 * 自動車の基底クラスができて、少しだけ Strategy との違いが出た。
*/

int main(void) {
    puts("START Bridge パターン ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    if(1.00) {
        int result;
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",result = test_ElectricCar());
        assert(result == 0);
    }
    puts("=== Bridge パターン END");
    return 0;
}