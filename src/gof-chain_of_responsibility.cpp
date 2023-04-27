/**
  GoF Chain of Responsibility パターン

  Chain of Responsibility うん、これも知らない、というか以降やるものはほぼ知らないものがほどんどか：）
  直訳すれば「責任の鎖」。うん、面白そうだ。

  Handler インタフェース（基底抽象クラス）が1つ。
  Handler の派生クラスConcreteHanler が複数個。
  request を行うClientが1つ。

  Handler インタフェースは next として、次の責任者、Handler オブジェクトをメンバ変数に持つ。
  Handler インタフェースはメンバ関数として request を持つ。

*/
#include <iostream>
#include <cassert>

using namespace std;

template<class M,class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << message << '\t' << endl;
};

int main() {
    cout << "START GoF Chain of Responsibility ===============" << endl;
    cout << "=============== GoF Chain of Responsibility END" << endl;
    return 0;
}