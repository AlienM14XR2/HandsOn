/**
  Observer パターン

  はい、知りません：）なんだっけ？

  Observer パターンを学びます。Observer とは、英語で「観察者」を意味する単語です。
  Observer パターンとは、状態の変化を観察することを目的としたものですが、どちらかというと
  「観察」よりも「通知」に重点の置かれたものになっています。

  だそうです。

  Subject、 observerList をメンバ変数に持つ。Observer を集約している。
  addObserver メンバ関数、notifyObserver メンバ関数、done メンバ関数を持つ。
  Observer、 update メンバ関数を宣言したインタフェース。
  Subject Observer それぞれに派生クラスが複数存在する。

  そんな、クラス図。
  やっぱり、分からないから基本のクラスを作っていく。
*/
#include <iostream>

using namespace std;

int main() {
    cout << "START Observer ===============" << endl;
    cout << "=============== Observer END" << endl;
    return 0;
}