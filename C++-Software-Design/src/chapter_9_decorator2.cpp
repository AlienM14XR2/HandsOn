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
#include <string>
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
 * 値ベースのコンパイル時 Decorator パターン
 * 
 * 抽象化の主要部分にはテンプレートを使います。また　C++20　のコンセプトも転送参照も使います。
 * ですが、そんなことよりも Decorator パターンを用いた設計という重要な目的を見失ってはいけません。
 * 通常商品の種類が増えた場合や、新たな価格変更要因に対応する decorator を容易に追加できるように
 * することが目的です。
 * ConferenceTicket クラスを例に進めます。
 * 
 * ConferenceTicket クラスは値型にうってつけです。基底クラスも仮想関数もありません。
 * すなわち、基底クラスを指すポインタを介し修飾する必要がありません。
 * その代わり、コンポジションか、または非 public な直接継承で修飾します。
 * 
 * Discounted クラスと Taxed クラスの実装例を挙げます。
*/


/**
 * ConferenceTicket クラス
*/
class ConferenceTicket final {      // Item の継承を止める
private:
    std::string name;
    Money money;
public:
    ConferenceTicket(std::string _name, Money _money): name{std::move(name)}, money{std::move(_money)} 
    {}

    const std::string& getName() const { return name; }
    Money price() const { return money; }
};

/**
 * PricedItem コンセプト
 * 注意：concept は c++20 から利用可能
*/
template <class T>
concept PricedItem = 
    requires (T item) {
        { item.price() } -> std::same_as<Money>;        // この書き方も知らなかった。
    };

/**
 * Discounted クラス
*/
template <double discount, PricedItem Item>
class Discounted {        // コンポジションを利用する。
private:
    Item item;
public:
    template <class... Args>
    explicit Discounted(Args&&... args): item{ std::forward<Args>(args)... }    // この書き方もエグイ。
    {}

    Money price() const {
        double amount = item.price().getAmount() - (item.price().getAmount() * discount);
        return Money{amount};
//        return item.price() * (discount);     Money の実装がサンプルとは異なるため（サンプルには具体的な実装がなかったはず）、これでは駄目。
    }
};

/**
 * Taxed クラス
*/
template <double taxRate, PricedItem Item>
class Taxed : private Item {      // 継承を利用する。
public:
    template <class...  Args>
    explicit Taxed( Args&&... args ):Item{ std::forward<Args>(args)... } 
    {}

    Money price() const {
        return Item::price() * (taxRate);
    }
};

/**
 * Discounted クラスと Taxed クラスは、他の Item 用の decorator です。
 * 今回の実装はクラステンプレートという形です。それぞれのテンプレートの先頭引数に値引き率、税率を指定し、
 * 次の引数には修飾した Item の型を指定します。
 * 
 * ここで特筆すべきは制約引数の PricedItem です（テンプレートの第 2 引数）。
 * 制約引数とは論理的な要件、すなわち期待する動作を表現します。この制約により、指定できるをメンバ関数 price() を持つ Item 
 * のみに制限でき、他の型を指定するとその場でコンパイルエラーとなります。
 * この PricedItem は 「ガイドライン 35」の Decorator パターン実装の Item 基底クラスと同じ役割を持ち、また『単一責任の原則』
 * に基づいた関心の分離も実現しています。
*/

int test_ConferenceTicket() {
    puts("--- test_ConferenceTicket");
    try {
        Taxed<0.15, Discounted<0.2,ConferenceTicket>> item{"Core C++", 499.0};
        const Money total = item.price();
        ptr_lambda_debug<const char*,const double&>("total price is ",total.getPrice());
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

/**
 * このアプローチには深刻なレベルになり得る制限が複数あります。
 * テンプレートですべてに対応するため、実行時の柔軟性が完全に欠落してしまう点です。
 * 値引率、税率でさえテンプレート引数として実装されるため、新たな型もそれぞれ作成しなければなりません。
 * このことはコンパイル時間が延び、また生成コードも多くなることを意味します（実行ファイルのサイズ増加）。
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
    if(2.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_ConferenceTicket());        
    }
    puts("=== Decorator パターン END");
    return 0;
}