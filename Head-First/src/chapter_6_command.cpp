/**
 * 6 章 Command パターン
 * 
 * e.g.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_6_command.cpp -o ../bin/main
 * 
 * 呼び出しのカプセル化
 * 
 * Command パターンは、リクエストをオブジェクトとしてカプセル化し、その結果、他のオブジェクトを
 * 様々なリクエスト、キュー、またはログリクエストでパラメータ化でき、アンドゥ可能な操作もサポートする。
 * 
 * リモコン API をサンプルにする。
 * 7 つのスロットがあり、ON/OFF と最後に押されたものをUndo するボタンがある。
 * デバイスは任意で設定可能とする。
*/
#include <iostream>
#include <vector>

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
    virtual void undo() const = 0;
};

/**
 * 未設定のスロットを表現する、何もしないコマンドクラス
*/
class NoCommand final : public virtual Command {
public:
    ~NoCommand() {
        ptr_lambda_debug<const char*,const int&>("DONE. NoCommand Destructor.",0);
    }
    virtual void execute() const override {
        ptr_lambda_debug<const char*,const int&>("none.",0);
    }
    virtual void undo() const override {
        ptr_lambda_debug<const char*,const int&>("none.",0);
    }
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
    virtual void undo() const override {
        light.off();
    }
};
/**
 * 照明を消すコマンドを実装する。
*/
class LightOffCommand final : public virtual Command {
private: 
    mutable Light light;
    LightOffCommand() {}
public:
    LightOffCommand(const Light& l) {light = l;}
    LightOffCommand(const LightOffCommand& own) {*this = own;}
    ~LightOffCommand() {}

    virtual void execute() const override {
        light.off();
    }
    virtual void undo() const override {
        light.on();
    }
};

/**
 * ガレージドア
*/
class GarageDoor {
public:
    GarageDoor() {}
    GarageDoor(const GarageDoor& own) {*this = own;}
    ~GarageDoor() {
        ptr_lambda_debug<const char*,const int>("DONE. GarageDoor Destructor.",0);
    }
    void up() {
        puts("Open GarageDoor.");
    }
    void down() {
        puts("Close GarageDoor.");
    }
};

/**
 * ガレージドア・オープンコマンド
*/
class GarageDoorOpenCommand final : public virtual Command {
private:
    mutable GarageDoor garageDoor;
    GarageDoorOpenCommand() {}
public:
    GarageDoorOpenCommand(const GarageDoor& gd) {
        garageDoor = gd;
    }
    GarageDoorOpenCommand(const GarageDoorOpenCommand& own) {*this = own;}
    ~GarageDoorOpenCommand() {}

    virtual void execute() const override {
        garageDoor.up();
    }
    virtual void undo() const override {
        garageDoor.down();
    }
};
/**
 * ガレージドア・クローズコマンド
*/
class GarageDoorCloseCommand final : public virtual Command {
private:
    mutable GarageDoor garageDoor;
    GarageDoorCloseCommand() {}
public:
    GarageDoorCloseCommand(const GarageDoor& gd) {garageDoor = gd;}
    GarageDoorCloseCommand(const GarageDoorCloseCommand& own) {*this = own;}
    ~GarageDoorCloseCommand() {}

    virtual void execute() const override {
        garageDoor.down();
    }
    virtual void undo() const override {
        garageDoor.up();
    }
};

/**
 * ステレオ
*/
class Stereo {
protected:
    unsigned int volume = 0;
public:
    Stereo() {
        volume = 18;
    }
    Stereo(const unsigned int& vol) {
        volume = vol;
    }
    Stereo(const Stereo& own) {*this = own;}
    ~Stereo() {
        ptr_lambda_debug<const char*,const int>("DONE. Stereo Destructor.",0);
    }

    void on() {
        ptr_lambda_debug<const char*,const int>("Stereo ON.",0);
    }
    void off() {
        ptr_lambda_debug<const char*,const int>("Stereo OFF.",0);
    }
    void setVolume(const unsigned int& vol) {
        volume = vol;
    }
    void setMusic() {   // これは事前に音楽オブジェクトを受け付ける必要があるはず。
        ptr_lambda_debug<const char*,const int>("Stereo start music.",0);
    }

};

class StereoOnCommand final : public virtual Command {
private:
    mutable Stereo stereo;
    StereoOnCommand() {}
public:
    StereoOnCommand(const Stereo& s) {stereo = s;}
    StereoOnCommand(const StereoOnCommand& own) {*this = own;}
    ~StereoOnCommand() {}

    virtual void execute() const override {
        stereo.on();
        stereo.setMusic();
        stereo.setVolume(36);
    }
    virtual void undo() const override {
        stereo.off();
    }
};

