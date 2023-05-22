/**
  GoF Interpreter 

  「解釈者・説明者」を意味する単語。
  「解析した結果」得られた手順に則った処理を実現するために最適なパターンです。 とな：）

  Wiki から
  Interpreter パターンの使用例
    - データベースに特化した問い合わせ言語（SQLなど）
    - 通信プロトコルを記述するために良く用いられる特化したコンピュータ言語
    - 特化言語を導入した汎用のコンピュータ言語
  
  UML クラス図  
  AbstractExpression 「抽象表現」クラスがある。基底抽象クラス、インタフェース。
    - メンバ関数で interpret() を宣言している。
  
  NonterminalExpression 「末端表現」クラスがある。AbsractExpression クラスの派生クラス。
  AbstractExpression（インタフェースポインタかな？）をコンテナに持つ、AbstractExpression を集約している。
    - メンバ変数に childExpressions を定義している。コンテナでインタフェースの集約。
    - メンバ関数で interpret() を宣言している。
  
  以上。
*/
#include <iostream>

using namespace std;


int main() {
    cout << "START GoF Interpreter ===============" << endl;
    cout << "=============== GoF Interpreter END" << endl;
    return 0;
}