/**
 * C++ 標準クラスの std::queue と std::stack の動作確認を行う。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror queue_stack.cpp -o ../bin/main
*/
#include <iostream>
#include <cassert>
#include <queue>
#include <stack>
#include <memory>

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](const auto message, const auto debug) -> void {
    std::cout << "DEBUG: " << message << '\t' << debug << std::endl;
};

template <class Err>
concept ErrReasonable = requires(Err& e) {
    e.what();
};
template <class Err>
requires ErrReasonable<Err>
void (*ptr_print_error)(Err) = [](const auto e) -> void {
    std::cout << "ERROR: " << e.what() << std::endl;
};

int test_debug_and_error() {
    puts("=== test_debug_and_error");
    try {
        // ptr_print_now();
        auto pi = 3.141592;
        ptr_lambda_debug<const char*,const decltype(pi)&>("pi is ", pi);
        ptr_lambda_debug<const char*,const decltype(pi)*>("pi addr is ", &pi);
        throw std::runtime_error("It's test error.");
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        printf("e type is ... %s\n", typeid(e).name());
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * 本編はここから。
*/

class Widget {
public:
    Widget(const int& _value): value(_value) 
    {}
    int getValue() const { return value; }
private:
    int value;
};

int test_queue() {
    puts("=== test_queue");
    try {
        std::unique_ptr<Widget> wp1 = std::make_unique<Widget>(Widget(3));
        ptr_lambda_debug<const char*, Widget*>("wp1 addr is ", wp1.get());
        std::unique_ptr<Widget> wp2 = std::make_unique<Widget>(Widget(6));
        std::unique_ptr<Widget> wp3 = std::make_unique<Widget>(Widget(9));

        std::queue<Widget*> q;
        q.push(wp1.get());
        q.push(wp2.get());
        q.push(wp3.get());

        while(!q.empty()) {
            Widget* wp  = q.front();
            q.pop();
            ptr_lambda_debug<const char*, const int&>("value is ", wp->getValue());
        }
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}


/**
 * 何らかのオブジェクトを Pool する仕組みは、標準の std::queue や std::stack でよさそう。 
*/

int main(void) {
    puts("START std::queue と std::stack ===");
    if(0.01) {
        auto ret = 0;
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_debug_and_error());
        assert(ret == 1);   // テスト関数内で明示的にエラーにしているため。
    }
    if(1.00) {
        auto ret = 0;
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_queue());
        assert(ret == 0);
    }
    puts("=== std::queue と std::stack   END");
    return 0;
}