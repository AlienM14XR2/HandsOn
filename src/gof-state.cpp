/**
  GoF State パターン

  State パターンとは、 モノではなく、「状態」をクラスとして表現するパターンです。 

  https://ja.wikipedia.org/wiki/State_%E3%83%91%E3%82%BF%E3%83%BC%E3%83%B3#/media/%E3%83%95%E3%82%A1%E3%82%A4%E3%83%AB:State_Design_Pattern_UML_Class_Diagram.svg
  UML のクラス図をいつものように言葉で説明してみる。

  Context クラス（文脈）がある、このクラスはState クラスを集約している。
    - request() メンバ関数を持つ。（state.handle()の呼び出し） 
  State クラス（状態）がある、Context クラスに集約されている。
    - handle() メンバ関数を持つ。
  ConcreteStateA クラス、State の派生クラス、具象化。
  ConcreteStateB クラス、State の派生クラス、具象化。

  これは分かりやすい、State の具象化クラスで状態変化を起こしていく、別な言い方をすれば、これらで
  状態を管理していくのだね、このIF 以外からの状態変化は認めない。

*/
#include <iostream>

using namespace std;

int main() {
    cout << "START GoF State ===============" << endl;
    cout << "=============== GoF State END" << endl;
    return 0;
}