/**
 * C++ JSON ライブラリ動作確認
 * 
 * @see https://qiita.com/yohm/items/0f389ba5c5de4e2df9cf
 * 
 * nlohmann-json
 * ```
 * sudo apt install nlohmann-json3-dev
 * ```
 * /usr/include/nlohmann
 * 上記ディレクトリ以下に必要なヘッダファイルがある。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror json.cpp -o ../bin/main
*/
#include <iostream>
#include <cassert>
#include <nlohmann/json.hpp>

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](const auto message, const auto debug) -> void {
    std::cout << "DEBUG: " << message << '\t' << debug << std::endl;
};

template <class Error>
concept ErrReasonable = requires(Error& e) {
    e.what();
};
template <class Error>
requires ErrReasonable<Error>
void (*ptr_print_error)(Error) = [](const auto e) -> void {
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


using json = nlohmann::json;

void sample_1() {
    puts("=== sample_1");
    json j;
    j["pi"] = 3.141;
    j["happy"] = true;
    j["name"] = "Niels";
    j["nothing"] = nullptr;
    j["answer"]["everything"] = 42;  // 存在しないキーを指定するとobjectが構築される
    j["list"] = { 1, 0, 2 };         // [1,0,2]
    j["object"] = { {"currency", "USD"}, {"value", 42.99} };  // {"currentcy": "USD", "value": 42.99}
    std::cout << j << std::endl;  // coutに渡せば出力できる。
}

void sample_2() {
    puts("=== sample_2");
    json j2 = {
        {"pi", 3.141},
        {"happy", true},
        {"name", "Niels"},
        {"nothing", nullptr},
        {"answer", {
        {"everything", 42}
                }
        },
        {"list", {1, 0, 2}},
        {"object", {
        {"currency", "USD"},
        {"value", 42.99}
                }
        }
    };
    std::cout << j2 << std::endl;  // coutに渡せば出力できる。
}

void sample_parse_1() {
    puts("=== sample_parse_1");
    json j = R"({ "happy": true, "pi": 3.141 })"_json;
    /**
     * ここで_jsonという見慣れないサフィックスがあるが、これはC++11で導入されたユーザー定義リテラルというもの。
     * ちなみにプレフィックスのRはC++11で導入された生文字列リテラル。"などの文字をエスケープする必要がなくなる。
    */
    for(auto v: j) {
        ptr_lambda_debug<const char*, const decltype(v)&>("v is ", v);
    }

    auto happy = j.at("happy");
    auto pi = j.at("pi");
    ptr_lambda_debug<const char*, const decltype(happy)&>("happy is ", happy);
    ptr_lambda_debug<const char*, const decltype(pi)&>("pi is ", pi);
}

void sample_parse_2() {
    puts("=== sample_parse_2");
    std::string s = R"({ "happy": true, "pi": 3.1411592 } )";
    json j = json::parse(s);
    for(auto v: j) {
        ptr_lambda_debug<const char*, const decltype(v)&>("v is ", v);
    }

    auto happy = j.at("happy");
    auto pi = j.at("pi");
    ptr_lambda_debug<const char*, const decltype(happy)&>("happy is ", happy);
    ptr_lambda_debug<const char*, const decltype(pi)&>("pi is ", pi);

}

int main(void) {
    puts("=== START C++ JSON ライブラリ動作確認");
    if(0.01) {
        auto ret = 0;
        ptr_lambda_debug<const char*, const decltype(ret)&>("Play and Result ... ", ret = test_debug_and_error());
        assert(ret == 1);
    }
    if(1.00) {
        sample_1();
        sample_2();
        sample_parse_1();
        sample_parse_2();
    }
    puts("C++ JSON ライブラリ動作確認   END ===");
}
