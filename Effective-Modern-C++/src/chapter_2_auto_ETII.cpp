/**
 * 2 章 auto
 * 
 * 項目 6 auto が期待値とは異なる型を推論する場面では ETII を用いる
 * 
 * ETII（Explicitly Typed Initializer Idiom）明示的型付け初期化子イディオム
 * 
 * e.g.)
 * auto highPriority = static_cast<bool>(feature(w)[5]);
 * 
 * 上記に至る経緯。
 * std::vector<bool>::reference はプロクシクラス（Proxy Class）の一例です。プロクシクラスとは、他の型
 * の動作の模倣、拡張を目的としたクラスで、さまざまな場面で使用されます。std::vector<bool>::reference は
 * std::vector<bool> に対する operator[] がヒットの参照を返すように見せかけるものですし、また標準ライブラリ
 * のスマートポインタ型は raw ポインタを用いたリソース管理を実現するものです。プロクシクラスの使用については
 * すでに充分確率されており、実際のデザインパターンの「Proxy」は、もう長いことデザインパターンの殿堂にその名
 * を連ねています。
 * 一般に、「ユーザに意識させない（invisible）」プロクシクラスは、auto とは上手には協調しません。
 * 
 * 重要ポイント
 * - 「ユーザに意識させない」プロクシ型は、auto に、初期化式の「誤った」を推論させる。
 * - 明示的型付け初期化子イディオムを用いれば、auto に目的の型を推論させられる。
 * 
 * e.g.) compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_2_auto_ETII.cpp -o ../bin/main
*/
#include <iostream>
#include <memory>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](const auto message, const auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * 次は個人的興味と演習になる。
 * dynamic_cast について、ダウンキャスト、いいものとはされていないもの。
*/

class Point {
private:
    double x, y;
public:
    Point() = delete;
    Point(const double _x, const double _y): x{_x}, y{_y}
    {}
    Point(const Point& own) {*this = own;}
    ~Point() {}
    // ...
    double& getx() noexcept { return x; }
    double& gety() noexcept { return y; } 
    pair<double,double> getPoint() noexcept {
        return pair<double,double>(x, y);
    }
};

class Shape {
public:
    virtual ~Shape() = default;
    virtual void draw() = 0; 
};

class Circle final : public Shape {
private:
    Point center{0.0,0.0};
public:
    Circle() = delete;
    Circle(const Point& point): center{std::move(point)}
    {}
    Circle(const Circle& own) { *this = own; }
    ~Circle() {}
    // ...
    void draw() override {
        puts("------ circle draw");
        ptr_lambda_debug<const char*,const double&>("x is ", center.getx());
        ptr_lambda_debug<const char*,const double&>("y is ", center.gety());
    }
};

class Square final : public Shape {
private:
    Point top{0.0, 0.0};
    Point bottom{30.0, 30.0};
public:
    Square() = delete;
    Square(const Point& _top, const Point& _bottom): top{std::move(_top)}, bottom{std::move(_bottom)}
    {}
    Square(const Square& own) { *this = own; }
    ~Square() {}
    // ...
    void draw() override {
        puts("------ square draw");
        printf("top(x, y) is (%lf, %lf)\n",top.getx(), top.gety());
        printf("bottom(x, y) is (%lf, %lf)\n",bottom.getx(), bottom.gety());
    }
};

int test_Shape() {
    puts("--- test_Shape");
    Shape* shape = nullptr;
    Circle* circleB = nullptr;
    try {
        std::unique_ptr<Shape> shape1 = std::make_unique<Circle>(Circle{Point{30.0, 69.9}});
        shape1.get()->draw();

        std::unique_ptr<Shape> shape2 = std::make_unique<Square>(Square{Point{0.0, 10.0}, Point{30.0, 33.0}});
        shape2.get()->draw();

        shape = new Circle(Point{3.0, 6.0});
        shape->draw();

        Circle circleA = dynamic_cast<Circle&>(*shape);
        circleA.draw();

        // Square squareA = dynamic_cast<Square&>(*shape);     // コンパイルは通るが実行時にエラーになる。 std::bad_cast
        // puts("---- check point 1");
        // squareA.draw();

        circleB = dynamic_cast<Circle*>(shape);
        circleB->draw();
        if(shape) {
            ptr_lambda_debug<const char*,const Shape*>("shape addr is ", shape);
            ptr_lambda_debug<const char*,const Shape*>("circleB addr is ", circleB);
            ptr_lambda_debug<const char*,const Shape*>("circleA addr is ", &circleA);
        }

        // delete circleB;  // これは上記のデバッグを見れば明らかだけど、shape と全く同じものを指している。なので、これは二重開放につながると。
        delete shape;
        return EXIT_SUCCESS;
    } catch(exception& e) {
        if(shape) { delete shape; }
        cout << "ERROR: " <<  e.what() << endl;
        return EXIT_FAILURE;
    }
}

int main(void) {
    puts("START 項目 6 auto が期待値とは異なる型を推論する場面では ETII を用いる ===");
    if(0.01) {
        auto pi = 3.141592;
        ptr_lambda_debug<const char*,const decltype(pi)&>("pi is ",pi);
    }
    if(2.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_Shape());
    }
    puts("=== 項目 6 auto が期待値とは異なる型を推論する場面では ETII を用いる END");
    return 0;
}