/**
 * 8 章 Template Method パターン
 * 
 * アルゴリズムのカプセル化
 * 
 * コーヒーショップのコーヒーの作り方、紅茶の作り方をサンプルにする。
 * 
 * e.g. compile
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_8_template_method.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template<class M, class D>
void (*ptr_lambda_debug)(M, D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * Coffee と Tea を抽象化しよう
*/

/**
 * カフェイン飲料全般に関わるインタフェース。
*/
class CafeineBeverage {
public:
    virtual void prepareRecipe() const = 0;
    void boilWater() {
        puts("お湯を沸かす");
    }
    void pourInCup() {
        puts("カップに注ぐ");
    }
};

int main(void) {
    puts("START 8 章 Template Method パターン ===");
    if(0.01) {
        double pi = 3.14159;
        ptr_lambda_debug<const char*,const double&>("pi si ",pi);
    }
    puts("=== 8 章 Template Method パターン END");
    return 0;
}