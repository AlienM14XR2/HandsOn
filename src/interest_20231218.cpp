/**
 * Interest 2023-12-18
 *  
 * 今、興味あること、気になったことを確認するもの。
 * 
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror interest_20231218.cpp -o ../bin/interest
*/
#include <iostream>
#include <cassert>

using namespace std;

/**
 * pure virtual function の const 修飾と メンバ変数の mutable 修飾の必要性、ありなし。
 * その確認を行いたい。
*/

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

template <class T>
class A {
public:
    virtual ~A() = default;     // この記述について気になった。

    virtual T add(const T&, const T&) const = 0;    // const = 0 と = 0 の違い、これを確認するのが今回の主目的。
};

template <class T>
class B {
public:
    virtual ~B() = default;     // デストラクタのこの書き方も初めて知った。

    virtual T add(const T&, const T&) = 0;          // = 0 ... const 修飾なし。
};

template <class T>
class ConcreteA final : public virtual A<T> {
private:
    T bias;
    mutable T currentResult;    // メンバ変数の修飾に関係してくる、メンバ関数内で変更される場合は mutable にする必要がある。
    ConcreteA() {}
public:
    ConcreteA(const T& b) {
        bias = b;
    }
    ConcreteA(const ConcreteA& own) {*this = own;}
    ~ConcreteA() {}
    T add(const T& l, const T& r) const override {
        currentResult = (l + r) * bias;
        return currentResult;
    }

};

template <class T>
class ConcreteB final : public virtual B<T> {
private:
    T bias;
    T currentResult;    // mutable 修飾をする必要がない。
    ConcreteB() {}
public:
    ConcreteB(const T& b) {
        bias = b;
    }
    ConcreteB(const ConcreteB& own) {*this = own;}
    ~ConcreteB() {}
    T add(const T& l, const T& r) override {
        currentResult = (l + r) * bias;
        return currentResult;
    }
};

int test_ConcreteA() {
    puts("--- test_ConcreteA");
    try {
        ConcreteA<int> a(2);
        int test = 0;
        ptr_lambda_debug<const string&,const int&>("a.add(2,2) is ",test = a.add(2,2));
        assert(test == 8.0);
        return 0;
    } catch(...) {
        return -1;
    }
}

int test_ConcreteB() {
    puts("--- test_ConcreteB");
    try {
        ConcreteB<double> b(3.0);
        double test = 0.0f;
        ptr_lambda_debug<const string&,const double&>("b.add(3,3) is ",test = b.add(3,3));
        assert(test == 18.0);
        return 0;
    } catch(...) {
        return -1;
    }
}

/**
 * C++20 で導入された、concept その意味を再確認する。
 * テンプレートの制約として利用する。
 * 
 * template <仮引数リスト>
 * concept コンセプト名 = 要件リスト;
 * 
 * コンセプトの例として、「テンプレートの仮引数の型 T が draw() という引数を持たないメンバ関数をもっていること」
 * という要件を定義します。
*/

template <class T>
concept Drawable = requires (T& t) {
    t.draw();
};

/**
 * テンプレート仮引数 T をDrawable コンセプトで制約する。
*/
template <class T>
    requires Drawable <T>
void f(T& t) {
    t.draw();
}

class Box {
public:
    void draw() {
        puts("Box オブジェクトを描画");
    }
};

int test_concept_Box() {
    puts("--- test_concept_Box");
    try {
        Box b;
        f(b);       // OK
        int x = 0;
//        f(x);       // NG コンパイルエラー Drawable コンセプトの要件を満たさない。
        printf("x is %d\n",x);
        return 0;
    } catch (...) {
        return -1;
    }
}

int main(void) {
    puts("START Interest 2023-12-18 ===");
    if(1.00) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ", test_ConcreteA());
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ", test_ConcreteB());
    }
    if(1.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_concept_Box());
    }
    puts("=== Interest 2023-12-18 END");
    return 0;
}