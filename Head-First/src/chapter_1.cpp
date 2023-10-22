/**
 * Head First デザインパターン
 * １章 デザインパターン入門
 * 
 * ```
 * g++ -O3 -std=c++20 -pedantic-errors -Wall -Werror chapter_1.cpp -o ../bin/main
 * ```
*/
#include <iostream>

using namespace std;

template< class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * Duck（カモ） 抽象クラス
*/
class Duck {
public:
    Duck() noexcept {}
    Duck(const Duck& own) {
        (*this) = own;
    }
    virtual ~Duck() {
    }
    virtual void display() const = 0;
    void swim() {
        puts("すべてのカモは浮きます、おとりのカモでも！");
    }
};

/**
 * マガモ クラス
 * カモの具象化クラス
*/
class MallardDuck : public virtual Duck {

};

int main(void) {
    puts(" ========= START １章 デザインパターン入門");
    if(1) {
        int n = 3;
        double pi = 3.141592;
        ptr_lambda_debug<const char*, const int&>("n is ",n);
        ptr_lambda_debug<const char*, const double&>("pi is ",pi);

    }
    puts("１章 デザインパターン入門 END =========");
}