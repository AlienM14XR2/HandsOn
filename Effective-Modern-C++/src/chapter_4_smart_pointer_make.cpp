/**
 * 4 章 スマートポインタ
 * 
 * 項目 21 ：new の直接使用よりも std::make_unique や std::make_shared を優先する
 * 
 * std::make_shared は C++11 の一部となっていますが、std::make_unique は C++14 から標準ライブラリに加えられたものです。
 * もし、C++11 しか使えなくても std::make_unique の基本バージョンは自作が容易ですから。
 * 
 * 重要ポイント
 * - new の直接使用と比較すると、make 関数はソースコードの重複を避け、例外安全性を向上できる。また、std::make_shared および
 *   std::allocate_shared では、高速かつサイズの小さなコードを生成できる。
 * - make 関数が適切ではない場面には、カスタムデリータを指定する場合、および波括弧による初期化を目的とする場合の 2 つがある。
 * - std::shared_ptr の場合では、make 関数が適切ではない場面が増える。
 *   （1）専用のメモリ管理を実装したクラス
 *   （2）メモリに特別な注意が必要なシステム、巨大オブジェクト、対応する std::shared_ptr とは異なるライフタイムを持つ std::weak_ptr
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_4_smart_pointer_make.cpp -o ../bin/main
*/
#include <iostream>
#include <memory>
#include <set>

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](const auto message, const auto debug) -> void {
    std::cout << "DEBUG: " << message << '\t' << debug << std::endl;
};

template <class Error>
concept Reasonable = requires(Error& e) {
    e.what();
};

template <class Error>
requires Reasonable<Error>
void (*ptr_print_error)(Error) = [](const auto e) -> void {
    std::cout << "ERROR: " << e.what() << std::endl;
};

