/**
 * 1 章 型推論
 * 
 * 項目 1 ：テンプレートの型推論を理解する
 * 
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_1_type_inference.cpp -o ../bin/main
*/
#include <iostream>
#include <cassert>
#include <memory>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * ケース 1 ：ParamType が参照もしくはポインタだが、ユニバーサル参照ではない
 * 
 * e.g. 関数テンプレートの場合。
 * 
 * template <class T>
 * void f(ParamType param)
 * 
 * f(expr)                  // call with some expression ... f に式を与え呼び出す。
*/

template <class T>
void f(T& param) {      // const なし
    puts("------ f");
    // param += 1;      // コンパイルエラー、 param is read-only reference
    ptr_lambda_debug<const char*,const T&>("param is ",param);
}

int test_f() {
    puts("--- test_f");
    try {
        int x = 27;             // x  は int
        const int cx = x;       // cx は const int
        const int& rx = x;      // rx は const int としての x の参照

        f(x);                   // T は int, param の型は int&
        f(cx);                  // T は const int, param の型は int&
        f(rx);                  // T は const int, param の型は int&
        /**
         * 正誤表より補足だと：）
         * 上段落はまること削除だと（オイ：）
        */
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

template <class T>
void f2(const T& param) {       // const あり
    puts("------ f2");
    // param += 1;      // コンパイルエラー、 param is read-only reference
    ptr_lambda_debug<const char*,const T&>("param is ",param);
}

int test_f2() {
    puts("--- test_f2");
    try {
        int x = 27;             // x  は int
        const int cx = x;       // cx は const int
        const int& rx = x;      // rx は const int としての x の参照

        f2(x);                  // T は int, param の型は const int&
        f2(cx);                 // T は int, param の型は int&
        f2(rx);                 // T は int, param の型は int&

        /**
         * 先の例と同様に、型推論では rx の参照性は無視されます。
         * 仮に param が参照ではなくポインタだったとしても（または const を指すポインタ）、基本的には
         * 同様に推論されます。
        */

        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

template <class T>
void fp(T* param) {
    puts("------ fp");
    ptr_lambda_debug<const char*,const T&>("param is ",*param);
}

int test_fp() {
    puts("--- fp");
    try {
        int x = 27;             // x  は int
        const int* px = &x;     // 先の例と変わらず、px は const int としての x を指す

        fp(&x);                 // T は int, param の型は int*
        fp(px);                 // T は const int, param の型は const int*
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

/**
 * C++ の型推論は、参照やポインタの仮引数に対してごく自然に動作するため、改めて言葉で
 * 説明されても退屈で居眠りをしてしまったかもしれません。すべて自明で分かりきったことです！
 * 型推論システムは期待通りに動作します。
*/

/**
 * ケース 2 ：ParamType がユニバーサル参照である
 * 
 * ユニバーサル参照を仮引数にとるテンプレートの場合はずっと分かりにくくなります。
 * この種の仮引数は右辺値参照のように宣言されますが（T を仮引数にとる関数テンプレートでは、ユニバーサル参照に宣言する型が T&&）、
 * 左辺値の実引数が渡された場合の動作が変化します。
 * 
 * e.g. 関数テンプレートの場合。
 * 
 * template <class T>
 * void f(ParamType param)
 * 
 * f(expr)                  // call with some expression ... f に式を与え呼び出す。
 * 
 * - expr が左辺値ならば、T も ParamType も左辺値参照と推論される。これは 2 つの意味で特殊である。まず、テンプレートの型推論で、
 *   T を参照として推論するのはこの場合だけである。もう 1 つは、ParamType の宣言には右辺値参照という形態をとりながら、推論される
 *   型は左辺値参照となる点である。
 * 
 * - expr が右辺値の場合は、「通常の」規則が適用される（ケース 1）。
*/

template <class T>
void f_uref(T&& param) {        // param is universal reference.
    puts("------ f_uref");
    ptr_lambda_debug<const char*,const T&>("param is ", param);
}

int test_f_uref() {
    puts("--- test_f_uref");
    try {
        int x = 33;
        const int cx = x;
        const int& rx = x;      // rx は const int としての x の参照
        /**
         * 上記は先の例と同じ。
        */

        f_uref(x);      // x は左辺値、よって T は int&
        f_uref(cx);     // cx は左辺値、よって T は const int&, param の型も const int&
        f_uref(rx);     // rx は左辺値、よって T は const int&, patam の型も const int&

        // 数値リテラルは、y = 3 のように右辺値として扱われるもの。
        f_uref(33);     // 33 は数値リテラル、33 は右辺値、よって T は int, ゆえに param の型は int&&
        /**
         * 上例がなぜこのように動作するかについては、項目 24 で述べます。重要なのは、ユニバーサル参照の仮引数に対する型推論規則は、
         * 左辺値参照や右辺値参照の仮引数の場合とは異なるという点です。特に、型推論が左辺値実引数と右辺値実引数を区別する点は重要であり、
         * ユニバーサル参照に限った特殊な規則です。
        */

        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

/**
 * ユニバーサル参照に関する理解とヒマ潰しのために。
 * 
 * ここから少し私の興味を掘り下げる。
 * Strategy パターンを利用して、オブジェクト同士の add（足し算） について考えてみる。
 * operator+() で解決できるかもしれないがそれはそれ、これはこれだ。
*/

/**
 * 座標クラス
*/
class Point final {
private:
    double x=0.0, y=0.0;
    explicit Point():x{0.0}, y{0.0} {}
public:
    // explicit Point(double _x, double _y): x(_x), y(_y) {}
    explicit Point(const double& _x, const double& _y): x{_x}, y{_y} {}
    Point(const Point& own) {*this = own;}
    ~Point() {}
    // ...
    /**
     * operator はまだ理解が不十分だった。
     * 次の記述により意図した動作にならなかった。
    */
    // Point& operator=(const Point&) {
    //     return *this;
    // }
    // Point(Point&&)            = default;
    // Point& operator=(Point&&) = default;
    // ...
    std::unique_ptr<Point> clone() {
        return std::make_unique<Point>(*this);
    }
    double getX() const {
        return x;
    }
    double getY() const {
        return y;
    }
};

/**
 * 追加（加算）クラステンプレート
*/
template <class T>
class AddStrategy {
public:
    virtual ~AddStrategy() = default;
    virtual T add(const T&& rhs) = 0;     // rhs ... right-hand side.
};

/**
 * 座標加算ストラテジクラス
*/
class PointAddStrategy final : public AddStrategy<Point> {
private:
    Point point{0.0, 0.0};
public:
    PointAddStrategy(const Point& _point): point(_point) 
    {}
    PointAddStrategy(const PointAddStrategy& own) {*this = own;}
    ~PointAddStrategy() {}
    // ...
    Point add(const Point&& rhs) override {
        Point result{point.getX()+rhs.getX(), point.getY() + rhs.getY()};
        return result;
    }
};

int test_PointAddStrategy() {
    puts("--- test_PointAddStrategy");
    try {
        std::unique_ptr<AddStrategy<Point>> addStrategy = std::make_unique<PointAddStrategy>(PointAddStrategy{Point{30.0, 60.0}});
        // x: 30.0 + 3.0 = 33.0
        // y: 60.0 + 3.0 = 63.0
        Point result = addStrategy->add(Point{3.0, 3.0});
        // Point p1{30.0, 60.0};
        // PointAddStrategy strategy{p1};
        // Point result = strategy.add(Point{3.0, 3.0});
        ptr_lambda_debug<const char*, const double&>("x is ",result.getX());
        ptr_lambda_debug<const char*, const double&>("y is ",result.getY());
        assert(result.getX() == 33.0);
        assert(result.getY() == 63.0);
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

template <class Number>
class NumberAddStrategy final : public AddStrategy<Number> {
private:
    Number num;
public:
    NumberAddStrategy(const Number& _num): num(_num) {}
    // ...
    Number add(const Number&& rhs) override {
        return num+rhs;
    }
};

int test_NumberAddStrategy() {
    puts("--- test_NumberAddStrategy");
    try {
        NumberAddStrategy<int> int_strategy{12};
        int int_result = int_strategy.add(9);
        ptr_lambda_debug<const char*,const int&>("int_result is ",int_result);
        assert(int_result == 21);

        NumberAddStrategy<double> double_strategy{3.0};
        double double_result = double_strategy.add(6.0);
        ptr_lambda_debug<const char*, const double&>("double_result is ",double_result);
        assert(double_result == 9.0);

        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

/**
 * test_PointAddStrategy() と test_NumberAddStrategy() の結果内容を見ると
 * 関数定義の仮引数の型はユニバーサル参照にした方が 数値リテラルを扱えるので良いと言えるのか。
*/

int main(void) {
    puts("START 1 章 型推論 ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const double>("pi is ",pi);
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_f());
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_f2());
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_fp());
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_f_uref());
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_PointAddStrategy());
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_NumberAddStrategy());
    }
    puts("=== 1 章 型推論 END");
    return 0;
}