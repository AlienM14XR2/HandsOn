/**
 * 12 章 Compound パターン
 * 
 * パターンのパターン
 * パターンの連携
 * Compound とは 複合という意味
 * 
 * Compound パターンは複数のパターンを組み合わせ、繰り返し起こる問題や
 * 一般的な問題を解決するための解決方法。
 * MVCがそれに該当する。
 * 
 * 
 * e.g. compile
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_12_compound.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug)-> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * 再びカモ インタフェース（鳴き声）
*/
class Quackable {
public:
    virtual void quack() const = 0;
};
/**
 * マガモの鳴き声クラス
*/
class MallardDuck final : public virtual Quackable {
public:
    virtual void quack() const override {
        puts("ガーガー");
    }
};
/**
 * 鴨笛クラス
*/
class DuckCall final : public virtual Quackable {
public:
    virtual void quack() const override {
        puts("ガァガァ");
    }
};
/**
 * ゴム製のアヒル クラス
*/
class RubberDuck final : public virtual Quackable {
public:
    virtual void quack() const override {
        puts("キューキュー");
    }
};

int main(void) {
    puts("START 12 章 Compound パターン ===");
    if(0.01) {
        double pi = 3.14159;
        ptr_lambda_debug<const char*,const double&>("pi is ", pi);
    }
    puts("=== 12 章 Compound パターン END");
    return 0;
}