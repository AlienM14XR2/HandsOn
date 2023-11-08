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
};

class PizzaStore {
protected:
    Pizza* pizza = nullptr;
    virtual Pizza& createPizza(const string& type) const = 0;
public:
    PizzaStore() {}
    PizzaStore(const PizzaStore& own) {*this = own;}
    virtual ~PizzaStore() {}
    Pizza& orderPizza(const string& type) {
        *pizza = createPizza(type);
        pizza->prepare();
        pizza->bake();
        pizza->cut();
        pizza->box();
        return *pizza;
    }    
};

int main() {
    puts("START 4 章 Factory パターン =========");
    if(1) {
        double pi = 3.14159;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    puts("========= 4 章 Factory パターン END");
    return 0;
}