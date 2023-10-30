/**
 * 2 章 Observer パターン
 * 
 * 直訳で観察者を意味する。
 * 
 * パブリッシャ ＋ サブスクライバ ＝ Observer パターン
 * 
 * Observer パターンでは、あるオブジェクトの状態が変化すると、そのオブジェクトに依存している
 * すべてのオブジェクトに自動的に通知され更新されるようにするという、オブジェクト間の１ 対 多
 * の依存関係が定義されている。
 * 
 * Subject インタフェース
 * このSubject インタフェースを利用して、オブジェクトはオブザーバとしての登録やオブザーバからの削除を行う。
 * - registerObserver()
 * - removeObserver()
 * - notifyObserers()
 * 
 * ConcreteSubject
 * 具象サブジェクトは必ずSubject インタフェースを実装する。具象サブジェクトは登録メソッドと削除メソッドの他に、
 * 状態が変わるたびに現在のすべてのオブザーバを更新する notifiObservers() メソッドを実装する。
 * - registerObserver() {...}
 * - removeObserver() {...}
 * - notifyObserers() {...}
 * - getState()
 * - setState()
 * 
 * Observer インタフェース
 * オブザーバになる可能性のあるオブジェクトはすべて、Observer インタフェースを実装する必要がある。
 * Observer インタフェースには、Subject の状態が変わると呼び出される update() というメソッドが１つだけある。
 * - update() 
 * 
 * ConcreteObserver
 * 具象オブザーバは、Observer インタフェースを実装した任意のクラスです。オブザーバは更新情報を受け取るために
 * 具象サブジェクトに登録します。
 * 
 * 
 * ```
 * e.g. コンパイル
 * g++ -O3 -std=c++20 -pedantic-errors -Wall -Werror chapter_2_observer.cpp -o ../bin/main
 * ```
*/
#include <iostream>

using namespace std;

template <class M, class D>
void (*ptr_labmda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " + message + '\t' + debug << endl;
};

int main(void) {
    puts(" START ========= 2 章 Observer パターン");
    puts("2 章 Observer パターン ========= END");
    return 0;
}