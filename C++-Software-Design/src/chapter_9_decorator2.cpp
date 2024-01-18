/**
 * 9 章 Decorator パターン
 * 
 * ガイドライン 36 ：実行時抽象化とコンパイル時抽象化間のトレードオフを理解する
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_9_decorator2.cpp -o ../bin/main
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

int test_Money() {
    puts("--- test_Money");
    try {
        Money m1{30}, m2{60};
        Money m3 = m1 + m2;         // こわいな、これはまず、m1 に m2 を足している、つまりこの段階で m1 の amount は 90 になっている。
        m3 = m3 * 0.1;
        double resultAmount = -1;
        double resultPrice = -1;
        ptr_lambda_debug<const char*,const double&>("m3 amount is ",resultAmount = m3.getAmount());
        ptr_lambda_debug<const char*,const double&>("m3 price is ",resultPrice = m3.getPrice());
        assert(resultAmount == 90);
        assert(resultPrice == 99);

        m1 = Money(30);
        m2 = Money(60);
        Money m4{0};
        m4 = (m2 - m1);
        ptr_lambda_debug<const char*,const double&>("m4 amount is ",resultAmount = m4.getAmount());
        ptr_lambda_debug<const char*,const double&>("m4 price is ",m4.getPrice());
        assert(resultAmount == 30);

        m1 = Money(30);
        m2 = Money(60);
        Money m5 = (m1 - m2);
        ptr_lambda_debug<const char*,const double&>("m4 amount is ",resultAmount = m5.getAmount());
        ptr_lambda_debug<const char*,const double&>("m4 price is ",m5.getPrice());
        assert(resultAmount == -30);
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

/**
 * 商品基底クラス
*/
class Item {
public:
    virtual ~Item() = default;
    virtual Money price() const = 0;
};

/**
 * 商品拡張クラス
 * Decorator パターンの肝になるもの。
*/
class DecoratedItem : public Item {
private:
    std::unique_ptr<Item> item;
protected:
    Item&       getItem()       { return *item; }
    const Item& getItem() const { return *item; }
public:
    explicit DecoratedItem(std::unique_ptr<Item> _item) : item(std::move(_item))
    {
        if(!item) {
            throw std::invalid_argument("Invalid item.");
        }
    }
};

/**
 * C++ 書籍
 * 商品の具象クラス。
*/
class CppBook final : public Item {
private:
    std::string title;
    Money money{0};
public:
    explicit CppBook(std::string _title, Money _money) : title{std::move(_title)}, money{std::move(_money)}
    {}

    const std::string& getTitle() const { return title; }
    Money price() const override { return money; }
};

/**
 * Discounted クラス
 * Item を指す std::unique_ptr と 値引き率を表す double 型（0.0 から 1.0）で初期化します。
*/
class Discounted final : public DecoratedItem {
private:
    double factor;
public:
    explicit Discounted(double discount, std::unique_ptr<Item> item): DecoratedItem(std::move(item)), factor(discount)     // この初期化順番を逆にはできないから注意が必要。
    {
        if( discount < 0.0 || discount > 1.0 ) {
            ptr_lambda_debug<const char*,const double&>("your input discount is ",discount);
            throw std::invalid_argument("Invalid discount");
        }
    }

    /**
     * price() 関数こそが、Decorator パターンをフル活用する箇所になる。
    */

    Money price() const override {
        double amount = getItem().price().getAmount() - (getItem().price().getAmount() * factor);
        return Money{amount};
//        return getItem().price() * factor;
    }
};

/**
 * Taxed クラス
 * コンストラクタで税率を計算している。
*/
class Taxed final : public DecoratedItem {
private:
    double factor;
public:
    explicit Taxed(double taxRate, std::unique_ptr<Item> item): DecoratedItem(std::move(item)), factor(taxRate)
    {
        if( taxRate < 0.0 ) {
            ptr_lambda_debug<const char*,const double&>("your input taxRate is ",taxRate);
            throw std::invalid_argument("Invalid tax");
        }
    } 

    Money price() const override {
        return getItem().price() * factor;
    }
};

/**
 * 値ベースのコンパイル時 Decorator パターン
 * 
 * 抽象化の主要部分にはテンプレートを使います。また　C++20　のコンセプトも転送参照も使います。
 * ですが、そんなことよりも Decorator パターンを用いた設計という重要な目的を見失ってはいけません。
 * 通常商品の種類が増えた場合や、新たな価格変更要因に対応する decorator を容易に追加できるように
 * することが目的です。
 * ConferenceTicket クラスを例に進めます。
*/



int main(void) {
    puts("START Decorator パターン ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_Money());
    }
    puts("=== Decorator パターン END");
    return 0;
}