/**
 * 4 章 Visitor パターン の続き
 * 
 * # ガイドライン 18: Acyclic Visitor の性能を把握する
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_4_acyclic_visitor.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template<class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * AbstractVisitor 基底クラスの中身は仮想デストラクタだけで、他に関数などはない。
 * 各 Visitor を識別するための汎用タグとしての役割しかない。
*/
class AbstractVisitor {
public:
    virtual ~AbstractVisitor() = default;
};

/**
 * 図形固有の Visitor 基底クラス。
 * Visitor クラステンプレートは引数に具象図形クラスを受取、その型用の
 * 純粋仮想関数 visit() を宣言する。
*/
template<class T>
class Visitor {
protected:
    ~Visitor() = default;   // これ virtual じゃないのはなぜなのかな？
public:
    virtual void visit(const T&) const = 0;
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
    virtual void accept(const AbstractVisitor&) = 0;
};

/**
 * 円クラス
*/
class Circle final : public virtual Shape {
private:
    double radius;
    Point point{};
    Circle():radius{0.00}{}
public:
    explicit Circle(const double& r, const Point& p):radius(r),point(p) {
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
    void accept(const AbstractVisitor& av) override {
        // ダウンキャストを使うのか... だからタグなのか、AbstractVisitor が。 これなら循環しなくて済むということ。
        // 厳密にはこれはクロスキャストと呼ばれる、ダウンキャストより更に実行速度が悪い。
        // 書籍は、現実的な観点からはこの性能では使えないと言っている。
        if( const auto* cv = dynamic_cast<const Visitor<Circle>*>(&av)) {
            cv->visit(*this);
        }
    }
};

/**
 * 正方形クラス
*/
class Square final : public virtual Shape {
private:
    double side_;
    Point point{};
public:
    explicit Square(const double& s, const Point& p):side_(s),point(p) {
        // Checking that the given radius is valid.
    }
    Square(const Square& own) {*this = own;}
    ~Square() {}

    double getSide() const {return side_;}
    Point getPoint() const {return point;}
    void accept(const AbstractVisitor& av) override {
        if( const auto* cv = dynamic_cast<const Visitor<Square>*>(&av)) {
            cv->visit(*this);
        }
    }
};

/**
 * Draw visitor は Circle と Square の 2 種類の図形に対応するため、3 つの基底クラスから派生する。
*/
class Draw final : public AbstractVisitor
                    , public Visitor<Circle>
                    , public Visitor<Square>
{
public:
    void visit(const Circle& circle) const override {
        // Implementing the logic for drawing a circle.
        puts("draw circle.");
        printf("半径：%lf\n",circle.getRadius());
        printf("x is %lf\t y is %lf\n",circle.getPoint().x,circle.getPoint().y);
    }
    void visit(const Square& square) const override {
        // Implementing the logic for drawing a square.
        puts("draw square.");
        printf("横幅：%lf\n",square.getSide());
        printf("x is %lf\t y is %lf\n",square.getPoint().x,square.getPoint().y);
    }
}; 

int test_Draw_Circle() {
    puts("--- test_Draw_Circle");
    try {
        Circle circle(3.00,Point{0.0, 10.0});
        Draw draw;
        circle.accept(draw);
        return EXIT_SUCCESS;
    } catch(...) {
        return EXIT_FAILURE;
    }
}

int test_Draw_Square() {
    puts("--- test_Draw_Square");
    try {
        Square square(6.0,Point{30.0, 60.0});
        Draw draw;
        square.accept(draw);
        return EXIT_SUCCESS;
    } catch(...) {
        return EXIT_FAILURE;
    }
}

int main(void) {
    puts("START 4 章 Visitor パターン の続き Acyclic Visitor ===");
    if(0.01) {
        double pi = 3.14159;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_Draw_Circle());
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_Draw_Square());
    }
    puts("=== 4 章 Visitor パターン の続き Acyclic Visitor END");
    return EXIT_SUCCESS;
}