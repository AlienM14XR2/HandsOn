/**
  GoF Proxy パターン

  「代理人」、主人でもできることを「代理人」に任せるとな：）

  UML クラス図を言葉で説明する。

  https://ja.wikipedia.org/wiki/Proxy_%E3%83%91%E3%82%BF%E3%83%BC%E3%83%B3#/media/%E3%83%95%E3%82%A1%E3%82%A4%E3%83%AB:Proxy_pattern_diagram.svg

  GoF にフォーカスして気付いたことだが、意外と統一された定義ではないのかと思った。
  無論、書籍から得た情報ではなく全てネット情報なので鵜呑みにはできないのだが：）
  今回でいえば、Proxy クラスにRealSubject クラスが集約されていたり、いなかったりだ。
  あるクラスが集約されているとは、依存関係はなく、集約している側にされている側のオブジェクトがコンテナ管理になっていることを指す。

  では、集約されているものを言葉で説明してみる。

  Client クラス「顧客」このパターンの利用者と解釈した。
  Client はInterface である、Subject 基底抽象クラスを介して、機能を利用する。
  Client はSubject を集約している。

  Subject クラス「件名」、基底抽象クラス（Interface）。
    - doAction() をメンバ関数で宣言。
  
  RealSubject クラス「本件」はSubject の具象化クラス、派生クラス。
    RealSubject でしかできないことがある。
    - doAction() をメンバ関数で定義。

  Proxy クラス「代理人」はSubject の具象化クラス、派生クラス。
    RealSubject でも、できることを行う。
    - RealSubject をコンテナにメンバ変数として持つ。
    - doAction() をメンバ関数で定義。
  
  以上だね。
*/
#include <iostream>
#include <vector>
#include "stdio.h"

using namespace std;

template<class M,class D>
void (*ptr_lambda_debug)(M,D) = [](auto message,auto debug) -> void {
  cout << message << '\t' << debug <<  endl;
};

class Subject {
public:
  virtual ~Subject() {}
  virtual void doAction() const = 0;
};
class RealSubject final : public virtual Subject {
public:
  RealSubject() {}
  RealSubject(const RealSubject& own) {
    *this = own;
  }
  ~RealSubject() {}
  virtual void originalAction() {   // これは、ダウンキャストしてもダメかな。気になる、試してみたい：）
    cout << "originalAction ... RealSubject." << endl;
  }
  virtual void doAction() const override {
    cout << "doAction ... RealSubject." << endl;
  }
};
class Proxy final : public virtual Subject {
  vector<RealSubject*> subjects;
public:
  Proxy() {}
  Proxy(const Proxy& own) {
    *this = own;
    this->subjects = own.subjects;
  }
  ~Proxy() {}
  void addSubject(RealSubject* preal) {
    subjects.push_back(preal);
  }
  virtual void doAction() const override {
    for(RealSubject* s: subjects) {
      s->doAction();
    }
  }
};
// うん、Client 作ってないんだよね：）いらねっ
int test_Basic_Proxy() {
  cout << "------------------------------------- test_Basic_Proxy" << endl;
  RealSubject real;
  real.originalAction();
  Proxy proxy;
  proxy.addSubject(&real);
  proxy.doAction();
  // ここから、実験、本来はインタフェースで操作するんだよね。
  Subject* interface = static_cast<Subject*>(&real);
  cout << "interface->doAction() is " << endl;
  interface->doAction();
  interface = static_cast<Subject*>(&proxy);
  cout << "interface->doAction() is ... proxy" << endl;
  interface->doAction();
  // interface->originalAction(); アップキャストなので、流石にこれはコンパイルエラー。
  RealSubject* ptrReal = dynamic_cast<RealSubject*>(interface);
  cout << "ptrReal->doAction() is " << endl;
  ptrReal->doAction();  // Yes ダウンキャスト成功：）
  cout << "ptrReal->originalAction() is " << endl;
  ptrReal->originalAction();  // Yes 無論できる：）

  /*
    うん、やっぱり、インタフェースとポインタすげ。
    アップキャストとダウンキャストを使い分ければ、インタフェースポインタで何でも出来ちゃう：）
    Client を作らなかったのはここでテストしたことが即ち、Client だと思ったから：）
    少し、ちがうか：）
    うん、時間だ、今日はここまで。
  */

  return 0;
}

int main() {
    cout << "START GoF Proxy ===============" << endl;
    ptr_lambda_debug<const string&,const int&>("Here we go :)",0);
    if(1) {
      ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_Basic_Proxy());
    }
    cout << "=============== GoF Proxy END" << endl;
    return 0;
}