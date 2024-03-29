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
#include <mutex>
#include <atomic>

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

int test_stack() {
    puts("=== test_stack");
    try {
        std::unique_ptr<Widget> wp1 = std::make_unique<Widget>(Widget(12));
        std::unique_ptr<Widget> wp2 = std::make_unique<Widget>(Widget(15));
        std::unique_ptr<Widget> wp3 = std::make_unique<Widget>(Widget(18));

        std::stack<Widget*> s;
        s.push(wp1.get());
        s.push(wp2.get());
        s.push(wp3.get());

        while(!s.empty()) {
            Widget* wp = s.top();
            s.pop();
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
 * 念のため、同期処理は行うこと。
*/

class NoPoolException final : std::exception {
public:
    const char* what() const noexcept override {
        return "No Pool Objects.";
    }
};

template <class T>
class ConnectionPool final {
public:
    ~ConnectionPool() {     // その役割が任意のポインタの Pool なので、解放は本クラスで行う必要がある。
        while(!q.empty()) {
            const T* pt = q.front();
            q.pop();
            delete pt;
        }
        puts("...... Done ConnectionPool Destructor.");
    }
    bool empty() {
        return q.empty();
    }
    void push(const T* pt) const {
        std::lock_guard<std::mutex> guard(m);
        q.push(pt);
    }
    const T* pop() const {
        std::lock_guard<std::mutex> guard(m);
        const T* ret = nullptr;
        if(!q.empty()) {
            ret = q.front();
            q.pop();
        }
        if(!ret) {
            throw std::runtime_error(NoPoolException().what()) ;
        }
        return ret;     // TODO nullptr の場合は、何らかの exception としたいが、やりすぎかな。
    }
private:
    mutable std::mutex m;
    mutable std::queue<const T*> q;
};

int test_ConnectionPool() {
    puts("=== test_ConnectionPool");
    try {
        Widget* wp1 = new Widget(21);
        Widget* wp2 = new Widget(24);
        Widget* wp3 = new Widget(27);

        ConnectionPool<Widget> cp;
        cp.push(wp1);
        cp.push(wp2);
        cp.push(wp3);

        const Widget* wp = cp.pop();
        ptr_lambda_debug<const char*, const int&>("value is ", wp->getValue()); 
        wp = cp.pop();
        ptr_lambda_debug<const char*, const int&>("value is ", wp->getValue()); 
        wp = cp.pop();
        ptr_lambda_debug<const char*, const int&>("value is ", wp->getValue()); 
        ptr_lambda_debug<const char*, const bool&>("cp is empty ? ", cp.empty()); 

        wp = cp.pop();          // これは nullptr
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

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
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_stack());
        assert(ret == 0);
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_ConnectionPool());
        assert(ret == 1);   // テスト関数内で明示的にエラーにしている。
    }
    puts("=== std::queue と std::stack   END");
    return 0;
}