int test_debug() {
    puts("=== test_debug");
    try {
        auto pi = 3.141592;
        ptr_lambda_debug<const char*,const decltype(pi)&>("pi is ", pi);
        ptr_lambda_debug<const char*,const decltype(pi)*>("pi addr is ", &pi);
        throw std::runtime_error("It's test error.");
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        printf("e type is ... %s\n", typeid(e).name());
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * std::make_shared は C++11 の一部となっていますが、std::make_unique は C++14 から標準ライブラリに加えられたものです。
 * もし、C++11 しか使えなくても std::make_unique の基本バージョンは自作が容易ですから。
*/

template <class T, class... Args>
std::unique_ptr<T> make_unique(Args&&... params) {
    return std::unique_ptr<T>(new T(std::forward<Args>(params)...));
}

int test_my_make_unique() {
    puts("=== test_my_make_unique");
    try {
        std::unique_ptr<int> ip = make_unique<int>(99);
        ptr_lambda_debug<const char*,const int&>("ip val is ", *ip.get()); 
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

class Widget {

};

void sample() {
    auto upw1(std::make_unique<Widget>());          // make 関数を使用
    ptr_lambda_debug<const char*,const Widget*>("upw1 addr is ", upw1.get());
    
    std::unique_ptr<Widget> upw2(new Widget());     // make 未使用
    ptr_lambda_debug<const char*,const Widget*>("upw2 addr is ", upw2.get());

    auto spw1(std::make_shared<Widget>());          // make 関数を使用
    ptr_lambda_debug<const char*,const Widget*>("spw1 addr is ", spw1.get());

    std::shared_ptr<Widget> spw2(new Widget());      // make 未使用
    ptr_lambda_debug<const char*,const Widget*>("spw2 addr is ", spw2.get());

    /**
     * new を用いたバージョンでは、作成する型を 2 度記述していますが、make 関数を使用するバージョンでは 1 度です。
     * 型を繰り返し記述することは、ソフトウェア工学での重要な信条、コードの重複を避けよ、に反します。
    */
}

/**
 * make 関数が望ましい理由の 2 つ目は、例外安全性への対応です。何らかの優先度を考慮しつつ Widget を処理する関数
 * を考えてみましょう。
 * 
 * int computePriority();
 * 
 * processWidget(std::shared_ptr<Widget>(new Widgete)), computePriority());    // リソース解放漏れの恐れあり！！
 * 
 * 上例では new で作成した Widget の解放漏れが発生する恐れがあります。
 * その答えは、ソースコードをオブジェクトコードへ翻訳するコンパイラにあります。実行時に関数に渡す実引数は、その関数の
 * 実行開始前に評価する必要があるため、processWidget 呼び出しでは、processWidget が実行を開始できるようになる前に、次
 * の内容を実行しなければなりません。
 * 
 * - 「new Widget」という式を評価しなければならない。すなわち、Widget をヒープ上に作成しなければならない。
 * - new により生成されたポインタの管理に責任を持つ、std::shared_ptr<Widget> のコンストラクタを実行しなければならない。
 * - computePriority を実行しなければならない。
 * 
 * 上例の順序で実行するようコードを生成することは、コンパイラの要件とはされていません。new の結果がコンストラクタの実
 * 引数になるため、「new Widget」は std::shared_ptr コンストラクタを呼び出す前に実行しなければなりませんが、computePriority
 * は new や std::shared_ptr よりも前でも後でも構わないのです。ここで重要なのは『二者の間』、すなわちコンパイラが次の順序で
 * 実行するコードを生成する場合です。
 * 
 * 1. 「new Widget」
 * 2. computePriority
 * 3. std::shared_ptr コンストラクタ
 * 
 * 上記の順序で実行するコードが生成され、かつ実行時に computePriority が例外を発生させると、ステップ 1 でダイナミックに割り
 * 当てた Widget が解放漏れとなってしまいます。
 * std::shared_ptr に保持させ、ライフタイムの管理が開始されるのはステップ 3 であるためです。
 * std::make_shared を用いればこの問題を回避できます。呼び出し側のコードは次のようなものになります。
 * 
 * processWidget(std::make_shared<Widget>(), computePriority());            // リソース解放漏れの恐れがない
*/

/**
 * new の直接使用よりも make 関数を優先する理由は確固たるものです。しかし、ソフトウェア工学上の優位点、例外安全性、効率向上に
 * も関わらず、本項目は絶対に make 関数を用いよとは述べておらず、優先せよとしています。これには理由があり、make 関数を使用でき
 * ない、または使用すべきではない場面も存在するのです。
 * 
 * 1 つはカスタムデリータ、2 つ目はあるオブジェクトを作成する際、そのコンストラクタがオーバーロードされており、仮引数に std::initializer_list
 * をとるものと、とらないものがある場合、波括弧を記述したオブジェクト作成は std::initializer_list をとるコンストラクタにオーバーロード
 * 解決されます。波括弧ではなく丸括弧を記述した場合は、std::initializer_list をとらないコンストラクタです。
 * make 関数では、内部でオブジェクトのコンストラクタに仮引数を完全転送しますが、ここで使用するのは丸括弧でしょうか、それとも波括弧でしょうか？
 * 型にもよりますが、この答えは大きく差異となります。
 * 
 * std::unique_ptr で make 関数が問題となるのはこの 2 点だけです（カスタムデリータと波括弧による初期化）。
*/

/**
 * std::shared_ptr および make 関数では、問題点はもう 2つあります。いずれも極端な場合ですが、極端な世界に生きる開発者もいます。
 * クラスによっては自身で operator new や operator delete を定義するものがあります。これらの関数を定義したということは、そのクラス
 * オブジェクトに対してはグローバルなメモリ割当／解放関数が不適切であることを意味します。
 * 専用の operator new と operator delete を持つ型のオブジェクトの作成に make 関数を用いるのは悪手になります。
*/

/**
 * Coffee Break
 * 
 * 以前実装した Strategy Decorator Observer Factory の内で
 * Observer を std::shared_ptr と std::weak_ptr でリファクタリングしてみる。
 * 理由は単純、 前項の重要ポイントに次の記述があったから。
 * 
 * - 不正になる可能性がある std::shared_ptr ライクなポインタには std::weak_ptr を使用する
 * - std::weak_ptr を使用する可能性がある場面としては、キャッシュ、※『observer リスト』、std::shared_ptr の循環防止がある。
 * 
 * ※ が該当するとあったからに他ならない。
*/

class Investment {
public:
    virtual ~Investment() = default;
    virtual void deal() = 0;
};

template<class T>
class DealStrategy {
public:
    virtual ~DealStrategy() = default;
    virtual void deal(T&) = 0;
};

template<class Subject>
class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(Subject&) = 0;
};

class Stock final : public Investment {
public:
    using InvestorObserver = Observer<Stock>;
    Stock(std::unique_ptr<DealStrategy<Stock>>& _dealStrategy) : dealStrategy{std::move(_dealStrategy)}
    {}
    // ...
    virtual void deal() override {
        puts("------ Stock::deal");
        dealStrategy.get()->deal(*this);
        notify();
    }
    bool attach(std::unique_ptr<InvestorObserver>& investorObserver) {
        auto [pos,success] = observers.insert(std::move(investorObserver));  // この書き方初めて見る、pair を返却するからか。
        return success;
    }
    bool detach(std::unique_ptr<InvestorObserver>& investorObserver) {
        return (observers.erase(std::move(investorObserver)) > 0U);
    }
    void notify() {
        for(auto iter = begin(observers); iter != end(observers); ) {
            const auto observer = iter++;
            observer->get()->update(*this);         // Observer（観察者）に通知している。
        }
    }
private:
    std::unique_ptr<DealStrategy<Stock>> dealStrategy;
    std::set<std::shared_ptr<InvestorObserver>> observers;      // ここを std::shared_ptr にする、std::unique_ptr からの変換は簡単とあったので、attach detach のシグネチャは変えないで一度確認してみる。
};

class Bond final : public Investment {
private:
    std::unique_ptr<DealStrategy<Bond>> dealStrategy;
public:
    Bond(std::unique_ptr<DealStrategy<Bond>>& _dealStrategy) : dealStrategy{std::move(_dealStrategy)} 
    {}
    virtual void deal() override {
        puts("------ Bond::deal");
        dealStrategy.get()->deal(*this);
    }
};

class RealEstate final : public Investment {
private:
    std::unique_ptr<DealStrategy<RealEstate>> dealStrategy;
public:
    RealEstate(std::unique_ptr<DealStrategy<RealEstate>>& _dealStrategy) : dealStrategy{std::move(_dealStrategy)} 
    {}
    virtual void deal() override {
        puts("------ RealEstate::deal");
        dealStrategy.get()->deal(*this);
    }
};

enum struct InvestmentType {
    STOCK,
    BOND,
    REAL_ESTATE,
    FUTURES_CONTRACT,
};

class StockDeal final : public DealStrategy<Stock> {
private:
    std::unique_ptr<DealStrategy<Stock>> dealStrategy;
public:
    StockDeal() {}
    StockDeal(std::unique_ptr<DealStrategy<Stock>>& _dealStrategy) : dealStrategy{std::move(_dealStrategy)}
    {}
    // ...
    virtual void deal(Stock& stock) override {
        if(dealStrategy.get()) {
            dealStrategy.get()->deal(stock);
        }
        puts("------ StockDeal::deal");
        ptr_lambda_debug<const char*,Investment*>("stock addr is \t\t", &stock);
    }
};

class StockDealA final : public DealStrategy<Stock> {
private:
    std::unique_ptr<DealStrategy<Stock>> dealStrategy;
public:
    StockDealA() 
    {}
    StockDealA(std::unique_ptr<DealStrategy<Stock>>& _dealStrategy): dealStrategy{std::move(_dealStrategy)}
    {}
    // ...
    virtual void deal(Stock& stock) override {
        if(dealStrategy.get()) {
            dealStrategy.get()->deal(stock);
        }
        puts("------ StockDealA::deal");
        ptr_lambda_debug<const char*,Investment*>("stock addr is ", &stock);
    }
};

class StockDealB final : public DealStrategy<Stock> {
private:
    std::unique_ptr<DealStrategy<Stock>> dealStrategy;
public:
    StockDealB() 
    {}
    StockDealB(std::unique_ptr<DealStrategy<Stock>>& _dealStrategy): dealStrategy{std::move(_dealStrategy)}
    {}
    // ...
    virtual void deal(Stock& stock) override {
        if(dealStrategy.get()) {
            dealStrategy.get()->deal(stock);
        }
        puts("------ StockDealB::deal");
        ptr_lambda_debug<const char*,Investment*>("stock addr is ", &stock);
    }
};

class BondDeal final : public DealStrategy<Bond> {
public:
    virtual void deal(Bond& bond) override {
        puts("------ BondDeal::deal");
        ptr_lambda_debug<const char*,Investment*>("bond addr is \t\t", &bond);       
    }
};

class RealEstateDeal final : public DealStrategy<RealEstate> {
public:
    virtual void deal(RealEstate& realEstate) override {
        puts("------ RealEstateDeal::deal");
        ptr_lambda_debug<const char*,Investment*>("realEstate addr is \t", &realEstate);
    }
};

class Investor {
protected:
    std::string name;
    std::string email;
public:
    Investor(const std::string& _name, const std::string& _email): name{_name}, email{_email} 
    {}
    Investor(const Investor&) = default; 
    virtual ~Investor() = default;

    // ...
    std::string getName() const { return name; }
    std::string getEmail() const { return email; }
};

class SystemAdmin final : public Investor {
public:
    SystemAdmin(const std::string& _name, const std::string& _email): Investor{_name, _email}
    {}
    // ...
};

class InvestorObserver final : public Observer<Stock> {
private:
    Investor investor;
public:
    InvestorObserver(const Investor& _investor): investor{_investor}
    {}
    virtual void update(Stock& stock) override {
        puts("------ InvestorObserver::update");
        ptr_lambda_debug<const char*,Investment*>("stock addr is ", &stock);
        ptr_lambda_debug<const char*,const std::string&>("name is ", investor.getName());
        ptr_lambda_debug<const char*,const std::string&>("email is ", investor.getEmail());
    }
};

class SystemAdminObserver final : public Observer<Stock> {
    SystemAdmin systemAdmin;
public:
    SystemAdminObserver(const SystemAdmin& _systemAdmin): systemAdmin{_systemAdmin}
    {}
    virtual void update(Stock& stock) override {
        puts("------ SystemAdminObserver::update");
        ptr_lambda_debug<const char*,Investment*>("stock addr is ", &stock);
        ptr_lambda_debug<const char*,const std::string&>("name is ", systemAdmin.getName());
        ptr_lambda_debug<const char*,const std::string&>("email is ", systemAdmin.getEmail());
    }
};

std::unique_ptr<Investment> stockFactory() {
    std::unique_ptr<DealStrategy<Stock>> dsB = std::make_unique<StockDealB>(StockDealB{});
    std::unique_ptr<DealStrategy<Stock>> dsA = std::make_unique<StockDealA>(StockDealA{dsB});

    std::unique_ptr<DealStrategy<Stock>> dealStrategy = std::make_unique<StockDeal>(StockDeal{dsA});
    std::unique_ptr<Observer<Stock>> investorObserver = std::make_unique<InvestorObserver>(InvestorObserver{Investor{"Jack","jack@loki.org"}});
    std::unique_ptr<Observer<Stock>> systemAdminObserver = std::make_unique<SystemAdminObserver>(SystemAdminObserver{SystemAdmin{"Admin","admin@loki.org"}});

    Stock stock{dealStrategy};
    stock.attach(investorObserver);
    stock.attach(systemAdminObserver);

    return std::make_unique<Stock>(std::move(stock));
}
std::unique_ptr<Investment> bondFactory() {
    std::unique_ptr<DealStrategy<Bond>> dealStrategy = std::make_unique<BondDeal>(BondDeal{});
    return std::make_unique<Bond>(Bond{dealStrategy});
}
std::unique_ptr<Investment> realEstateFactory() {
    std::unique_ptr<DealStrategy<RealEstate>> dealStrategy = std::make_unique<RealEstateDeal>(RealEstateDeal{});
    return std::make_unique<RealEstate>(RealEstate{dealStrategy});
}
std::unique_ptr<Investment> investmentFactory(InvestmentType type) {
    switch(type) {
        case InvestmentType::STOCK: return stockFactory();
        case InvestmentType::BOND:  return bondFactory();
        case InvestmentType::REAL_ESTATE: return realEstateFactory();
        default: throw std::runtime_error("No match InvestmentType. @see investmentFactory()");
    }
}

int test_investmentFactory() {
    puts("=== test_investmentFactory");
    try {
        std::unique_ptr<Investment> ic_1 = investmentFactory(InvestmentType::STOCK);
        ptr_lambda_debug<const char*,Investment*>("Investment* ...(Stock) is ", ic_1.get());
        ic_1.get()->deal();
        ic_1.release();
        ptr_lambda_debug<const char*,Investment*>("After release Investment* ...(Stock) is ", ic_1.get());

        ic_1 = investmentFactory(InvestmentType::BOND);
        ptr_lambda_debug<const char*,Investment*>("Investment* ...(Bond) is ", ic_1.get());
        ic_1.get()->deal();
        ic_1.release();

        ic_1 = investmentFactory(InvestmentType::REAL_ESTATE);
        ptr_lambda_debug<const char*,Investment*>("Investment* ...(RealEstate) is ", ic_1.get());
        ic_1.get()->deal();
        ic_1.release();

        ic_1 = investmentFactory(InvestmentType::FUTURES_CONTRACT);     // これは未実装の取引（取引自体は仕様として定義されている）。
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int main(void) {
    puts("START 項目 21 ：new の直接使用よりも std::make_unique や std::make_shared を優先する ===");
    if(0.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_debug());
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_my_make_unique());
        sample();
    }
    if(2.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_investmentFactory());
    }
    puts("=== 項目 21 ：new の直接使用よりも std::make_unique や std::make_shared を優先する END");
    return 0;
}