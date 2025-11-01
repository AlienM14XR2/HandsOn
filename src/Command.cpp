/**
 * Command パターン
 * 
 * これも復習だけど、未だに利用したことないな。
 * 各クラスが粗結合であるため拡張性が高く維持される。
 * 
 * - Invoker : 起動者
 * - Command : コマンドインタフェース
 * - Concrete Command : コマンドの具象化
 * - Receiver : 受信機、コマンドを受けるもの、具体的な個別処理、
 * 
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror -I../inc Command.cpp -o ../bin/Command
 */
#include <iostream>
#include <cassert>

template <class M, class D>
void (*ptr_print_debug)(M, D) = [](const auto message, const auto debug) -> void
{
    std::cout << "DEBUG: " << message << '\t' << debug << std::endl;
};

template <class Error>
concept ErrReasonable = requires(Error& e) {
    e.what();
};
template <class Error>
requires ErrReasonable<Error>
void (*ptr_print_error)(Error) = [](const auto e) -> void {
    std::cerr << "ERROR: " << e.what() << std::endl;
};


/**
 * Command インタフェース
 * 
 */
class Command
{
public:
    virtual ~Command() = default;
    virtual void execute() const = 0;
};

/**
 * 照明クラス
 * 
 * Receiver に相当するもの
 */
class Light
{
public:
    // action 1
    void on()
    {
        puts("Light on.");
    }
    // action 2
    void off()
    {
        puts("Light off.");
    }
};

class LightOnCommand final : public Command
{
private:
    Light& receiver;
public:
    LightOnCommand(Light& _receiver) : receiver{_receiver}
    {}
    virtual void execute() const
    {
        receiver.on();
    }
};

class LightOffCommand final : public Command
{
private:
    Light& receiver;
public:
    LightOffCommand(Light& _receiver) : receiver{_receiver}
    {}
    virtual void execute() const
    {
        receiver.off();
    }
};

// Invoker に相当する。
// Command インタフェースは意識するが、その詳細な中身には感知しない。
class Controller
{
private:
    Command* command = nullptr;
public:
    void setCommand(Command* _command)
    {
        command = _command;
    }
    void pressButton() const
    {
        if(command) {
            command->execute();
        }
    }
};

// client に相当
int test_command_light()
{
    puts("------ test_command_light");
    try {
        Light light;
        LightOnCommand lightOnCommand(light);   
        LightOffCommand lightOffCommand(light);
        Controller controller;

        controller.setCommand(&lightOnCommand);
        controller.pressButton();
        controller.setCommand(&lightOffCommand);
        controller.pressButton();
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}


int main(void) {
    puts("START main ===");
    if(1) {
        int result = -1;
        ptr_print_debug<const char*, int&>("Play and Result ... test_command_light", result = test_command_light());
        assert(result == 0);
    }
    puts("=== main END");
    return EXIT_SUCCESS;
}