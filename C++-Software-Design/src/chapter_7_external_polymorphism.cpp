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
#include <stdexcept>    // これはどこで使ってるのかな
#include <vector>

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
        draw(obj);  // この記述が許される意味が今の私には理解できない、つまり operator() に関する基本的な知識がなさ過ぎる。
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

class OepnGLDrawStrategy final {
private:
    /* Drawing related data members, e.g. colors, textures, ... */
    string color = "none";
public:
    explicit OepnGLDrawStrategy(/* Drawing related arguments */) 
    {}
    explicit OepnGLDrawStrategy(const string& _color) : color{_color}
    {}

    // Circle 用、関数コール演算子
    void operator()(const Circle& circle) const {
        puts("--- circle draw");
        ptr_lambda_debug<const char*,const double&>("radius is ",circle.getRadius());
        ptr_lambda_debug<const char*,const string&>("color is ",color);
    }
    // Square 用、関数コール演算子
    void operator()(const Square& square) const {
        puts("--- square draw");
        ptr_lambda_debug<const char*,const double&>("side is ",square.getSide());
        ptr_lambda_debug<const char*,const string&>("color is ",color);
    }
};

/**
 * ShapeModel クラステンプレート内では、使えるならばどんなものでも実装に使用できる、完全な柔軟性が得られます
 * （ Strategy パターンや std::function に限った話ではないということ） 。
 * ただし、ShapeModel をインスタンス化するすべての型は、コンパイルできるよう、ShapeModel::draw() 関数が求める要求
 * を満たす必要がある。
 * 
 * 現状の実行時 Strategy 実装をコンパイル時実装へ容易に切換えることもできる（即ちポリシーベースの設計。「ガイドライン 19」を参照）。
 * 
 * 守らなければならない決まり事は 1 つしかありません。ShapeModel クラステンプレートが定義するコール規約である、Circle 用、Square 用、
 * それぞれの関数コール演算子を実装することです。
 * 
 * 上例のアプローチには素晴らしい長所がある。
 * - 関心の分離と、図形クラスから多態動作を切り出した結果、グラフィックライブラリなどへの
 *   依存関係をすべて排除できた。関係性が非常に希薄になり、美しいまでに SRP に従っている。
 * - 図形クラスが簡潔になり、多態ではなくなった。
 * - 新規図形を容易に追加できる、既存コードに干渉的に Shape 基底クラスから派生する必要がなく、
 *   Adapter パターンを用いる必要もないため、サードパーティのものでも追加できる。そんため、OCP を完全に遵守している。
 * - 継承関係一般に見られるコードの重複を大幅に削減し、1 ヶ所だけに実装した。DRY 原則にも非常によく従っている。
 * - ShapeConcept クラスと ShapeModel クラステンプレートは一体であり、ともに抽象化を成すため、DIP を遵守するのも大幅に
 *   容易になった。
 * - クラステンプレートという形のため、間接参照の多くを排除でき、性能も向上させられる。
 * - どんな型に対して、既存コードに干渉せず、多態動作を追加できる（著者が一番推している点）。
*/

int test_External_Polymorphism() {
    puts("--- test_External_Polymorphism");
    try {
        using Shapes = std::vector<std::unique_ptr<ShapeConcept>>;
        using CircleModel = ShapeModel<Circle,OepnGLDrawStrategy>;
        using SquareModel = ShapeModel<Square,OepnGLDrawStrategy>;

        Shapes shapes{};
        ptr_lambda_debug<const char*,const size_t&>("shapes.size() is ",shapes.size());

        shapes.emplace_back(
            std::make_unique<CircleModel>(
                Circle{3.3},OepnGLDrawStrategy("red")
            )
        );
        shapes.emplace_back(
            std::make_unique<SquareModel>(
                Square(6.0),OepnGLDrawStrategy("green")
            )
        );
        shapes.emplace_back(
            std::make_unique<CircleModel>(
                Circle{9.9},OepnGLDrawStrategy("blue")
            )
        );

        // Drawing all shapes
        for(const auto& shape: shapes) {
            shape->draw();
        }
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

/**
 * - どんな型に対して、既存コードに干渉せず、多態動作を追加できる（著者が一番推している点）。
 * 
 * これの実証サンプル。
 * 
*/

int main(void) {
    puts("START External Polymorphism パターン ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_External_Polymorphism());
    }
    puts("=== External Polymorphism パターン END");
    return 0;
}