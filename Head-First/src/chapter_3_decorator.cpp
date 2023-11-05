/**
 * 3 章 Decorator
 * 
 * g++ -O3 -std=c++20 -pedantic-errors -Wall -Werror chapter_3_decorator.cpp -o ../bin/main
 * g++ -O3 -std=c++20 -pedantic-errors -Wall -Werror -D NDEBUG chapter_3_decorator.cpp -o ../bin/main
 * 
 * オブジェクトの装飾
 * 
 * 開放・閉鎖原則、既存コードの修正は行わず（閉鎖）に拡張性（開放）を持つ設計のこと。
 * 
 * Decorator パターンはオブジェクトに追加の責務を動的に付与する。デコレータは、サブクラス化の
 * 代替となる、柔軟な機能拡張手段を備えている。
 * 
 * コーヒーショップのレジシステムを想定したサンプルになる。
*/
#include <iostream>
#include <cassert>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * 飲み物の基底クラス
 * 飲み物の抽象クラス
*/
class Beverage {
protected:
    string description = "不明な飲み物";
public:
    virtual ~Beverage() {}
    string getDescription() noexcept {
        return description;
    }
    virtual double cost() const = 0;
};

/**
 * 調味料の基底クラス（Decorator）
 * 調味料の抽象クラス
 * Beverage のサブクラス
*/
class CondimentDecorator : public virtual Beverage {
protected:
    Beverage* beverage;
public:
    ~CondimentDecorator() {}
    virtual string getDescription() const = 0; 
};

/**
 * 飲み物、エスプレッソ クラス
*/
class Espresso final : public virtual Beverage {
public:
    Espresso() {
        description = "エスプレッソ";
    }
    Espresso(const Espresso& own) { *this = own; }
    ~Espresso() {}
    virtual double cost() const override {
        return 1.99;
    }
};

/**
 * 飲み物、ブレンド クラス
*/
class HouseBlend final : public virtual Beverage {
public:
    HouseBlend() {
        description = "ハウスブレンドコーヒー";
    }
    HouseBlend(const HouseBlend& own) {*this = own;}
    ~HouseBlend() {}
    virtual double cost() const override {
        return 0.89;
    }
};

/**
 * モカ
 * 調味料の具象クラス
*/
class Mocha final : public virtual CondimentDecorator {
public:
    Mocha(Beverage& b) {
        beverage = &b;
    }
    Mocha(const Mocha& own) {*this = own;}
    ~Mocha() {}

    virtual string getDescription() const override {
        string result = beverage->getDescription() + "、モカ";
        return result;
    }
    virtual double cost() const override {
        return beverage->cost() + 0.20;
    }
};

/**
 * 豆乳
 * 調味料の具象クラス
*/
class Soy final : public virtual CondimentDecorator {
public:
    Soy(Beverage& b) {
        beverage = &b;
    }
    Soy(const Soy& own) {*this = own;}
    ~Soy() {}

    virtual string getDescription() const override {
        string result = beverage->getDescription() + "、豆乳";
        return result;
    }
    virtual double cost() const override {
        return beverage->cost() + 0.60;
    }
};

/**
 * ホイップ
 * 調味料の具象クラス
*/
class Whip final : public virtual CondimentDecorator {
public:
    Whip(Beverage& b) {
        beverage = &b;
    }
    Whip(const Whip& own) {*this = own;}
    ~Whip() {}

    virtual string getDescription() const override {
        string result = beverage->getDescription() + "、ホイップ";
        return result;
    }
    virtual double cost() const override {
        return beverage->cost() + 1.20;
    }
};

int test_Beverages() {
    try {
        puts("--- test_Beverage");
        Espresso espresso;
        Soy soy(espresso);
        ptr_lambda_debug<const string&, const string&>("soy(espresso) ... description is ",soy.getDescription());
        ptr_lambda_debug<const string&, const double&>("soy(espresso) ... cost is ",soy.cost());
        assert(soy.cost() == 2.59);    // 1.99 + 0.60
        return 0;
    } catch(exception& e) {
        cout << e.what() << endl;
        return -1;
    }
}

int main(void) {
    puts("START 3 章 Decorator =========");
    if(1) {
        double pi = 3.14159;
        ptr_lambda_debug<const char*, const double&>("pi is ",pi);
    }
    if(1.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_Beverages());
    }
    puts("========= END 3 章 Decorator ");
    return 0;
}