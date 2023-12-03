/**
 * 9 章の続き Composite パターン
 * 
 * Composite パターンの定義
 * 
 * オブジェクトをツリー構造に構成して部分 ー 全体階層を表現できる。
 * Composite パターンを使うと、クライアントは個々のオブジェクトと
 * オブジェクトのコンポジションを統一的に扱うことができる。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_9_composite.cpp -o ../bin/main
 * 
*/
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * 一般的なComposite パターン
*/

class Component {
public:
    ~Component() {}
    virtual void operation() const = 0;
    virtual void add(Component& c) const = 0;
    virtual void remove(Component* c) const = 0;
    virtual Component* getChild(const unsigned int& i) const = 0;
};

class Leaf final : public virtual Component {
public:
    Leaf() {}
    Leaf(const Leaf& own) {*this = own;}
    ~Leaf() {}

    virtual void operation() const override {
        puts("do anything.");
    }
    virtual void add(Component& c) const override {
        throw runtime_error("Not Supported.");
    }
    virtual void remove(Component* c) const override {
        throw runtime_error("Not Supported.");
    }
    virtual Component* getChild(const unsigned int& i) const override {
        throw runtime_error("Not Supported.");
    } 
};

class Composite final : public virtual Component {
private:
    mutable vector<Component*> components;
public:
    Composite() {}
    Composite(const Composite& own) {*this = own;}
    ~Composite() {}

    virtual void operation() const override {
        puts("do anything (composite action).");
        size_t size = components.size();
        for(size_t i=0; i < size; i++) {
            Component* comp = components.at(i);
            comp->operation();
        }
    }
    virtual void add(Component& c) const override {
        components.push_back(&c);
    }
    virtual void remove(Component* c) const override {
        size_t size = components.size();
        printf("size is %zu\n", size);
        printf("c addr is %p\n", (void*)c);
        for(size_t i = 0; i < size; i++) {
           Component* comp = components.at(i);
            printf("comp addr is %p\n", (void*)comp);
           if(comp == c) {
                printf("in --------------\n");
                components.erase(components.begin() + i);
                break;
           }
        }
    }
    virtual Component* getChild(const unsigned int& i) const override {
        size_t size = components.size();
        if( i < size ) {
            return components.at(i);
        } else {
            return nullptr;
        }
    }
};

int test_Composite() {
    puts("--- test_Composite");
    try {
        Leaf l1;
        Leaf l2;
        Leaf l3;

        Composite composite;
        composite.operation();
        puts("--- add");
        composite.add(l1);
        composite.add(l2);
        composite.add(l3);
        composite.operation();
        Component* component = composite.getChild(0);
        // component->operation();
        puts("--- remove");
        composite.remove(component);
        composite.remove(composite.getChild(0));
        composite.remove(composite.getChild(0));
        composite.operation();
        assert(composite.getChild(0) == nullptr);
        
        return 0;
    } catch(exception& e) {
        cout << e.what() << endl;
        return -1;
    }
}

/**
 * MenuComponent を実装する
 * サンプル実装の続き。
*/

class MenuComponent {
public:
    virtual ~MenuComponent() {}
    //
    // コンポジット メソッド
    //
    virtual void add(MenuComponent& mc) {
        throw runtime_error("ERROR: Not Supported.");
    }
    virtual void remove(MenuComponent* mc) {
        throw runtime_error("ERROR: Not Supported.");
    }
    virtual MenuComponent* getChild(const unsigned int& i) {
        throw runtime_error("ERROR: Not Supported.");
    }
    //
    // 操作 メソッド
    //
    virtual string getName() {
        throw runtime_error("ERROR: Not Supported.");
    }
    virtual string getDescription() {        
        throw runtime_error("ERROR: Not Supported.");
    }
    virtual double getPrice() {
        throw runtime_error("ERROR: Not Supported.");
    }
    virtual bool isVegetarian() {
        return false;
    }
    virtual void print() {
        throw runtime_error("ERROR: Not Supported.");
    }
};

class MenuItem final : public virtual MenuComponent {
private:
    string name;
    string description;
    bool vegetarian;
    double price;
    MenuItem():name{""},description{""},vegetarian{false},price{0.00}{}
public:
    MenuItem(const string& n, const string& desc, const bool& vege, const double p) {
        name = n;
        description = desc;
        vegetarian = vege;
        price = p;
    }
    MenuItem(const MenuItem& own) {*this = own;}
    ~MenuItem() {}

