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
  
  TerminalExpression 「末端表現」クラスがある。AbsractExpression クラスの派生クラス。
    - メンバ関数で interpret() を定義している。

  NonterminalExpression 「Non 末端表現」クラスがある。AbsractExpression クラスの派生クラス。
    AbstractExpression（インタフェースポインタかな？）をコンテナに持つ、AbstractExpression を集約している。
    - メンバ変数に childExpressions を定義している。コンテナでインタフェースの集約。
    - メンバ関数で interpret() を定義している。
  
  以上。
  うん、なんかさ、こんなパターン他にもあったような気がするのだか：）
  あぁ、にてるの分かった、Chain of Responsibility ? だったかな、スペルに自信がないが。
  でもこれも分かった。
*/
#include <iostream>
#include <vector>

using namespace std;

template<class M,class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
  cout << message << "\t" << debug << endl;
};

class AbstractExpression {
public:
  virtual void interpret() const = 0;
  virtual ~AbstractExpression() {}
};
class TerminalExpressionA final : public virtual AbstractExpression {
public:
  TerminalExpressionA() {}
  TerminalExpressionA(const TerminalExpressionA& own) {
    *this = own;
  }
  ~TerminalExpressionA() {}
  virtual void interpret() const override {
    cout << "My name is " << endl;
  }
};
class TerminalExpressionB final : public virtual AbstractExpression {
public:
  TerminalExpressionB() {}
  TerminalExpressionB(const TerminalExpressionB& own) {
    *this = own;
  }
  ~TerminalExpressionB() {}
  virtual void interpret() const override {
    cout << "Years Old " << endl;
  }
};
class NonterminalExpression final : public virtual AbstractExpression {
  mutable vector<AbstractExpression*> expressions;
  NonterminalExpression() {}
public:
  NonterminalExpression(AbstractExpression* name, AbstractExpression* old) {
    expressions.push_back(name);
    expressions.push_back(old);
  }
  NonterminalExpression(const NonterminalExpression& own) {
    *this = own;
    this->expressions = own.expressions;
  }
  ~NonterminalExpression() {}
  void name(const string& n) {
    expressions[0]->interpret();
    cout << n << endl;
  }
  void old(const int& y) {
    cout << y << endl;
    expressions[1]->interpret();
  }
  void interpret() const override {
    AbstractExpression** p = expressions.data();
    (*p)[0].interpret();
    cout << "Derek" << endl;
    cout << 33 << endl;
    (*p)[1].interpret();
  }
};
int test_Basic_Interpreter() {
  cout << "-------------------------------------- test_Basic_Interpreter" << endl;
  TerminalExpressionA a;
  TerminalExpressionB b;
  NonterminalExpression nt(static_cast<AbstractExpression*>(&a),static_cast<AbstractExpression*>(&b));
  nt.name("Jack");
  nt.old(30);
  nt.interpret();
  return 0;
}
int main() {
    cout << "START GoF Interpreter ===============" << endl;
    ptr_lambda_debug<const string&,const int&>("Yeah here we go.",0);
    if(1) {
      ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_Basic_Interpreter());
    }
    cout << "=============== GoF Interpreter END" << endl;
    return 0;
}