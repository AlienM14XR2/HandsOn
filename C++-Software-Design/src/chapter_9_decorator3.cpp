/**
 * 9 章 Decorator パターン
 * 
 * ガイドライン 36 ：実行時抽象化とコンパイル時抽象化間のトレードオフを理解する
 * 
 * ガイドライン 36 の要約
 * - コンパイル時 Decorator パターン実装と実行時 Deccorator パターン実装、いずれにも注意すべき点がある。
 * - コンパイル時の解は一般に優れた実行性能を得られるが、実行時の柔軟性とカプセル化という点では劣ることを理解する。
 * - 実行時の解は柔軟性が高く、また実装詳細を上手く隠蔽するが、実行性能では劣る点を理解する。
 * - 参照セマンティクスの解よりも値セマンティクスの解を優先する。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_9_decorator3.cpp -o ../bin/main
*/
#include <iostream>
#include <memory>
#include <utility>
#include <cassert>

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
 * 
 * Type Erasure = External Polymorphism + Bridge + prototype
*/

class Item {
private:
    /**
     * Concept 基底クラスは、通例通りラッピングした型に対する要件（期待される動作）を表現します。
    */
    struct Concept {
        virtual ~Concept() = default;
        virtual Money price() const = 0;
        virtual std::unique_ptr<Concept> clone() const = 0;
    };

    /**
     * Model クラステンプレートは、Concept 基底クラスの具象化を行う。
     * すなわち、price()、 clone() 関数の実装。
     * ただし、あくまで クラステンプレートなので、具体的な処理は T で表現されるところによる。
    */
    template <class T>
    struct Model : public Concept {
        T item;
        explicit Model(const T& _item): item(_item)
        {}
        explicit Model(T&& _item): item(std::move(_item))
        {}

        Money price() const override {
            return item.price();
        }
        std::unique_ptr<Concept> clone() const override {
            return std::make_unique<Model<T>>(*this);
        }
    };
    std::unique_ptr<Concept> pimpl;
public:
    /**
     * 五関数同時ルール（https://oreil.ly/fzS3f）に従い実装し、さらに全種類の商品を受け取れる
     * テンプレートのコンストラクタも実装します。
    */
    template <class T>
    Item(T item): pimpl{ std::make_unique<Model<T>>(std::move(item)) } 
    {}
    Item(const Item& item): pimpl{ item.pimpl->clone() }
    {}
    ~Item() = default;
    Item(Item&&) = default;
    Item& operator=(Item&&) = default;

    Item& operator=(const Item& item) {
        pimpl = item.pimpl->clone();
        return *this;
    }
    /**
     * 全商品に必要なインタフェースの再現です。
    */
    Money price() const {
        return pimpl->price();
    }
};

/**
 * 上例のラッパクラスを用いれば、新商品を容易に追加でき、既存コードに干渉しない。
 * また、特定の基底クラスの使用を強制することもありません。
 * price() メンバ関数を持ち、かつコピー可能なクラスならば、何でも追加できます。
 * 先に挙げた、コンパイル時 Decorator パターンの実装にある ConferenceTicket がまさにこれに該当します。
 * 必要なものが揃っており、確かに値セマンティクスを基にしています。しかし Discounted クラスと Taxed クラスは
 * 違います。テンプレート引数に修飾された Item を必要とします。
 * そのため、この2つのクラスは Type Erasure パターン用に再実装する必要があります。
*/

class Discounted {
private:
    Item item;
    double factor;
public:
    Discounted(double discount, Item _item): item{std::move(_item)}, factor(discount)
    {}

    Money price() const {
        double amount = item.price().getAmount() - (item.price().getAmount() * factor);
        return Money{amount};
    }
};

class Taxed {
private:
    Item item;
    double factor;
public:
    Taxed(double taxRate, Item _item): item{std::move(_item)}, factor(taxRate) 
    {}

    Money price() const {
        return item.price() * factor;
    }
};

/**
 * 注目すべきは、上例の 2 つのクラスはどちらも派生クラスではなく、それでいて Decorator パターンを完璧に
 * 実装しています。両クラスは Item ラッパクラスが要件とする内容、すなわち商品として扱うための price() メンバ関数
 * とコピーコンストラクタを実装しており、さらに自身でも Item を保持します。そのため、次の test_Item() に示すよう、
 * 任意に Decorator パターンと組み合わせることができるのです。
*/

class ConferenceTicket {
private:
    std::string name;
    Money money{0};
public:
    explicit ConferenceTicket(std::string _name, Money _money): name{std::move(_name)}, money{std::move(_money)} 
    {}

    const std::string& getName() const { return name; }
    Money price() const { return money; }
};

int test_Item() {
    puts("--- test_Item");
    try {
        // 20% discount, 15% tax : (499*0.8)*1.15 = 459.08
        double expects = 459.08;
        Item item(Taxed(0.15, Discounted(0.2, ConferenceTicket{"Core C++",499.0})));
        Money total = item.price();
        ptr_lambda_debug<const char*,const double&>("total price is ",total.getPrice());
        ptr_lambda_debug<const char*,const double&>("expects is ",expects);
        assert(total.getPrice() == expects);
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

int main(void) {
    puts("START 9 章 Decorator パターン 値ベースの実行時 Decorator パターン ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_Item());
    }
    puts("=== 9 章 Decorator パターン 値ベースの実行時 Decorator パターン END");
    return 0;
}