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
 * ガイドライン 35 の要約
 * - 継承が解となる場合は少ないことを理解する。
 * - 既存コードに干渉せず、かつ階層的に動作を拡張／カスタマイズする目的を持って Decorator パターンを使用する。
 * - 動作の独立した部分を再利用し、組み合わせる場合には Decorator パターンの使用を検討する。
 * - Decorator、Adapter、Strategy パターンの違いを理解する。
 * - Decorator パターンの高度な柔軟性を活用する。しかし、短所（実装時の間違い、利用間違い、仮想関数の複数コールによるオーバーヘッド等）も肝に銘じておく。
 * - 無意味な Decorator パターンはいけない。正しく使用できる設計を優先する。
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
#include <array>
#include <vector>
#include <cstddef>
#include <cstdlib>
#include <memory_resource>

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
        price = price -rhs.price;
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

int test_DecoratedItem() {
    puts("--- test_DecoratedItem");
    try {
        // 7% tax: 19 * 1.07 = 20.33
        std::unique_ptr<Item> item1(
            std::make_unique<Taxed>(
                0.07
                , std::make_unique<CppBook>("Effective C++",19)
            )
        );

        std::unique_ptr<Item> item2(
            std::make_unique<Taxed>(
                0.19
                ,std::make_unique<Discounted>(0.2
                    ,std::make_unique<ConferenceTicket>("CppCon",999.0))
                
            )
        );

        const Money totalPrice1 = item1->price();
        double result1 = -1.0;
        ptr_lambda_debug<const char*,const double&>("totalPrice1.getPrice() is ",result1 = totalPrice1.getPrice());
        assert(result1 == 20.33);
        const Money totalPrice2 = item2->price();
        ptr_lambda_debug<const char*,const double&>("totalPrice2.getPrice() is ",totalPrice2.getPrice());
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

/**
 * Decorator パターンの例 その 2
 * C++17 で再実装された STL アロケータです。
*/

int test_allocator() {
    puts("--- test_allocator");
    try {
        std::array<std::byte,1000> raw;       // 注意：未初期化

        std::pmr::monotonic_buffer_resource buffer{raw.data(), raw.size(), std::pmr::null_memory_resource()};
        /**
         * std::pmr::null_memory_resource() は
         * buffer を使い切った場合は、bad_alloc 例外をスローするためのもの。
         * 
         * std::pmr::new_delete_resource() は
         * 動的にメモリ割当を行う。
        */
        std::pmr::vector<std::pmr::string> strings{&buffer};

        strings.emplace_back("String longer than SSO can handle");
        strings.emplace_back("Another long string that goes beyond SSO");
        strings.emplace_back("A third long string that cannot be handle by SSO");

        for(std::pmr::string s: strings) {
            cout << s << endl;
        }
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

int main(void) {
    puts("START カスタマイズを階層化するには Decorator パターン ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_Money());
    }
    if(1.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_DecoratedItem());
    }
    if(2.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_allocator());
    }
    puts("=== カスタマイズを階層化するには Decorator パターン END");
    return 0;
}