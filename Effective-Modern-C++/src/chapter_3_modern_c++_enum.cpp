/**
 * 3 章 現代の C++ への移行
 * 
 * 項目 10 ：enum にはスコープを設ける
 * 
 * e.g.) compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_3_modern_c++_enum.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](const auto message, const auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

enum struct Color {
    black,
    white,
    red,
};

void sample(const Color& color) {
    puts("--- sample");

    switch (color) {
        case Color::black: 
            puts("------ black");
            break;
        case Color::white: 
            puts("------ white");
            break;
        case Color::red:
            puts("------ red");
           break;
    }
}

int main(void) {
    puts("START 項目 10 ：enum にはスコープを設ける ===");
    if(0.01) {
        auto pi = 3.141592;
        ptr_lambda_debug<const char*,const decltype(pi)&>("pi is ", pi);
    }
    if(1.00) {
        // Color black = Color::black;
        auto white = Color::white;
        sample(white);
    }
    puts("=== 項目 10 ：enum にはスコープを設ける END");
    return 0;
}