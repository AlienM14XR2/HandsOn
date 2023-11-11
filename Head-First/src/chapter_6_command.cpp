/**
 * 6 章 Command パターン
 * 
 * e.g.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_6_command.cpp -o ../bin/main
 * 
 * 呼び出しのカプセル化
 * 
 * リモコン API をサンプルにする。
 * 7 つのスロットがあり、ON/OFF と最後に押されたものをUndo するボタンがある。
 * デバイスは任意で設定可能とする。
*/
#include <iostream>

using namespace std;

template<class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * Command インタフェース
*/
class Command {
public:
    virtual ~Command() {}
    virtual void execute() const = 0;
};

/**
 * 照明クラス
*/
class Light {
public:
    Light() {}
    Light(const Light& own) {*this = own;}
    ~Light() {
        ptr_lambda_debug<const char*,const int&>("DONE. Light Destructor.",0);
    }
    void on() {
        ptr_lambda_debug<const char*,const int&>("Light on",0);
    }
    void off() {
        ptr_lambda_debug<const char*,const int&>("Light off",0);
    }
};

/**
 * 照明をつけるコマンドを実装する。
*/
class LightCommand final : public virtual Command {
private:
    mutable Light light;
    LightCommand() {}
public:
    LightCommand(const Light& l) {
        light = l;
    }
    LightCommand(const LightCommand& own) {*this = own;}
    ~LightCommand() {
        ptr_lambda_debug<const char*,const int&>("DONE. LightCommand Destructor.",0);
    }
    
    virtual void execute() const override {
        light.on();
    }
};

/**
 * コマンドオブジェクトを使用する。
*/
class SimpleRemoteControl {
private:
    Command* slot = nullptr;
    SimpleRemoteControl():slot{nullptr} {}
public:
    SimpleRemoteControl(Command& cmd) {
        slot = &cmd;
    }
    SimpleRemoteControl(const SimpleRemoteControl& own) {
        *this = own;
    }
    ~SimpleRemoteControl() {
        ptr_lambda_debug<const char*,const int&>("DONE. SimpleRemoteControl Destructor.",0);
    }

    void buttonWasPressed() {
        slot->execute();
    }
};

/**
 * リモコンを使う簡単なテストを実施する。
*/
int test_SimpleRemoteControl() {
    puts("--- test_SimpleRemoteControl");
    try {
        Light* light = new Light();
        LightCommand lightCmd(*light);
        SimpleRemoteControl control(lightCmd);
        control.buttonWasPressed();
        delete light;
        return 0;
    } catch(exception& e) {
        cout << e.what() << endl;
        return -1;
    }
}

int main(void) {
    puts("START 6 章 Command パターン =========");
    if(0) {
        double pi = 3.14159;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    if(1.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_SimpleRemoteControl());
    }
    puts("========= 6 章 Command パターン END");
    return 0;
}