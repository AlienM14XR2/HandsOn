/**
 * 4 章 Factory パターン
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_4_factory.cpp -o ../bin/main
 * 
 * OO の利点を活用した構築
 * 
 * サンプルは様々な地域に出店するピザ屋のシステム。
*/
#include <iostream>

using namespace std;

template<class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

class Pizza {
public:
    virtual ~Pizza() {}
    virtual void prepare() const = 0;
    virtual void bake() const = 0;
    virtual void cut() const = 0;
    virtual void box() const = 0;
    virtual void display() const = 0;
};

class NYStyleCheesePizza final : public virtual Pizza {
public:
    NYStyleCheesePizza() {}
    NYStyleCheesePizza(const NYStyleCheesePizza& own) {*this = own;}
    ~NYStyleCheesePizza() {}
    virtual void prepare() const override {
        ptr_lambda_debug<const char*,const int&>("prepare ... NYStyleCheesePizza",0);
    }
    virtual void bake() const override {
        ptr_lambda_debug<const char*,const int&>("bake ... NYStyleCheesePizza",0);
    }
    virtual void cut() const override {
        ptr_lambda_debug<const char*,const int&>("cut ... NYStyleCheesePizza",0);
    }
    virtual void box() const override {
        ptr_lambda_debug<const char*,const int&>("box ... NYStyleCheesePizza",0);
    }
    virtual void display() const override {
        puts("NY スタイル チーズ ピザ");
    }
};
class NYStyleVeggiePizza final : public virtual Pizza {
public:
    NYStyleVeggiePizza() {}
    NYStyleVeggiePizza(const NYStyleCheesePizza& own) {*this = own;}
    ~NYStyleVeggiePizza() {}
    virtual void prepare() const override {
        ptr_lambda_debug<const char*,const int&>("prepare ... NYStyleVeggiePizza",0);
    }
    virtual void bake() const override {
        ptr_lambda_debug<const char*,const int&>("bake ... NYStyleVeggiePizza",0);
    }
    virtual void cut() const override {
        ptr_lambda_debug<const char*,const int&>("cut ... NYStyleVeggiePizza",0);
    }
    virtual void box() const override {
        ptr_lambda_debug<const char*,const int&>("box ... NYStyleVeggiePizza",0);
    }
    virtual void display() const override {
        puts("NY スタイル 野菜 ピザ");
    }
};

class PizzaStore {
private:
protected:
    PizzaStore() {}
    mutable Pizza* pizza = nullptr;
//    virtual void createPizza(const string& type) const = 0;
public:
    PizzaStore(Pizza* const p) {
        pizza = p;
    }
    PizzaStore(const PizzaStore& own) {*this = own;}
    virtual ~PizzaStore() {}

    Pizza& orderPizza() {
        pizza->prepare();
        pizza->bake();
        pizza->cut();
        pizza->box();
        return *pizza;
    }    
};

class NYPizzaStore final : public virtual PizzaStore {
private:
    NYPizzaStore() {}
protected:
public:
    NYPizzaStore(Pizza* const p) {
        pizza = p;
    }
    NYPizzaStore(const NYPizzaStore& own) {*this = own;}
    ~NYPizzaStore() {}
};

void createPizza(const string& type, Pizza* pizza) {
    if(type == "cheese") {
//    printf("BBBBBBBBBBBBBBBB\n");
        NYStyleCheesePizza cheese;      // これはローカル変数、結局関数を抜けたら破壊される：）つまり、システム起動時に存在している必要がある。
//    printf("CCCCCCCCCCCCCCCC\n");
        pizza = move(static_cast<Pizza*>(&cheese));
   printf("DDDDDDDDDDDDDDDD\n");
    } else if(type == "veggie") {
        NYStyleVeggiePizza veggie;
        pizza = move(static_cast<Pizza*>(&veggie));
    }
}

NYStyleCheesePizza cheese;
NYStyleVeggiePizza veggie;

void createPizzaV2(const string& type, Pizza* pizza) {
    if(type == "cheese") {
        pizza = move(static_cast<Pizza*>(&cheese));
   printf("DDDDDDDDDDDDDDDD\n");
    } else if(type == "veggie") {
        pizza = move(static_cast<Pizza*>(&veggie));
    }
}

Pizza* createPizzaV3(const string& type) {  // ここまで来てやっと理想にたどり着いた、現状の私の理解ではこれが理想的なファクトリ、サンプルのようにすると開放が疎かになる：）
    if(type == "cheese") {
        return new NYStyleCheesePizza();
    } else if(type == "veggie") {
        return new NYStyleVeggiePizza();
    }
    return NULL;
}

int test_NYPizzaStore(const string type) {
    puts("--- test_NYPizzaStore");
    try {
        Pizza* pizza = nullptr;
        pizza = createPizzaV3(type);
        // createPizzaV2(type, pizza);
    //    createPizza(type, pizza);
        // 次の２行は createPizza の一部を抜粋したもの。
        // NYStyleCheesePizza cheese;
        // pizza = static_cast<Pizza*>(&cheese);

        NYPizzaStore store(pizza);
        store.orderPizza();
        pizza->display();
        delete pizza;
        return 0;
    } catch(exception& e) {
        cout << e.what() << endl;
        return -1;
    }
}

int main() {
    puts("START 4 章 Factory パターン =========");
    if(1) {
        double pi = 3.14159;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    if(1.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_NYPizzaStore("cheese"));
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_NYPizzaStore("veggie"));
    }
    puts("========= 4 章 Factory パターン END");
    return 0;
}