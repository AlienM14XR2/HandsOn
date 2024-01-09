/**
 * 7 章 Bridge パターン、Prototype パターン、External Polymorphism パターン
 * 
 * ガイドライン 28 ：物理的依存関係を排除するには Bridge パターン
 * 
 * e.g. compile
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_7_bridge.cpp -o ../bin/main
*/
#include <iostream>
#include <memory>

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
 * 電気自動車クラス
*/
class ElectricCar {
private:
    std::unique_ptr<Engine> engine_;
public:
    explicit ElectricCar(/* mybe some engine arguments */):engine_{std::make_unique<ElectricEngine>(/* engine arguments */)} {}
    void drive() {
        puts("--- ElectricCar.drive()");
    }
};



int main(void) {
    puts("START Bridge パターン ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    puts("=== Bridge パターン END");
    return 0;
}