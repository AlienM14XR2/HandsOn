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

  うん、Receiver がパラメータであり、それらをカプセル化している。
  ConcreteCommand がそのパラメータの値を操作し実行する。
  今はそんな解釈かな。
*/
#include <iostream>

using namespace std;

template<class M,class D>
void (*ptr_lambda_debug)(M,D) = [](auto message,auto debug) -> void {
    cout << message << '\t' << debug << endl;
};

class Invoker {
public:
    virtual ~Invoker() {}
};
class Receiver final {
public:
    ~Receiver() {}
    void action() {
        cout << "action ... " << endl;
    }
};
class Command : public virtual Invoker {
protected:
    Receiver* receiver;
public:
    virtual void execute() const = 0;
};
class ConcreteCommand final : public virtual Command {
    ConcreteCommand() {
        receiver = nullptr;
    }
public:
    ConcreteCommand(Receiver* rece) {
        receiver = rece;
    }
    ConcreteCommand(const ConcreteCommand& own) {
        *this = own;
        this->receiver = own.receiver;
    }
    ~ConcreteCommand() {}
    virtual void execute() const override {
        receiver->action();
    }
};
int test_basic_command() {
    cout << "------------------------------ test_basic_command" << endl;
    Receiver receiver;
    // Receiver* precei = &receiver;
    ConcreteCommand command(&receiver);
    command.execute();    
    return 0;
}
int main() {
    cout << "START GoF Command ===============" << endl;
    ptr_lambda_debug<const string&,const int&>("Here we go.",0);
    if(1) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ", test_basic_command());
    }
    cout << "=============== GoF Command END" << endl;
    return 0;
}