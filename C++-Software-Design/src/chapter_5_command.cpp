/**
 * 5 章 Strategy パターンと Command パターン
 * 
 * ガイドライン 21：処理対象を分離するには Command パターン
 * 
 * Command パターンは処理内容の抽象化と分離を狙ったものです。
 * ここでいう処理内容には、（多くの場合）一度だけしか実行しない、かつ、（通常は）その場ですぐ実行する、
 * という性質があることを前提としています。
 * 
 * 目的：要求をオブジェクトとしてカプセル化することによって、異なる要求
 * や、要求からなるキューやログにより、クライアントをパラメータ化する。
 * また、取り消し可能なオペレーションをサポートする。
 * 
 * Command パターンの例として、電卓の実装を考えてみる。
 * 
 * Command パターンと Strategy パターン
 * 構造的に見れば Strategy パターンと Command パターンは同一です。
 * 動的多態性を使おうが静的多態性を使おうが、この両者に構造的な違いはありません。
 * 両者の違いはその目的にあります。
 * Strategy パターンは何をどう（How）行うかを表すのに対し、Command パターンは何を（What）行うかを表します。
 * 
 * ガイドライン 21 の要約
 * - 抽象化と動作（undo できないもの含む）のカプセル化を目的に、Command パターンを適用する。
 * - Command パターンと Strategy パターンの境界線は確固たるものではないことを認識する。
 * - Command パターンでは、静的多態性／動的多態性どちらも可能である。
 * 
 * ガイドライン 22：参照セマンティクスより値セマンティクスを優先する
 * ガイドライン 22 の要約
 * - 参照セマンティクスはコードを理解しにくくすることを理解する。
 * - 意味的に明解な値セマンティクスを優先する。
 * 
 * ガイドライン 23：Strategy パターンと Command パターンの実装には値ベースを優先する
 * ガイドライン 23 の要約
 * - Strategy パターン、Command パターンを実装する際は std::function を検討する（純粋仮想関数がひとつのような場合は有効性が高いと思う）。
 * - std::function の性能上の短所を考慮する（処理スピードは遅い）。
 * - 型消去技術は Strategy パターン、Command パターンの値セマンティクスアプローチの一般化であることを認識する。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_5_command.cpp -o ../bin/main
*/
#include <iostream>
#include <stack>
#include <memory>
#include <cassert>
#include <cstdlib>
#include <span>
#include <vector>
#include <charconv>
#include <string>
#include <string_view>
#include <optional>
#include <functional>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * std::function の紹介
 * 
 * std::function は callable（コーラブル、コール可能なもの。関数ポインタ、関数オブジェクト、ラムダ式など）
 * を抽象化します。
*/

void foo(const int& i) {
    cout << "foo: " << i << endl;
}

