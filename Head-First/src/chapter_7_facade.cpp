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

/**
 * サブシステム群
*/

class Amplifier {

};

class Tuner {

};

class StreamingPlayer {

};

class Projector {

};

class Screen {

};

class PopcornPopper {
    
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