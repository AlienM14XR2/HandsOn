/**
 * 7 章 Bridge パターン、Prototype パターン、External Polymorphism パターン
 * 
 * ガイドライン 30 ：コピー演算を抽象化するには Prototype パターン
 * 
 * 何か具体的なものは判っていない状態、C++ で言えば、具象クラスは判らずに抽象クラスのみ判っている状態で
 * Foo*（Foo は抽象クラス） をコピーするための解として、古典的 GoF デザインパターンの Prototype パターン
 * がある。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_7_prototype.cpp -o ../bin/main
*/
#include <iostream>

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
    // .. more animal-specific functions
};

int main(void) {
    puts("START Prototype パターン ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    puts("=== Prototype パターン END");
    return 0;
}