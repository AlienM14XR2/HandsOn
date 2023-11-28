/**
 * 8 章 Template Method パターン
 * 
 * アルゴリズムのカプセル化
 * 
 * Template Method パターンの定義
 * Template Method パターンは、メソッド内でアルゴリズムの骨組みを定義し、一部の手順をサブクラスに委ねる。
 * Template Method は、アルゴリズムの構造を変えることなく、アルゴリズムのある手順をサブクラスに再定義させる。
 * 
 * コーヒーショップのコーヒーの作り方、紅茶の作り方をサンプルにする。
 * 
 * e.g. compile
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_8_template_method.cpp -o ../bin/main
*/
#include <iostream>
#include <cassert>

using namespace std;

template<class M, class D>
void (*ptr_lambda_debug)(M, D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * Coffee と Tea を抽象化しよう
*/

/**
 * カフェイン飲料全般に関わるインタフェース。
*/
class CaffeineBeverage {
public:
    virtual ~CaffeineBeverage() {}
    
    const void prepareRecipe() {
        boilWater();
        brew();
        pourInCup();
        addCondiments();
    }
    void boilWater() {
        puts("お湯を沸かす");
    }
    void pourInCup() {
        puts("カップに注ぐ");
    }
    virtual void brew() const = 0;
    virtual void addCondiments() const = 0;
};

class Coffee final : public virtual CaffeineBeverage {
public:
    Coffee() {}
    Coffee(const Coffee& own) {*this = own;}
    ~Coffee() {}
    virtual void brew() const override {
        puts("フィルタを使ってコーヒーをドリップする");
    }
    virtual void addCondiments() const override {
        puts("砂糖とミルクを追加する");
    }
};

class Tea final : public virtual CaffeineBeverage {
public:
    Tea() {}
    Tea(const Tea& own) {*this = own;}
    ~Tea() {}
    virtual void brew() const override {
        puts("紅茶を浸す");
    }
    virtual void addCondiments() const override {
        puts("レモンを追加する");
    }
};

int test_Coffee() {
    puts("--- test_Coffee");
    Coffee coffee;
    coffee.prepareRecipe();
    try {
        return 0;
    } catch(exception& e) {
        cout << e.what() << endl;
        return -1;
    }
}

int test_Tea() {
    puts("--- test_Tea");
    try {
        Tea tea;
        tea.prepareRecipe();
        return 0;
    } catch(exception& e) {
        cout << e.what() << endl;
        return -1;
    }
}

int main(void) {
    puts("START 8 章 Template Method パターン ===");
    if(0.01) {
        double pi = 3.14159;
        ptr_lambda_debug<const char*,const double&>("pi si ",pi);
    }
    if(1.01) {
        int check = -1;
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",check = test_Coffee());
        assert(check == 0);
        check = -1;
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",check = test_Tea());
        assert(check == 0);
    }
    puts("=== 8 章 Template Method パターン END");
    return 0;
}