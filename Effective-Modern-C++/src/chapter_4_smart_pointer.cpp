/**
 * 4 章 スマートポインタ
 * 
 * 著者が考える raw ポインタを愛せない理由。
 * 
 * 1. その宣言からでは、単一オブジェクトを指すのか、配列を指すのか判断できない。
 * 
 * 2. その宣言からでは、使用を終えた時に対象オブジェクトを破棄すべきか否か、すなわちポインタ
 *    がその指しているオブジェクトを『所有している』か否かを判断できない。
 * 
 * 3. 対象オブジェクトを破棄すべきと判断したとしても、delete するか、もしくは他の破棄方法を
 *    実行すべきか（そのポインタ専用の破棄関数など）、破棄方法を通知する手段がない。
 * 
 * 4. 破棄方法が delete だと分かったとしても、上記の理由 1 のように単一オブジェクトとして扱う
 *    のか（「delete」）、それとも配列として扱うのかを知る術がない（「delete []」）。 形態が
 *    一致しない破棄を実行するとその結果は未定義となってしまう。
 * 
 * 5. ポインタが対象オブジェクトを所有しており、破棄方法も判明したとしても、コードの全実行パス
 *    中で『正確に一度だけ』破棄するのは容易ではない（例外処理の実行パスも含め）。 実行パスを
 *    すべて把握できなければリソースの解放もれになり、逆に複数回破棄しては未定義動作となってしまう。
 * 
 * 6. ポインタが不正か否かを通知する術が、一般に存在しない。不正ポインタとは、オブジェクトを保存
 *    していないメモリを指すポインタであり、ポインタが指しているオブジェクトを破棄すると発生する。
 * 
 * C++11 には 4 種類のスマートポインタがあります。
 * std::auto_ptr
 * std::unique_ptr
 * std::shared_ptr
 * std::weak_ptr
 * いずれもダイナミックに割り当てたオブジェクトのライフサイクルの管理を支援するべく設計されたものです。
 * すなわち、適切な時に、（例外発生時も含め）、適切な方法でオブジェクトを確実に破棄し、リソースの解放
 * 漏れを防止します。
 * 
 * 項目 18 ：独占するリソースの管理には std::unique_ptr を用いる
 * 
 * std::unique_ptr は独占所有（排他的所有権）セマンティクスを備えます。非ヌルの std::unique_ptr はその
 * 指す対象を常に所有し、std::unique_ptr をムーブすると、所有権も元のポインタからムーブ先のポインタへ
 * 転送されます（元のポインタはヌルになる）。std::unique_ptr はコピーできません。std::unique_ptr は
 * 『ムーブ専用型』です。非ヌルの std::unique_ptr を破棄すると、std::unique_ptr が対象のリソースを破棄
 * します。デフォルトのリソース破棄では、std::unique_ptr が持つ raw ポインタに対する delete が実行されます。
 * 
 * 一般的な使われ方は、factory 関数の戻り値型です。基底クラスのポインタを利用する場合です（実質は各派生クラスのインスタンス）。
 * デフォルトでは、delete により破棄されますが、カスタムデリータを用いるよう、std::unique_ptr 作成時に指定可能です。
 * 
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_4_smart_pointer.cpp -o ../bin/main
*/
#include <iostream>
#include <memory>
#include <set>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](const auto message, const auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

template<class Error>
concept Reasonable = requires(Error& e) {
    e.what();
};
template<class Error>
requires Reasonable <Error>
void (*ptr_print_error)(Error) = [](const auto e) -> void {
    cout << "ERROR: " << e.what() << endl;
};

int test_debug() {
    puts("=== test_debug");
    try {
        auto pi = 3.141592;
        ptr_lambda_debug<const char*,const decltype(pi)&>("pi is ", pi);
        ptr_lambda_debug<const char*,const decltype(pi)*>("pi addr is ", &pi);
        throw runtime_error("It's test error.");
        return EXIT_SUCCESS;
    } catch(exception& e) {
        printf("e type is ... %s\n", typeid(e).name());
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * factory 関数の例を具体的に実装する演習でもやってみる。
 * Investment 投資を基底クラスとした次の派生クラスを作るものとする。
 * Stock 株式、Bond 債権、RealEstate 不動産。
 * 
 * - 取引に関して、Stragety パターンを実装してみる。 ... DONE
 * - 一部の取引（Stock）に Decorator パターンを実装してみる。... DONE
 * - 一部の取引（Stock）に Observer パターンを実装してみる、取引完了通知を Investor（投資家） に行う。 ... DONE
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
    std::set<std::unique_ptr<InvestorObserver>> observers;
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
    string name;
    string email;
public:
    Investor(const string& _name, const string& _email): name{_name}, email{_email} 
    {}
    Investor(const Investor&) = default; 
    virtual ~Investor() = default;

    // ...
    string getName() const { return name; }
    string getEmail() const { return email; }
};

class SystemAdmin final : public Investor {
public:
    SystemAdmin(const string& _name, const string& _email): Investor{_name, _email}
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
        ptr_lambda_debug<const char*,const string&>("name is ", investor.getName());
        ptr_lambda_debug<const char*,const string&>("email is ", investor.getEmail());
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
        ptr_lambda_debug<const char*,const string&>("name is ", systemAdmin.getName());
        ptr_lambda_debug<const char*,const string&>("email is ", systemAdmin.getEmail());
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
        default: throw runtime_error("No match InvestmentType. @see investmentFactory()");
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
    } catch(exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * スレッドセーフや同期等の問題は正直無視した設計と実装だが、ひとまずこれが今の私の理解と実力だろう。
 * 一年前よりは大分まともで Modern な C++ になったと思う。
 * 時間をみてこの点を std::mutex あるいは、Thread Promise Future？ を使って再設計してみたい。
 * 以降はまた、書籍に戻る。
*/

/**
 * カスタムデリータ
*/
auto deleteInvestment = [](Investment* pInventment) {
    // makeLogEntry(pInventment);       // ログ出力、書籍でも具体的な実装は示していないし、本題とは関係ないが、こんな利用が考えられるという一例と解釈した。
    delete pInventment;     // カスタムデリータなのだから、この一行は必須。
};

int main(void) {
    puts("START 項目 18 ：独占するリソースの管理には std::unique_ptr を用いる ===");
    if(0.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_debug());
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ...", test_investmentFactory());
    }
    puts("=== 4 章 項目 18 ：独占するリソースの管理には std::unique_ptr を用いる END");
    return 0;
}
