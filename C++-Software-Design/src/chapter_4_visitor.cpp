/**
 * 4 章 Visitor パターン
 * 
 * 確かになぜこれが最初に紹介すべきパターンなのか理解できない。
 * 
 * ガイドライン 15: 型または処理の追加に備え設計する。
 * 
 * 図形描画をサンプルに説明するようだ。
 * オブジェクト指向の解は型（円、正方形、その他）の追加という点ではOCP に従っているが、処理の追加という点では
 * 逸脱してしまっている（Serializa のような Shape をバイト列に変換する必要が生じた場合）。
 * 
 * 動的多態性を用いると設計上の判断を迫られる。
 * 型の追加は容易だが、処理の追加は困難になる（手続き型では逆、型の追加が困難で、処理の追加は容易）。
 * 静的多態性は上記の範疇ではない、しかし適切に利用することは当然だと「本」は言っている。
 * 
 * ガイドライン 15 の要約
 * - 異なるプログラミングパラダイムそれぞれの長所と短所を把握する。
 * - プログラミングパラダイムの長所を活かし、短所を回避する。
 * - 動的多態性では型を追加するか、もしくは処理を追加するかの選択があることを理解する。
 * - 主に型を追加するならば、オブジェクト指向の解を優先する。
 * - 主に処理を追加するならば、手続き型／関数型プログラミングの解を優先する。
 * 
 * ガイドライン 16: 処理セットを拡張するには Visitor パターン。
 * 
 * Visitor パターンの目的は処理（Operation、オペレーション、機能、操作、動作、メソッド）を追加可能と
 * することです。
 * 
 * ガイドライン 16 の要約
 * - 継承関係からなる既存の階層構造に、新規処理を追加するのは困難であることを肝に銘じておく。
 * - 処理の追加を容易にすることを目的に Visitor パターンを適用する。
 * - Visitor の短所に留意する（型追加が困難になる、ダブルディスパッチによるコスト、依存関係が巡回するので実際の利用では複雑さが増す... etc）。
 * 
 * ガイドライン 17: Visitor パターンの実装には std::variant を検討する。
 *
 *  
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_4_visitor.cpp -o ../bin/main
*/
#include <iostream>
#include <cstdlib>
#include <string>
#include <variant>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

struct Print {
    void operator()(int value) const {
        cout << "int: " << value << endl;
    }
    void operator()(double value) const {
        cout << "double: " << value << endl;
    }
    void operator()(const string& value) const {
        cout << "string: " << value << endl;
    }
};

int test_variant() {
    puts("--- test_variant");
    try {
        // Creates a default variant that contains an 'int' initialized to 0
        std::variant<int,double,string> v{};

        v = 42;
        v = 3.14;
        v = 2.71F;
        v = "Jack";
        v = 43;

        const int i = std::get<int>(v);
        ptr_lambda_debug<const char*,const int&>("i is ",i);
        const int* const pi = std::get_if<int>(&v);
        ptr_lambda_debug<const char*,const int*>("pi is ",pi);
        std::visit(Print{}, v);

        return EXIT_SUCCESS;
    } catch(...) {
        return EXIT_FAILURE;
    }
}

class Circle;
class ShapeVisitor {
public:
    virtual ~ShapeVisitor() = default;
    virtual void visit(const Circle&) const = 0;
};
class Shape {
public:
    virtual ~Shape() = default;
    virtual void accept(const ShapeVisitor& sv) = 0;
};

/**
 * 矩形のパターンの追加を行う。
 * 矩形の種類の追加は容易にできる。
 * ただし、Visitor パターンでは 型 のクローズセットが必要とされる。
 * その代償として 処理 のオープンセットが得られると考える。
 * つまり、今後新たに Square を実装する際には、新たに追加される予定の
 * 処理を含めた実装が求められる。 
*/

class Circle final : public virtual Shape {
private:
    double radius;
    Circle():radius{0.00}{}
public:
    explicit Circle(const double& r):radius(r) {
        // TODO Checking that given radius is valid.
    }
    Circle(const Circle& own) {*this = own;}
    ~Circle() {}

    double getRadius() const {
        return radius;
    }
    void accept(const ShapeVisitor& sv) override {
        sv.visit(*this);
    }
};

/**
 * 処理の分離を行う（処理の追加は容易にできる）。
*/

class Draw final : public virtual ShapeVisitor {
public:
    Draw() {}
    Draw(const Draw& own) {*this = own;}
    ~Draw() {}
    void visit(Circle const& circle) const override {
        ptr_lambda_debug<const char*,const double&>("半径：",circle.getRadius());
        puts("Draw Circle.");
    }
};

class Rotate final : public virtual ShapeVisitor {
public:
    Rotate() {}
    Rotate(const Rotate& own) {*this = own;}
    ~Rotate() {}

    void visit(Circle const& circle) const override {
        ptr_lambda_debug<const char*,const double&>("半径：",circle.getRadius());
        puts("Rotate Circle.");
    }
};

int test_Draw_Circle() {
    puts("--- test_Draw_Circle");
    try {
        Circle circle(3.00f);
        Draw draw;
        circle.accept(draw);
        return EXIT_SUCCESS;
    } catch(...) {
        return EXIT_FAILURE;
    }
}

int test_Rotate_Circle() {
    puts("--- test_Rotate_Circle");
    try {
        Circle circle(6.00f);
        Rotate rotate;
        circle.accept(rotate);
        return EXIT_SUCCESS;
    } catch(...) {
        return EXIT_FAILURE;
    }
}


int main() {
    puts("START 4 章 Visitor パターン ===");
    if(0.01) {
        double pi = 3.14159;
        ptr_lambda_debug<const char*,const double&>("pi is ", pi);
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_Draw_Circle());
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_Rotate_Circle());
    }
    if(1.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_variant());
    }
    puts("=== 4 章 Visitor パターン END");
}