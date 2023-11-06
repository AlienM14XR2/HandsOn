/**
 * 3 章 Decorator
 * 
 * g++ -O3 -std=c++20 -pedantic-errors -Wall -Werror chapter_3_decorator.cpp -o ../bin/main
 * g++ -O3 -std=c++20 -pedantic-errors -Wall -Werror -D NDEBUG chapter_3_decorator.cpp -o ../bin/main
 * 
 * オブジェクトの装飾
 * 
 * 開放・閉鎖原則、既存コードの修正は行わず（閉鎖）に拡張性（開放）を持つ設計のこと。
 * 
 * Decorator パターンはオブジェクトに追加の責務を動的に付与する。デコレータは、サブクラス化の
 * 代替となる、柔軟な機能拡張手段を備えている。
 * 
 * コーヒーショップのレジシステムを想定したサンプルになる。
*/
#include <iostream>
#include <cassert>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * 飲み物の基底クラス
 * 飲み物の抽象クラス
*/
class Beverage {
protected:
    string description = "不明な飲み物";
public:
    virtual ~Beverage() {}
    string getDescription() noexcept {
        return description;
    }
    virtual double cost() const = 0;
};

/**
 * 調味料の基底クラス（Decorator）
 * 調味料の抽象クラス
 * Beverage のサブクラス
*/
class CondimentDecorator : public virtual Beverage {
protected:
    Beverage* beverage;
public:
    ~CondimentDecorator() {}
    Beverage* getBeverage() noexcept {
        return beverage;
    }
    void setBeverage(Beverage* b) noexcept {
        beverage = b;
    }
    virtual string getDescription() const = 0; 
};

/**
 * 飲み物、エスプレッソ クラス
*/
class Espresso final : public virtual Beverage {
public:
    Espresso() {
        description = "エスプレッソ";
    }
    Espresso(const Espresso& own) { *this = own; }
    ~Espresso() {}
    virtual double cost() const override {
        return 1.99;
    }
};

/**
 * 飲み物、ブレンド クラス
*/
class HouseBlend final : public virtual Beverage {
public:
    HouseBlend() {
        description = "ハウスブレンドコーヒー";
    }
    HouseBlend(const HouseBlend& own) {*this = own;}
    ~HouseBlend() {}
    virtual double cost() const override {
        return 0.89;
    }
};

/**
 * モカ
 * 調味料の具象クラス
*/
class Mocha final : public virtual CondimentDecorator {
public:
    Mocha(Beverage& b) {
        beverage = &b;
        printf("&b addr is %p\n",(void*)&b);
    }
    Mocha(const Mocha& own) {*this = own;}
    ~Mocha() {}

    string getDescription(CondimentDecorator& cd) {
        return cd.getDescription() + "、モカ";
    }
    string getDescription(string s) {
        return s + "、モカ";
    }
    virtual string getDescription() const override {
        printf("beverage addr is %p\n",(void*)beverage);
        string result = beverage->getDescription() + "、モカ";
        return result;
    }
    virtual double cost() const override {
        return beverage->cost() + 0.20;
    }
};

/**
 * 豆乳
 * 調味料の具象クラス
*/
class Soy final : public virtual CondimentDecorator {
public:
    Soy(Beverage& b) {
        beverage = &b;
    }
    Soy(const Soy& own) {*this = own;}
    ~Soy() {}

    virtual string getDescription() const override {
        string result = beverage->getDescription() + "、豆乳";
        return result;
    }
    virtual double cost() const override {
        return beverage->cost() + 0.60;
    }
};

/**
 * ホイップ
 * 調味料の具象クラス
*/
class Whip final : public virtual CondimentDecorator {
public:
    Whip(Beverage& b) {
        beverage = &b;
    }
    Whip(const Whip& own) {*this = own;}
    ~Whip() {}

    virtual string getDescription() const override {
        string result = beverage->getDescription() + "、ホイップ";
        return result;
    }
    virtual double cost() const override {
        return beverage->cost() + 1.20;
    }
};

/**
 * 飲物とそのデコレーションの動作確認
*/
int test_Beverages() {
    try {
        puts("--- test_Beverage");
        // エスプレッソ（豆乳）の動作確認
        Espresso espresso;
        Soy soy(espresso);
        ptr_lambda_debug<const string&, const string&>("soy(espresso) ... description is ",soy.getDescription());
        ptr_lambda_debug<const string&, const double&>("soy(espresso) ... cost is ",soy.cost());
        assert(soy.cost() == 2.59);    // 1.99 + 0.60

        // ハウスブレンド（モカ）の動作確認
        HouseBlend houseBlend;
        Mocha mocha(houseBlend);
        ptr_lambda_debug<const string&, const string&>("mocha(houseBlend) ... description is ",mocha.getDescription());
        ptr_lambda_debug<const string&, const double&>("mocha(houseBlend) ... cost is ",mocha.cost());
        assert(mocha.cost() == 1.09);

        // ハウスブレンド（ホイップ）の動作確認
        Whip whip(houseBlend);
        ptr_lambda_debug<const string&, const string&>("whip(houseBlend) ... description is ",whip.getDescription());
        ptr_lambda_debug<const string&, const double&>("whip(houseBlend) ... cost is ",whip.cost());
        assert(whip.cost() == 2.09);
        return 0;
    } catch(exception& e) {
        cout << e.what() << endl;
        return -1;
    }
}
int test_Beverages2() {
    try {
        puts("--- test_Beverages2");
        HouseBlend hb;
        Mocha mocha1 = Mocha(hb);
        Mocha mocha2 = Mocha(hb);   // これでは意図した結果にはならない、その理由をよく考えてみよう。Java とは違うのだよ、Java とは。
        Mocha mocha3 = Mocha(hb);
        mocha2.setBeverage(mocha1.getBeverage());   // これでも駄目だね：）
        ptr_lambda_debug<const string&, const string&>("mocha2 ... description is ",mocha2.getDescription(mocha1)); // これで狙い通りにはなったかな。
        ptr_lambda_debug<const string&, const double&>("mocha2 ... cost is ",mocha2.cost());
        ptr_lambda_debug<const string&, const string&>("mocha3 ... description is ",mocha3.getDescription(mocha2.getDescription(mocha1))); // これで狙い通りにはなったかな、なるほどねやりたいことがより明確にはなった、つまり文字列を渡して編集可能にしてやればいいのか。
        return 0;
    } catch(exception& e) {
        cout << e.what() << endl;
        return -1;
    }
}
int test_Beverages3() {
    puts("--- test_Beverages3");
    HouseBlend* hb = new HouseBlend();
    Mocha* mocha1 = new Mocha(*hb);
    // Java のサンプルと同じ意図したものを表現するには、調味料のコンストラクタを新たに追加する必要があるのかもしれない、これはTry and Error を行って確認するしかない。
    Mocha* mocha2 = new Mocha(*mocha1); // ポインタにしてもそのインスタンスの生成時はやはり 2 と同じなので、結果も同じ。
    ptr_lambda_debug<const string&, const string&>("mocha2 ... description is ",mocha2->getDescription());
    ptr_lambda_debug<const string&, const double&>("mocha2 ... cost is ",mocha2->cost());
    delete hb;
    delete mocha1;
    delete mocha2;
    return 0;
}

int main(void) {
    puts("START 3 章 Decorator =========");
    if(1) {
        double pi = 3.14159;
        ptr_lambda_debug<const char*, const double&>("pi is ",pi);
    }
    if(1.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_Beverages());
    }
    if(1.02) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_Beverages2());
    }
    if(1.03) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_Beverages3());
    }
    puts("========= END 3 章 Decorator ");
    return 0;
}