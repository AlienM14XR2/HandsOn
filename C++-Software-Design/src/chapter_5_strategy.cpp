/**
 * 5 章 Strategy パターンと Command パターン
 * 
 * # ガイドライン 19 処理方法を分離するには Strategy パターン
 * Strategy パターン
 * アルゴリズムの集合を定義し、各アルゴリズムをカプセル化して、それらを交換可能にする。
 * Strategy パターンを利用することで、アルゴリズムを、それを利用するクライアントからは
 * 独立して変更できるようになる。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_5_strategy.cpp -o ../bin/main
*/
#include <iostream>
#include <memory>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * 座標
*/
struct Point {
    double x;
    double y;
    Point() :x{0.0},y{0.0} {}
    Point(double tx, double ty):x(tx),y(ty) {}
    Point(const Point& own) {*this = own;}
    ~Point() {}
};

/**
 * 図形基底クラス
*/
class Shape {
public:
    virtual ~Shape() = default;
    virtual void draw(/* some arguments */) const = 0;
};

/**
 * 描画用 Strategy パターン基底クラス
 * 
 * これは、悪い例です。私の知っているものではない。
 * 振る舞い毎にひとクラス、且つ特定のクラスに特化した処理であるはず。
 * 次のクラスは、２つクラス、Circle と Square を 引数に持つ draw() がある点で
 * Strategy パターンとは言えない。
*/
// class DrawStrategy {
// public:
//     virtual ~DrawStrategy() = default;
//     virtual void draw(const Circle& circle) const = 0;
//     virtual void draw(const Square& square) const = 0;
// };

/**
 * DrawStrategy の実装、考え方はサンプルと同じだった。
*/

template<class T>
class DrawStrategy {
public:
    virtual ~DrawStrategy() = default;
    virtual void draw(const T&) const = 0;
};

/**
 * 円クラス
*/
class Circle final : public virtual Shape {
private:
    double radius;
    Point point;
    DrawStrategy<Circle>* drawer;       // 不思議、コンパイルレベルの問題なのか理解に苦しむが、メンバ引数の表記順番を入れ替えるとコンパイルエラーになる（ただしくはWarning）。
    Circle():radius{0.00},point{},drawer{nullptr}{}
public:
    explicit Circle(const double& r, const Point& p, DrawStrategy<Circle>& d):radius(r),point(p),drawer(&d) {     // 書籍では move セマンティクスを利用している。
        // TODO Checking that given radius is valid.
    }
    Circle(const Circle& own) {*this = own;}
    ~Circle() {}

    double getRadius() const {
        return radius;
    }
    Point getPoint() const {
        return point;
    }
    void draw(/* some arguments */) const override {
        puts("step 1 draw circle.");
        drawer->draw(*this/* some arguments */);
    }
};

class CircleDrawStrategy final : public virtual DrawStrategy<Circle> {
public:
    CircleDrawStrategy() {}
    CircleDrawStrategy(const CircleDrawStrategy& own) {*this = own;}
    ~CircleDrawStrategy() {}

    void draw(const Circle& circle) const override {
        puts("step 2 draw circle.");
        printf("radius is %lf\n",circle.getRadius());
        printf("x is %lf\t y is %lf\n",circle.getPoint().x, circle.getPoint().y);
    }
};

int test_Circle() {
    puts("--- test_Circle");
    try {
        CircleDrawStrategy drawStrategy;
        Circle circle(30.0,Point{0.0,10.0},drawStrategy);
        circle.draw();
        return EXIT_SUCCESS;
    } catch(...) {
        return EXIT_FAILURE;
    }
}

/**
 * ここまでは、書籍ではなく、自分が考える Strategy パターンを実装、テスト
 * してみた。ここから先は、書籍を読み進めてサンプリングしてみる。
*/

class CircleStrategy {
public:
    virtual ~CircleStrategy() = default;
    virtual void draw(const Circle& /* some arguments */) const = 0;
};

/**
 * サンプルでは上記を Circle クラスのメンバ変数としてコンポジションして利用している。
 * 次がサンプルの本題だと思う。
*/

class OpenGLCircleStrategy final : public virtual CircleStrategy {
public:
    explicit OpenGLCircleStrategy(/* Drawing related arguments */) {}
    OpenGLCircleStrategy(const OpenGLCircleStrategy& own) {*this = own;}
    ~OpenGLCircleStrategy() {}

    void draw(const Circle& circle) const override {
        // use OpenGL.
        puts("draw ... use OpenGL.");
    }
};

class OpenGLCircleStrategyV2 final : public virtual DrawStrategy<Circle> {
public:
    explicit OpenGLCircleStrategyV2(/* Drawing related arguments */) {}
    OpenGLCircleStrategyV2(const OpenGLCircleStrategyV2& own) {*this = own;}
    ~OpenGLCircleStrategyV2() {}
    
    void draw(const Circle& circle) const override {
        // use OpenGL.
        puts("draw ... use OpenGL.");
    }

};



int main() {
    puts("START 5 章 Strategy パターンと Command パターン ===");
    if(0.01) {
        double pi = 3.14159;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_Circle());
    }
    puts("=== 5 章 Strategy パターンと Command パターン END");
    return EXIT_SUCCESS;
}