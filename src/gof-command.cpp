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
  ConcreteCommand はReceiver が隠蔽しているパラメータ初期化関数を呼び出し実行する。
  あるいは、そのパラメータの値を操作し実行する。
  カプセル化を推したい、今はそんな解釈かな。
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
    // int invoke() {
    //     cout << "... Probably wrong." << endl;
    //     return 0;
    // }
};
class Receiver final {
    void initForAction() {
        cout << "\t\tinit ... " << endl;
    }
public:
    ~Receiver() {}
    void action() {
        cout << "\t\taction ... " << endl;
        initForAction();
    }
};
class Command : public virtual Invoker {
protected:
    Receiver* receiver;
public:
    virtual void execute() const = 0;
    int invokeBefore() {
        cout << "invoke before ... " << endl;
        execute();
        return 0;
    }
    int invokeAfter() {
        execute();
        cout << "invoke after ... " << endl;
        return 0;
    }
    int invokeAround() {
        cout << "invoke around ... " << endl;
        execute();
        cout << "around invoke ... " << endl;
        return 0;
    }
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
        cout << "\texecute ... " << endl;
        receiver->action();
    }
};
int test_basic_command() {
    cout << "------------------------------ test_basic_command" << endl;
    Receiver receiver;
    // Receiver* precei = &receiver;
    ConcreteCommand concreteCommand(&receiver);
    concreteCommand.execute();

    Command* command = static_cast<Command*>(&concreteCommand);
    command->execute();
    command->invokeBefore();
    command->invokeAfter();
    command->invokeAround();

    // Invoker* invoker = static_cast<Invoker*>(command);
    // invoker->invoke();
    return 0;
}
/**
  ここまで単純化されたものを動かして、眺めてみるとやはりReceiver がパラメータをカプセル化
  している方が美しいと思うが、実際の使い勝手を考慮すると、ConcreteCommand が操作できても
  いいとも思える、これは厳格な安全性を取るか、開発効率を優先するのかというトレードオフかな。
  無論、オレはコードが美しくあってほしい。
*/
int main() {
    cout << "START GoF Command ===============" << endl;
    ptr_lambda_debug<const string&,const int&>("Here we go.",0);
    if(1) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ", test_basic_command());
    }
    cout << "=============== GoF Command END" << endl;
    return 0;
}