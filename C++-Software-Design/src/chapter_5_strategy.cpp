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

class Circle;
class Square;
/**
 * 描画用 Strategy パターン基底クラス
*/
class DrawStrategy {
public:
    virtual ~DrawStrategy() = default;
    virtual void draw(const Circle& circle) const = 0;
    virtual void draw(const Square& square) const = 0;
};

/**
 * 円クラス
*/
class Circle final : public virtual Shape {
private:
    double radius;
    Point point;
    DrawStrategy* drawer;       // 不思議、コンパイルレベルの問題なのか理解に苦しむが、メンバ引数の表記順番を入れ替えるとコンパイルエラーになる（ただしくはWarning）。
    Circle():radius{0.00},point{},drawer{nullptr}{}
public:
    explicit Circle(const double& r, const Point& p, DrawStrategy& d):radius(r),point(p),drawer(&d) {     // 書籍では move セマンティクスを利用している。
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
        puts("draw circle.");
        drawer->draw(*this/* some arguments */);
    }
};




int main() {
    puts("START 5 章 Strategy パターンと Command パターン ===");
    if(0.01) {
        double pi = 3.14159;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    puts("=== 5 章 Strategy パターンと Command パターン END");
    return EXIT_SUCCESS;
}