/**
 * 7 章 Bridge パターン、Prototype パターン、External Polymorphism パターン
 * 
 * ガイドライン 30 ：コピー演算を抽象化するには Prototype パターン
 * 
 * 何か具体的なものは判っていない状態、C++ で言えば、具象クラスは判らずに抽象クラスのみ判っている状態で
 * Foo*（Foo は抽象クラス） をコピーするための解として、古典的 GoF デザインパターンの Prototype パターン
 * がある。
 * 
 * 解明 Prototype パターン
 * Prototype パターンは GoF がまとめた、5 つある生成に関するパターンの 1 つで、抽象ソフトウェアエンティティのコピー作成の抽象化を
 * 主眼としています。
 * 
 * Prototype パターン
 * 目的：生成すべきオブジェクトの種類を原型となるインスタンスを使って明確にし、
 *      それをコピーすることで新たなオブジェクトの生成を行う。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_7_prototype.cpp -o ../bin/main
*/
#include <iostream>
#include <string>
#include <memory>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * 羊もどきの例：Animal のコピー
*/

class Animal {
public:
    virtual ~Animal() = default;
    virtual void makeSound() const = 0;
    virtual string& getName() const = 0;
    virtual std::unique_ptr<Animal> clone() const = 0;  // Prototype design pattern.
    /**
     * Modern C++ では手動クリーンアップ（ new と delete）は禁忌なので、戻り値は std::unique_ptr として返却する。
    */

    // .. more animal-specific functions
};

class Sheep final : public Animal {
private:
    mutable std::string name_;
public:
    explicit Sheep(std::string name): name_{std::move(name)} {}
    void makeSound() const override {
        puts("Baa");
    }
    string& getName() const override {
        return name_;
    }
    virtual std::unique_ptr<Animal> clone() const override {
        return std::make_unique<Sheep>(*this);      // Copy-construct a Sheep
    }
    /**
     * Sheep クラスでは、コピーコンストラクタとコピー代入演算子を、delete も隠蔽もしていない点に注目してください。
     * 特殊メンバ関数により Sheep インスタンスをコピーできる点は変わっていないのです。この点はまったく問題なく、
     * clone() とは、単にコピーを作成する方法を 1 つ（virtual なコピー演算を）増やしたに過ぎません。
    */
};

int test_Sheep() {
    puts("--- test_Sheep");
    try {
        std::unique_ptr<Animal> const dolly = std::make_unique<Sheep>("Dolly");
        ptr_lambda_debug<const char*,const string&>("dolly->getName() is ",dolly->getName());
        dolly->makeSound();

        std::unique_ptr<Animal> const dollyClone = dolly->clone();
        ptr_lambda_debug<const char*,const string&>("dollyClone->getName() is ",dollyClone->getName());
        dollyClone->makeSound();

        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

int main(void) {
    puts("START Prototype パターン ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_Sheep());
    }
    puts("=== Prototype パターン END");
    return 0;
}