class StereoOffCommand final : public virtual Command {
private:
    mutable Stereo stereo;
    StereoOffCommand() {}
public:
    StereoOffCommand(const Stereo& s) {stereo = s;}
    StereoOffCommand(const StereoOffCommand& own) {*this = own;}
    ~StereoOffCommand() {}

    virtual void execute() const override {
        stereo.off();
    }
    virtual void undo() const override {
        stereo.on();
        stereo.setMusic();
        stereo.setVolume(36);
    }
};

/**
 * リモコンクラス
 * コマンドインタフェースのポインタをメンバ変数に持つ。
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

    void setCommand(Command& cmd) {
        slot = &cmd;
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

        GarageDoor gd;
        GarageDoorOpenCommand gdOpenCmd(gd);
        control.setCommand(gdOpenCmd);
        control.buttonWasPressed();

        delete light;
        return 0;
    } catch(exception& e) {
        cout << e.what() << endl;
        return -1;
    }
}

/**
 * リモコンを実装する
 * リモコンクラス
*/
class RemoteControl final {
private:
    vector<Command*> onCommands;
    vector<Command*> offCommands;
    Command* noCommand = nullptr;
    Command* undoCommand = nullptr;     // アンドゥボタン用に、最後に実行したコマンドを格納する。

    bool isCorrectRange(const int& slot) {
        if(slot >= 0 && slot < 7) {
            return true;
        } else {
            ptr_lambda_debug<const char*,const int&>("slot is out of range.",0);
            return false;
        } 
    }
public:
    RemoteControl() {
        noCommand = new NoCommand();
        for(size_t i = 0; i < 7; i++) {
            onCommands.push_back(noCommand);
            offCommands.push_back(noCommand);
        }
    }
    RemoteControl(const RemoteControl& own) {*this = own;}
    ~RemoteControl() {
        delete noCommand;
    }

    void setCommand(const int& slot, Command* onCmd, Command* offCmd) {
        // TODO vector の使い方の調査
        if(isCorrectRange(slot)) {
            onCommands[slot] = onCmd;
            offCommands[slot] = offCmd;
        }
    }
    void onButtonWasPushed(const int& slot) {
        if(isCorrectRange(slot)) {
            Command* cmd = onCommands.at(slot);
            cmd->execute();
        }
    }
    void offButtonWasPushed(const int& slot) {
        if(isCorrectRange(slot)) {
            Command* cmd = offCommands.at(slot);
            cmd->execute();
        }
    }
};

int test_RemoteControl() {
    puts("--- test_RemoteControl");
    try {
        RemoteControl control;
        control.onButtonWasPushed(-1);
        control.onButtonWasPushed(0);
        control.onButtonWasPushed(6);
        control.onButtonWasPushed(7);

        control.offButtonWasPushed(-1);
        control.offButtonWasPushed(0);
        control.offButtonWasPushed(6);
        control.offButtonWasPushed(7);
        return 0;
    } catch(exception& e) {
        cout << e.what() << endl;
        return -1;
    }
}

int test_RemoteControl_Light_On_Off() {
    puts("--- test_RemoteControl_Light_On_Off");
    try {
        RemoteControl control;
        Light light;
        LightCommand lightCmd(light);
        LightOffCommand lightOffCmd(light);
        control.setCommand(0,&lightCmd,&lightOffCmd);
        control.onButtonWasPushed(0);
        control.offButtonWasPushed(0);
        return 0;
    } catch(exception& e) {
        cout << e.what() << endl;
        return -1;
    }
}

int test_RemoteControl_GarageDoor_Open_Close() {
    puts("--- test_RemoteControl_GarageDoor_Open_Close");
    try {
        RemoteControl control;
        GarageDoor gd;
        GarageDoorOpenCommand openCmd(gd);
        GarageDoorCloseCommand closeCmd(gd);
        control.setCommand(1,&openCmd,&closeCmd);
        control.onButtonWasPushed(1);
        control.offButtonWasPushed(1);
        return 0;
    } catch(exception& e) {
        cout << e.what() << endl;
        return -1;
    }
}

int test_RemoteControl_Stereo_On_Off() {
    puts("--- test_RemoteControl_Stereo_On_Off");
    try {
        RemoteControl ctrl;
        Stereo stereo;
        StereoOnCommand onCmd(stereo);
        StereoOffCommand offCmd(stereo);
        ctrl.setCommand(0,&onCmd,&offCmd);
        ctrl.onButtonWasPushed(0);
        ctrl.offButtonWasPushed(0);
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
    if(1.02) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_RemoteControl());
    }
    if(1.03) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_RemoteControl_Light_On_Off());
    }
    if(1.04) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_RemoteControl_GarageDoor_Open_Close());
    }
    if(1.05) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_RemoteControl_Stereo_On_Off());
    }
    puts("========= 6 章 Command パターン END");
    return 0;
}