    string getName() noexcept {
        return name;
    }
    string getDescription() noexcept {
        return description;
    }
    bool isVegetarian() noexcept {
        return vegetarian;
    }
    double getPrice() noexcept {
        return price;
    }

    virtual void print() noexcept {
        printf("%s\n",getName().c_str());
        if(isVegetarian()) {
            printf("(v)\n");
        }
        printf("%3.2lf\n",getPrice());
        printf("%s\n",getDescription().c_str());
        puts("---");
    }

};

int test_MenuItem() {
    puts("--- test_MenuItem");
    try {
        MenuItem beefSteak("Beef Steak.","USA Beef 100% 300g",false,25.12);
        beefSteak.print();
        puts("---");
        MenuItem salad("Green Salad.","Organic.",true,10.75);
        salad.print();
        return 0;
    } catch(exception& e) {
        cout << e.what() << endl;
        return -1;
    }
}

/**
 * コンポジットメニュを実装する
 * TODO list で MenuComponent のポインタをメンバ変数に持つ。
*/
class Menu final : public virtual MenuComponent {
private:
    mutable vector<MenuComponent*> menuComponents;
    string name;
    string description;
    Menu():name{""},description{""} {}
public:
    Menu(const string& n, const string& desc) {
        name = n;
        description = desc;
    }
    Menu(const Menu& own) {*this = own;}
    ~Menu() {}

    string getName() noexcept {
        return name;
    }
    string getDescription() noexcept {
        return description;
    }

    virtual void print() noexcept {
        printf("%s\n",getName().c_str());
        printf("%s\n",getDescription().c_str());
        puts("---------");
        size_t size = menuComponents.size();
        for(size_t i=0; i < size; i++) {
            MenuComponent* comp = menuComponents.at(i);
            comp->print();
        }
    }
    virtual void add(MenuComponent& mc) {
        menuComponents.push_back(&mc);
    }
    virtual void remove(MenuComponent* mc) {
        size_t size = menuComponents.size();
        for(size_t i = 0; i < size; i++) {
           MenuComponent* comp = menuComponents.at(i);
           if(comp == mc) {
                printf("HIT: comp == mc --------------\n");
                menuComponents.erase(menuComponents.begin() + i);
                break;
           }
        }
    }
    virtual MenuComponent* getChild(const unsigned int& i) {
        size_t size = menuComponents.size();
        if(i < size) {
            return menuComponents.at(i);
        } else {
            return nullptr;
        }
    }
};

int test_Menu() {
    puts("--- test_Menu");
    try {
        Menu pancakeHouseMenu("パンケーキハウスメニュ", "朝食");
        Menu dinerMenu("食堂メニュ", "昼食");
        Menu cafeMenu("カフェメニュ", "夕食");
        Menu dessertMenu("デザートメニュ", "デザート：）");

        Menu allMenus("すべてのメニュ", "すべてを統合したメニュ");
        allMenus.add(pancakeHouseMenu);
        allMenus.add(dinerMenu);
        allMenus.add(cafeMenu);

        // 参照とポインタはやっぱり強力だった、ここより前のコーディングと次に行っていることをよく考えてみてください。
        MenuItem beef("ステーキ","ビーフ",false,25.12);
        MenuItem tofu("ステーキ（V）","豆腐",true,12.00);
        MenuItem iceCream("アイスクリーム","バニラ",false,5.25);
        dessertMenu.add(iceCream);
        dinerMenu.add(beef);
        dinerMenu.add(tofu);
        dinerMenu.add(dessertMenu);

        allMenus.print();
        puts("--- remove iceCream from dessertMenu.");
        dessertMenu.remove(&iceCream);
        allMenus.print();

        return 0;
    } catch(exception& e) {
        cout << e.what() << endl;
        return -1;
    }
}

int main(void) {
    puts("START 9 章の続き Composite パターン ===");
    if(0.01) {
        double pi = 3.14159;
        ptr_lambda_debug<const string&,const double&>("pi is ",pi);
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_Composite());
    }
    if(1.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ...", test_MenuItem());
    }
    if(1.02) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ...", test_Menu());
    }
    puts("=== 9 章の続き Composite パターン END");
    return 0;
}