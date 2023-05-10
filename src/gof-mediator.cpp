/**
  GoF Mediator パターン

  仲介者、直訳すると。
  http://www.javacamp.org/designPattern/mediator.html

  Facade Command Observer の各パターンは簡単になめてきた。
  なので、多少はMediator に関しても理解が容易になっているはず。
  うん、もう眠くなってしまった。テンプレートを作成して寝る。

  いつものように、クラス図を文章で説明してみるが、分かりづらいよね：）

  Mediator クラス、インタフェース（抽象基底クラス）があり、メンバ関数に次のものを持つ。
   - addColleague(colleague: Colleague)
   - consultation(colleague1: Colleague, colleague2: Colleague)
  ConcreteMediator クラス、Mediator の派生クラスがある。
   - 無論、基底クラスの関数をオーバーライドする必要がある。  
   - メンバ変数にColleague のMap を持つ。

  Colleague クラス、インタフェース（抽象基底クラス）があり、メンバ関数に次のものを持つ。
   - needsAdvice を持つ。
   - setColleague(colleatue: Colleague) を持つ。
  ConcreteColleague クラス、Colleague の派生クラス。
   - メンバ変数にMediator オブジェクトを持つ。
*/
#include <iostream>

using namespace std;

template<class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << message << "\t" << debug << endl;
};

int main() {
    cout << "Start GoF Mediator ===============" << endl;
    ptr_lambda_debug<const string&,const int&>("Here we go.",0);
    cout << "=============== GoF Mediator END" << endl;
    return 0;
}