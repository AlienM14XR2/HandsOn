/**
  GoF Proxy パターン

  「代理人」、主人でもできることを「代理人」に任せるとな：）

  UML クラス図を言葉で説明する。

  https://ja.wikipedia.org/wiki/Proxy_%E3%83%91%E3%82%BF%E3%83%BC%E3%83%B3#/media/%E3%83%95%E3%82%A1%E3%82%A4%E3%83%AB:Proxy_pattern_diagram.svg

  GoF にフォーカスして気付いたことだが、意外と統一された定義ではないのかと思った。
  無論、書籍から得た情報ではなく全てネット情報なので鵜呑みはできないのだが：）
  今回でいえば、Proxy クラスにRealSubject クラスが集約されていたり、いなかったりだ。
  あるクラスが集約されているとは、依存関係はなく、集約している側にされている側のオブジェクトがコンテナ管理になっていることを指す。

  では、集約されているものを言葉で説明してみる。

  Client クラス「顧客」このパターンの利用者と解釈した。
  Client はInterface である、Subject 基底抽象クラスを介して、機能を利用する。
  Client はSubject を集約している。

  Subject クラス「件名」、基底抽象クラス（Interface）。
    - doAction() をメンバ関数で宣言。
  
  RealSubject クラス「本件」はSubject の具象化クラス、派生クラス。
    - doAction() をメンバ関数で定義。

  Proxy クラス「本件」はSubject の具象化クラス、派生クラス。
    - RealSubject をコンテナにメンバ変数として持つ。
    - doAction() をメンバ関数で定義。
  
  以上だね。
*/
#include <iostream>
#include <vector>
#include "stdio.h"

using namespace std;

int main() {
    cout << "START GoF Proxy ===============" << endl;
    cout << "=============== GoF Proxy END" << endl;
    return 0;
}