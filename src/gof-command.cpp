/**
  GoF Command パターン

  複雑なメソッドの引数を表現するためのクラス、とそのパターン。
  一言でいうならそうだと思うが、違うのかな：）

  一般的なクラス図が思いのほか複雑だった。
  言葉で説明してみるが、分かるかな、UML なしで。

  Invoker クラス。
  Invoker に集約されたCommand クラス。
  Command を継承した、ConcreteCommnad クラス。
  ConcreteCommnad に集約された、Receiver クラス。
  そして、ConcreteCommnad を利用するClinet クラス。

  最後のClient はどうでもいいが、いずれにせよ、ConcreteCommnad 
  を操作することになると。

  イメージがわかないな。
  ベースをそのまま書き起こしてみようか。
  なんか、眠くなってきた：）今日は５時起きだったか。
*/
#include <iostream>

using namespace std;

template<class M,class D>
void (*ptr_lambda_debug)(M,D) = [](auto message,auto debug) -> void {
    cout << message << '\t' << debug << endl;
};

int main() {
    cout << "START GoF Command ===============" << endl;
    ptr_lambda_debug<const string&,const int&>("Here we go.",0);
    cout << "=============== GoF Command END" << endl;
    return 0;
}