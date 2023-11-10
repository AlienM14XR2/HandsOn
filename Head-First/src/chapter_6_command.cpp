/**
 * 6 章 Command パターン
 * 
 * e.g.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_6_command.cpp -o ../bin/main
 * 
 * 呼び出しのカプセル化
 * 
 * リモコン API をサンプルにする。
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
    ~Light() {}
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
    ~LightCommand() {}
    
    virtual void execute() const override {
        light.on();
    }
};

int main(void) {
    puts("START 6 章 Command パターン =========");
    if(1) {
        double pi = 3.14159;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    puts("========= 6 章 Command パターン END");
    return 0;
}