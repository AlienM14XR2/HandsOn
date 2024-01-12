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
#include <stdexcept>

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

class ShapeConcept {
public:
    virtual ~ShapeConcept() = default;
    virtual void draw(/* some arguments */) const = 0;
};

struct DefaultDrawer {
    template <class T>
    void operator()(const T& obj) {
        draw(obj);
    }
};

template <class ShapeT
        , class DrawStrategy = DefaultDrawer>   // コンパイル時 Strategy 実装にした、テンプレートを利用したので、Default 値を与えることも可能。
class ShapeModel : public ShapeConcept {
public:
    // 実行時 Strategy 実装
//    using DrawStrategy = std::function<void(const Shape&)>;

    explicit ShapeModel(ShapeT _shape, DrawStrategy _drawer=DefaultDrawer{}) : shape{std::move(_shape)}, drawer{std::move(_drawer)}
    {
        /*
        *  Checking that the given 'std::function' is not empty
        */
    }

    void draw() const override {
        drawer(shape);
    }
private:
    ShapeT shape;
    DrawStrategy drawer;
};

/**
 * ShapeModel クラステンプレート内では、使えるならばどんなものでも実装に使用できる、完全な柔軟性が得られます
 * （ Strategy パターンや std::function に限った話ではないということ） 。
 * ただし、ShapeModel をインスタンス化するすべての型は、コンパイルできるよう、ShapeModel::draw() 関数が求める要求
 * を満たす必要がある。
 * 
 * 現状の実行時 Strategy 実装をコンパイル時実装へ容易に切換えることもできる（即ちポリシーベースの設計。「ガイドライン 19」を参照）。
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