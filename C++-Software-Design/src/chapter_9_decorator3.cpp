/**
 * 9 章 Decorator パターン
 * 
 * ガイドライン 36 ：実行時抽象化とコンパイル時抽象化間のトレードオフを理解する
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_9_decorator3.cpp -o ../bin/main
*/
#include <iostream>
#include <memory>
#include <utility>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * 金額クラス
*/
class Money {
private:
    double amount;      // 額（金額）
    // TODO 今回の学習とは全く関係がないが、951.048 等の場合、小数点以下第 3 位を切り上げする方法を考えてみよう。
    double price;       // 価格
    Money(): amount{0}, price{0} {}
public:
    Money(const double& _amount): amount(_amount), price{0} 
    {}
    Money(const Money& own) {*this = own;}
    ~Money() {}

    double getAmount() const {
        return amount;
    }
    double getPrice() const {
        return price;
    }
    Money operator*(const double& factor) {
        price = amount + (amount * factor);     // 私の実装では、ここで税率を含めた計算を行っているので、サンプルとは実装が異なる。
        return *this;
    }
    Money operator+(Money rhs) {
        amount = amount + rhs.amount;
        price = price + rhs.price;
        return *this;
    }
    Money operator-(Money rhs) {
        amount = amount - rhs.amount;
        price = price - rhs.price;
        return *this;
    }
};

/**
 * 値ベースの実行時 Decorator パターン
 * 
 * コンパイル時 Decorator パターンの速度性能は優れていいますが、柔軟性はまったく期待できません。
 * ここで頭を値ベースのもう 1 つの実装に向け、実行時の柔軟性を備えた動的多態性に立ち返りましょう。
 * 
 * 値セマンティクスの実装へ変身させるデザインパターン、Type Erasure パターンです。
 * 次に挙げる Item クラスは、価格をつけた商品用に、自らが所有権を持つ Type Erasure ラッパクラスを実装します。
*/

int main(void) {
    puts("START 9 章 Decorator パターン 値ベースの実行時 Decorator パターン ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    puts("=== 9 章 Decorator パターン 値ベースの実行時 Decorator パターン END");
    return 0;
}