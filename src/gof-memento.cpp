/**
  GoF Memento パターン

  「記念品」、思い出のスナップショットね、これは分かりやすい。
  Virtual Box のスナップショットの概念がそのまんまかな、どうだろ。

  クラス図を言葉で説明してみる。（UML クラス図の理解が甘いので鵜呑みにするな。
  https://dofactory.com/net/memento-design-pattern

  - Caretaker クラスがあり、Memento クラスはそれに集約されている。
    - メンバ変数に C++ でいうコンテナオブジェクトなりを持つ（vector, map 等）。

  - Memento クラス
    - メンバ変数に state を持つ。
    - メンバ関数に getState()を持つ。
    - メンバ関数に setState()を持つ。 参考では実装となっているが、これが気にいらない。カプセル化、コンストラクタで生成し、一元性、一意性を担保した方が美しく感じる。
    
  - Originator クラス
    - Memento オブジェクトの生成を行うもの。
    - メンバ変数に state を持つ。
    - メンバ関数に createMemento()を持つ。
      - return new Memento(state)
    - メンバ関数に setMemento(in Memento)を持つ。
      - state = m.getState()
*/
#include <iostream>

using namespace std;

int main() {
    cout << "START GoF Memento ===============" << endl;
    cout << "=============== GoF Memento END" << endl;
    return 0;
}