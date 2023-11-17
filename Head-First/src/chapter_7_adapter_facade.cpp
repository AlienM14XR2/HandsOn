/**
 * 7 章 Adapter パターンと Facade パターン
 * 
 * 適合可能にする。
 * 
 * Strategy パターン（1 章）で登場した Duck インタフェースとカモクラスの簡易版を利用する。
 * 
 * e.g. compile
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_7_adapter_facade.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template<class M,class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * Duck インタフェース
 * 既存のもの
*/
class Duck {
public:
    virtual void quack() const = 0;
    virtual void fly() const = 0; 
};

/**
 * カモクラス、Duck インタフェースの具象クラス
 * 既存のもの
*/
class MallardDuck final : public virtual Duck {
public:
    virtual void quack() const override {
        puts("ガーガー");
    }
    virtual void fly() const override {
        puts("飛んでいます");
    }
};

/**
 * Turkey インタフェース
 * 新しく追加されたもの
*/

int main(void) {
    puts("START 7 章 Adapter パターンと Facade パターン ===");
    if('d') {
        double pi = 3.14159;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    puts("=== 7 章 Adapter パターンと Facade パターン END");
    return 0;
}