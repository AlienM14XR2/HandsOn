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
 * 
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

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * 参照セマンティクス、ポインタセマンティクスとも呼ばれる。
 * やや否定的な意味をもたせられることが多いその原因を理解するため、
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
 * 電卓基底クラス
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
    puts("=== 5 章 Strategy パターンと Command パターン END");
}