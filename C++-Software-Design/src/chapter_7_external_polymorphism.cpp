/**
 * 7 章 Bridge パターン、Prototype パターン、External Polymorphism パターン
 * 
 * ガイドライン 31 ：干渉しない実行時多態を実現するには External Polymorphism パターン
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_7_external_polymorphism.cpp -o ../bin/main
*/
#include <iostream>
#include <memory>
#include <functional>
#include <utility>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << "\taddress is " << &debug << endl;
};

/**
 * 図形描画と Circle クラスの最新版へ立ち返ってみる。
*/

class Shape {
public:
    virtual ~Shape() = default;
    virtual void draw(/* some arguments */) const = 0;
};

class Circle final : public Shape {
public:
    using DrawStrategy = std::function<void(const Circle&/* ... */)>;

    explicit Circle(double r, DrawStrategy ds):radius(r),drawer(std::move(ds))
    {
        /**
         * Checking the given radius is valid ant that
         * the given 'std::function' instance is not empty.
        */
    }

    void draw() const override {
        drawer(*this);
    }
    double getRadius() const { return radius; }
private:
    double radius;
    DrawStrategy drawer;
};

/**
 * 上例は、Strategy パターンの std::function を用いたもの。
*/

int main(void) {
    puts("START External Polymorphism パターン ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    puts("=== External Polymorphism パターン END");
    return 0;
}