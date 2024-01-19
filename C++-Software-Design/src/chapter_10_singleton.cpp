/**
 * 10 章 Singleton パターン
 * 
 * 読者がすでに Singleton パターンに精通しており、自分なりの確固たる意見を持っているだろうとは承知しています。
 * Singleton パターンをアンチパターンとみなしており、本書が敢えて取り上げたのはどうしてだろうと疑問を感じる
 * かもしれません。
 * 
 * C++ 標準ライブラリに Singleton パターンに似たインスタンスがいつくかあると分かると大いに驚くかもしれません。
 * Singleton パターンとは何か、いつ動作するか、正しく付き合うにはどうするのが良いかについて解説しましょう。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_10_singleton.cpp -o ../bin/main
*/
#include <iostream>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

int main(void) {
    puts("START Singleton パターン ===");
    puts("=== Singleton パターン END");
    return 0;
}