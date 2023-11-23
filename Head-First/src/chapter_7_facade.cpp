/**
 * Facade パターン 
 * 
 * インタフェースをより完結にする。
 * 
 * Decorator パターン ... インタフェースを変更せずに責務を追加する。
 * Adapter パターン ... あるインタフェースを別のインタフェースに変換（適合させる）する。
 * 
 * Apapter パターンと Facade パターンの違い、それはその目的にある。
 * Adapter パターンは、あるインタフェースをクライアントが求めるインタフェースに変換することであり、
 * Facade パターンは、サブシステムに対する単純化されたインタフェースをクライアントに提供することにある。
 * 
 * スイートホームシアター これがサンプルらしい：）
 * 
 * e.g. compile
 * g++ -O3 -std=c++20 -DDEBUG -pedantic-errors -Wall -Werror chapter_7_facade.cpp -o ../bin/main 
 * 
*/
#include <iostream>
#include <cassert>

using namespace std;

template<class M, class D>
void (*ptr_lambda_debug)(M, D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * サブシステム群
*/

class Amplifier {
public:
    Amplifier() {}
    Amplifier(const Amplifier& own) {*this = own;}
    ~Amplifier() {}
};

class Tuner {
public:
    Tuner() {}
    Tuner(const Tuner& own) {*this = own;}
    ~Tuner() {}
};

class StreamingPlayer {
public:
    StreamingPlayer() {}
    StreamingPlayer(const StreamingPlayer& own) {*this = own;}
    ~StreamingPlayer() {}
};

class Projector {
public:
    Projector() {}
    Projector(const Projector& own) {*this = own;}
    ~Projector() {}
};

class Screen {
public:
    Screen() {}
    Screen(const Screen& own) {*this = own;}
    ~Screen() {}
};

class PopcornPopper {
public:
    PopcornPopper() {}
    PopcornPopper(const PopcornPopper& own) {*this = own;}
    ~PopcornPopper() {}
};

/**
 * ホームシアターファサードを構築する
 * 
 * HomeTheaterFacade の構築を行う。
 * まず、コンポジションを使ってファサードがサブシステムのすべての構成要素にアクセス
 * できるようにする。
 * 
 * あぁ、構成要素が多いから、それらを最初に作る必要があるな、一旦すべてメモしてみる。
 * - Amplifier          増幅器、アンプ。
 * - Tuner
 * - StreamingPlayer
 * - Projector
 * - TheaterLights
 * - Screen
 * - PopcornPopper
*/
class HomeTheaterFacade {
    // TODO 実装
};

int main(void) {
    puts("=== START Facade パターン");
    puts("Facade パターン === END");
    return 0;
}