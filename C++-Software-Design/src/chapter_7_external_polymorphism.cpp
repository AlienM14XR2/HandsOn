/**
 * 7 章 Bridge パターン、Prototype パターン、External Polymorphism パターン
 * 
 * ガイドライン 31 ：干渉しない実行時多態を実現するには External Polymorphism パターン
 * 
 * External Polymorphism パターン
 * 目的：継承関係にない、もしくは仮想メソッドを持たない C++ クラスでも、多態性持つものとして扱えるようにする。
 *       関係を持たないクラスでも画一的に扱える。
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

class Circle final {
public:
    explicit Circle(double _radius):radius(_radius)
    {
        /**
         * Checking the given radius is valid.
        */
    }

    double getRadius() const { return radius; }
private:
    double radius;
};

class Square final {
public:
    explicit Square(double _side) : side(_side)
    {
        /**
         * Checking the given side is valid.
        */
    }

    double getSide() const { return side; }
private:
    double side;
};

/**
 * 上例はShape との継承関係を無くしたもの。
 * 基本図形（Circle、Shape）としての本来の形にまで縮退しており、多態性は完全に排除されています。
 * 具象クラスに代わり機能を実装するのが、ShapeConcept 基底クラスと ShapeModel クラステンプレートによる実装です。
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