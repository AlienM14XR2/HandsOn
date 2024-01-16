/**
 * 9 章 Decorator パターン
 * 
 * ガイドライン 35 ：カスタマイズを階層化するには Decorator パターン
 * 
 * 解明 Decorator パターン
 * Decorator パターンもやはり GoF 本に収録されているもので、異なる種類の機能をコンポジションにより
 * 柔軟に組み合わせることを主眼に置いています。
 * 
 * Decorator パターン
 * 目的：オブジェクトに責任を動的に追加する。Decorator パターンは、サブクラス化よりも
 *       柔軟な機能拡張方法を提供する。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_9_decorator.cpp -o ../bin/main
*/
#include <iostream>
#include <cassert>
#include <stdexcept>
#include <utility>
#include <memory>
#include <string>

using namespace std;

template <class Message, class Debug>
void (*ptr_lambda_debug)(Message, Debug) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * 商品売買管理システムをリファクタリングする形で、書籍は説明するようだ。
 * まずは、Decorator パターンの古典的実装から行う。
*/

/**
 * 金額クラス
*/
class Money {
private:
    double amount;      // 額（金額）
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
        price = amount + (amount * factor);
        return *this;
    }
    Money operator+(Money rhs) {
        amount = amount + rhs.amount;
        price = price + rhs.price;
        return *this;
    }
};

int test_Money() {
    puts("--- test_Money");
    try {
        Money m1{30}, m2{60};
        Money m3 = m1 + m2;
        m3 = m3 * 0.1;
        double resultAmount = -1;
        double resultPrice = -1;
        ptr_lambda_debug<const char*,const double&>("m3 amount is ",resultAmount = m3.getAmount());
        ptr_lambda_debug<const char*,const double&>("m3 price is ",resultPrice = m3.getPrice());
        assert(resultAmount == 90);
        assert(resultPrice == 99);
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
    {}
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
 * カンファレンスチケット
 * 商品の具象クラス。
*/
class ConferenceTicket final : public Item {
private:
    std::string name;
    Money money{0};
public:
    explicit ConferenceTicket(std::string _name, Money _money): name{std::move(_name)}, money{std::move(_money)} 
    {}

    const std::string& getName() const { return name; }
    Money price() const override { return money; }
};

/**
 * CppBook クラスと ConferenceTicket クラスは Item の具象実装です。
 * 重要なのは両クラスとも price() をオーバーライドし、money を返す点です。
 * どちらも、税率や値引きを意識していませんが、どちらもその影響を受け、価格が
 * 変化するのは明らかです。
 * 価格変更要因は Discounted クラスと Taxed クラスという形で実装します。
*/

class Discounted final : public DecoratedItem {

};

class Taxed final : public DecoratedItem {
    
};

int main(void) {
    puts("START カスタマイズを階層化するには Decorator パターン ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_Money());
    }
    puts("=== カスタマイズを階層化するには Decorator パターン END");
    return 0;
}