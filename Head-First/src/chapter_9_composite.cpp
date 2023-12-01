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

int main(void) {
    puts("START 9 章の続き Composite パターン ===");
    if(0.01) {
        double pi = 3.14159;
        ptr_lambda_debug<const string&,const double&>("pi is ",pi);
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_Composite());
    }
    puts("=== 9 章の続き Composite パターン END");
    return 0;
}