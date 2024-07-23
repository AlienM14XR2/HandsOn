/**
 * study_0723
 * 最近 C++ 書いてなくて色々忘れている気がしている：）
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror study_0723.cpp -o ../bin/main
 */

#include <iostream>
#include <cassert>

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](auto message, auto debug) -> void {
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
 * 何らかの映像から特定のモノを識別するクラスについて考えてみる。
 * 入出力は考慮しない、あくまでも概念のみ。動的なモノと静的なモノに分類できる。
 * 
 * Strategy パターン、Decorator パターンでできないかな？
 * 
 * Decorator パターンではアプリで扱えるオブジェクトの最小粒度まで掘り下げて調べることが
 * できると思う。
 */

class Object {
public:
    virtual std::string toString() const = 0;
};

class Identification : public Object {
public:
    virtual void identify() const = 0;
private:
};

class TrafficLight final : public Identification {
public:
    std::string toString() const override {
        std::string name = "traffic light object";
        return name;
    }
    void identify() const override {
        puts("--- TrafficLight::identify");
        puts(toString().c_str());
    }

};

class SmallSId final : public Identification {
public:
    std::string toString() const override {
        std::string name = "small static object";
        return name;
    }
    void identify() const override {
        puts("--- SmallSId::identify");
        // 信号機と識別できた場合。
        if(1) {
            TrafficLight tl;
            tl.identify();
        }
    }
};

class StaticId final : public Identification {
public:
    std::string toString() const override {
        std::string name = "static object";
        return name;
    }
    void identify() const override {
        puts("--- StaticId::identify");
        // 大きさから Big, Medium, small に分類する。
        // 例えば small と 識別出来た場合はそのオブジェクトを生成する。
        if(1) {
            SmallSId small;
            small.identify();
        }
    }
};

/**
 * 各識別クラスの identify() メンバ関数をどれだけ賢くできるかと
 * その結果得られるオブジェクトの粒度をどこまで細かくできるかと
 * いうことに尽きるのかな。
 */

class DynamicId {
};


int test_001() {
    puts("--- test_001");
    try {
        StaticId s;
        ptr_lambda_debug<const char*, const std::string&>("name: ", s.toString());
        s.identify();
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        // std::cout << "Error: " << e.what() << std::endl;
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int main(void) {
    puts("START study_0723 ===");
    if(0.10) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*, const decltype(pi)&>("pi is ", pi);
        printf("pi addr = \t\t%p\n", (void*)&pi);
        ptr_lambda_debug<const char*, const void*>("pi addr is ", &pi);
    }
    if(1.00) {
        auto ret = 0;
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_001());
        assert(ret == 0);   
    }
    puts("===   study_0723 END");
}