int test_foo_functional() {
    puts("--- test_foo_functional");
    try {
        std::function<void(int)> f{};

        f = [](int i) { // Assigning a callable to 'f'
            cout << "lambda: " << i << endl;
        };
        f(1);            // Calling 'f' with the integer '1'

        auto g = f;     // Copying 'f' into 'g' これはディープコピー。
        f = foo;        // Assigning a different callable to 'f'
        f(2);
        g(3);

        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

/**
 * 関数ポインタ型との比較
 * 
 * 次の例では、関数型と関数ポインタ型を両方使っている。
 * 関数型と関数ポインタ型は、int と int を指すポインタのそれとほぼ同じです。
*/

using FunctionType          = double(double);       // 関数型
using FunctionPointerType   = double(*)(double);    // 関数ポインタ型

/**
 * 図形描画をリファクタリング
*/

class Shape {
public:
    virtual ~Shape() = default;
    virtual void draw(/* some arguments */) const = 0;
};

class Circle final : public virtual Shape {
public:
    // Circle クラスは std::function の型エイリアスをメンバ変数に持つ。
    using DrawStrategy = std::function<void(const Circle&)>;        // ここがリファクタされた。
    explicit Circle(const double& r,DrawStrategy d):radius(r)
                                                    ,drawer(std::move(d)) {}    // std::function が表す型のインスタンスを受取、このインスタンスが Strategy 基底クラスを指すポインタの代替となり
                                                                                // その場で同じ型の DrawStrategy 型のメンバ変数へムーブする。
    Circle(const Circle& own) {*this = own;}
    ~Circle() {}

    double getRadius() const { return radius; }
    void draw(/* some arguments */) const override {
        puts("called circle draw().");
        drawer(*this);
    }
private:
    double radius;
    DrawStrategy drawer;
};

class OpenGLCircleStrategy final {
public:
    explicit OpenGLCircleStrategy() {}
    OpenGLCircleStrategy(const OpenGLCircleStrategy& own) {*this = own;}
    ~OpenGLCircleStrategy() {}

    // std::function を GoF Strategy パターンの代替にする際の
    // 従うべき決め事はCircle をとるコール演算子の実装、1 つしかありません。
    void operator()(const Circle& circle) const {           // これが新たに追加された。
        puts("called OpenGLCircleStrategy operator().");
        printf("半径：%lf\n",circle.getRadius());
    }
};

int test_Circle() {
    puts("--- test_Circle");
    Shape* shape = nullptr;
    try {
        shape = new Circle(3.0,OpenGLCircleStrategy{});
        shape->draw();
        // throw runtime_error("test exception.");
        delete shape;
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        if(shape) {
            delete shape;
            puts("DONE delete shape.");
        }
        return EXIT_FAILURE;
    }
}
/**
 * サンプルでは、Circle 同様にSquare も実装、リファクタしている。
 * 重要な事としてここに明記しておく。
 * std::function を用いた実装は性能面ではよくないと書籍は示した。
 * ただし、回避する特殊な方法（型消去）があるが、それでもオブジェクト指向の解より性能面では劣る。
*/


/**
 * Modern C++ の考え方：値セマンティクス
*/
int test_value_semantics() {
    puts("--- test_value_semantics");
    try {
        vector<int> v1 = {1,2,3,4,5};
        auto v2{v1};    // Deep Copy. 自身のメモリ領域を所有し、そこに整数を保持する（v1 内の整数を参照しません）。

        assert(v1 == v2);
        assert(v1.data() != v2.data());
        printf("v1.data() is %p\n",(void*)v1.data());
        printf("v2.data() is %p\n",(void*)v2.data());

        v2[2] = 99;

        assert(v1 != v2);
        const auto v3{v1};
//        v3[2] = 99;     // compilation error.
        printf("v3[2] is %d\n",v3[2]);

        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

/**
 * 値セマンティクス：もうひとつの例
 * 今度は to_int() 関数を考える。
*/

std::optional<int> to_int(std::string_view sv) {
    std::optional<int> oi{};
    int i{};

    const auto result = std::from_chars(sv.data(), sv.data() + sv.size(), i);
    if( result.ec != std::errc::invalid_argument ) {
        oi = i;
    }
    return oi;
}

int test_to_int(const string_view& value) {
    puts("--- test_to_int");
    try {
        if( auto optional_int = to_int(value) ) {
            // Success: the returned std::optional contains an integer value.
            printf("optional_int is %d\n",optional_int.value());
        } else {
            // Failure: the returned std::optional does not contains a value.
            printf("optional_int.has_value() is %d\n",optional_int.has_value());
        }
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

/**
 * 参照セマンティクス、ポインタセマンティクスとも呼ばれる。
 * 
 * やや否定的な意味をもたれることが多いその原因を理解するため、
 * 次に挙げる C++20 の std::span クラステンプレートを例に見てみる。
*/

void print(std::span<int> s) {
    cout << "{";
    for(int i : s) {
        cout << ' ' << i;
    }
    cout << " }" << endl;
}

int test_print() {
    puts("--- test_print");
    try {
        vector<int> v{1,2,3,4};
        const vector<int> w{v};
        const span<int> s{v};

//        w[2] = 99;      // compilation error. const 修飾しているため内部に保持した値の変更はできない。
        s[2] = 99;        // const 修飾しているが、s はポインタを扱っているため、そのポインタの向き先の変更はできないが、値の変更はできる。
        print(s);

        /**
         * std::span のセマンティクスはポインタと同じであるため、参照セマンティクスになるのは当然です。
         * そしてこのことが次の後半で示すような、多くのリスクを生み出す。
        */

        v = {5,6,7,8,9};
        /**
         * 上記は、v の値だけでなくその要素数を増やしているため、内部ではメモリの再割当てが行われる。
         * しかし、そのことは s には通知されない。
         * つまり、s が指すアドレスは以前の v のものであり、すでに開放されたものであり、未定義動作になる。
         * これは、参照セマンティクスに古くからある問題です。
         * 参照セマンティクスはコードの理解と本来重要な細部の推論を困難にする、開発者が避けるべき代物なのです。
        */

        s[2] = 100;
        print(s);
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

/**
 * 電卓コマンド基底クラス
*/
class CalculatorCommand {
public:
    virtual ~CalculatorCommand() = default;
    virtual int execute(int i) const = 0;
    virtual int undo(int i) const = 0;
};

/**
 * 足し算
*/
class Add final : public virtual CalculatorCommand {
private:
    int operand{};
public:
    explicit Add(const int& opd):operand(opd) {}
    Add(const Add& own) {*this = own;}
    ~Add() {}

    int execute(int i) const override {
        return i + operand;
    }
    int undo(int i) const override {
        return i - operand;
    }
};

/**
 * 引き算
*/
class Subtract final : public virtual CalculatorCommand {
private:
    int operand{};
public:
    explicit Subtract(const int& opd):operand(opd) {}
    Subtract(const Subtract& own) {*this = own;}
    ~Subtract() {}

    int execute(int i) const override {
        return i - operand;
    }
    int undo(int i) const override {
        return i + operand;
    }
};

/**
 * Invoker
 * Command パターン の集約を行っている。
*/
class Calculator final {
private:
    int current{};
    using CommandStack = std::stack<CalculatorCommand*>;    // using は型の別名として利用している。
    CommandStack stack;
public:
    Calculator() {}
    Calculator(const Calculator& own) {*this = own;}
    ~Calculator() {}

    void compute(CalculatorCommand* command) {
        current = command->execute(current);
        stack.push(std::move(command));
    }
    void undoLast() {
        if(stack.empty()) return;
        auto command = std::move(stack.top());
        stack.pop();
        current = command->undo(current);
    }
    int result() {
        return current;
    }
    void clear() {
        current = 0;
        CommandStack{}.swap(stack); // Cleaning the stack.
    }
};

int test_Calculator() {
    puts("--- test_Calculator");
    try {
        Calculator calc{};
        const int before = calc.result();
        ptr_lambda_debug<const char*,const int&>("before is ",before);
        CalculatorCommand* op1 = new Add(3);
        CalculatorCommand* op2 = new Add(7);
        CalculatorCommand* op3 = new Subtract(4);
        CalculatorCommand* op4 = new Subtract(2);

        calc.compute(op1);
        calc.compute(op2);
        calc.compute(op3);
        calc.compute(op4);

        calc.undoLast();
        const int result = calc.result();
        ptr_lambda_debug<const char*,const int&>("result is ",result);
        assert(result == 6);

        delete op1;
        delete op2;
        delete op3;
        delete op4;
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

int main() {
    puts("START 5 章 Strategy パターンと Command パターン ===");
    if(0.01) {
        int x = 3;
        ptr_lambda_debug<const char*,const int&>("x is ", x);
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_Calculator());
    }
    if(1.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_print());
    }
    if(1.02) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_value_semantics());
    }
    if(1.03) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_to_int("42"));
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_to_int("NNN"));
    }
    if(1.04) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_foo_functional());
    }
    if(1.05) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_Circle());
    }
    puts("=== 5 章 Strategy パターンと Command パターン END");
}