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
#include <cassert>

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
    virtual ~Duck() {
        ptr_lambda_debug<const char*,const int&>("DONE. Duck Destructor.",0);
    }
    virtual void quack() const = 0;
    virtual void fly() const = 0; 
};

/**
 * カモクラス、Duck インタフェースの具象クラス
 * 既存のもの
*/
class MallardDuck final : public virtual Duck {
public:
    MallardDuck() {}
    MallardDuck(const MallardDuck& own) {*this = own;}
    ~MallardDuck() {
        ptr_lambda_debug<const char*,const int&>("DONE. MallardDuck Destructor.",0);
    }
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
class Turkey {
public:
    virtual ~Turkey() {
        ptr_lambda_debug<const char*,const int&>("DONE. Turkey Destructor.",0);
    }
    virtual void gobble() const = 0;    // 七面鳥はガーガーとは鳴かずに、ゴロゴロと鳴きます（gobble）
    virtual void fly() const = 0;       // 七面鳥は飛べますが、短い距離しか飛べません。
};

/**
 * 七面鳥クラス、Turkey インタフェースの具象クラス
 * 新しく追加されたもの
*/
class WildTurkey final : public virtual Turkey {
public:
    WildTurkey() {}
    WildTurkey(const WildTurkey& own) {*this = own;}
    ~WildTurkey() {
        ptr_lambda_debug<const char*,const int&>("DONE. WildTurkey Destructor.",0);
    }
    virtual void gobble() const override {
        puts("ゴロゴロ");
    }
    virtual void fly() const override {
        puts("短い距離を飛んでいます");
    }
};

int test_MallardDuck_WildTurkey() {
    puts("--- test_MallardDuck_WildTurkey");
    try {
        MallardDuck mallardDuck;
        mallardDuck.quack();
        mallardDuck.fly();

        WildTurkey wildTurkey;
        wildTurkey.gobble();
        wildTurkey.fly();
        return 0;
    } catch(exception& e) {
        cout << e.what() << endl;
        return -1;
    }
}

/**
 * ここで、Duck オブジェクトでは不十分なので、代わりに Turkey オブジェクトを使いたいとします。
 * しかし、当然七面鳥は別インタフェースを持っているので、そのまま使うことはできない。
 * そこで、アダプタを書くのです。
*/

int main(void) {
    puts("START 7 章 Adapter パターンと Facade パターン ===");
    if('d') {
        double pi = 3.14159;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_MallardDuck_WildTurkey());
    }
    puts("=== 7 章 Adapter パターンと Facade パターン END");
    return 0;
}