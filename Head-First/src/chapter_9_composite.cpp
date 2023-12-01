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
    virtual void remove(Component& c) const = 0;
    virtual Component* getChild(unsigned int& i) const = 0;
};

class Leaf final : public virtual Component {
public:
    Leaf() {}
    Leaf(const Leaf& own) {*this = own;}
    ~Leaf() {}

    virtual void operation() const override {
        puts("do anything.");
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
        puts("do anything.");
    }
    virtual void add(Component& c) const override {
        components.push_back(&c);
    }
    virtual void remove(Component& c) const override {
        size_t size = components.size();
        for(size_t i = 0; i < size; i++) {
           Component* comp = components.at(i);
           if(comp == &c) {
                components.erase(components.begin() + i);
           }
        }

    }
    virtual Component* getChild(unsigned int& i) const override {
        size_t size = components.size();
        if( i < size ) {
            return components.at(i);
        } else {
            return nullptr;
        }
    }
};

int main(void) {
    puts("START 9 章の続き Composite パターン ===");
    if(0.01) {
        double pi = 3.14159;
        ptr_lambda_debug<const string&,const double&>("pi is ",pi);
    }
    puts("=== 9 章の続き Composite パターン END");
    return 0;
}