/**
 * 3 章 現代の C++ への移行
 * 
 * 項目 17 ：自動的に生成される特殊メンバ関数を理解する
 * 
 * C++ の正式用語で特殊メンバ関数とは、C++ が自ら生成するメンバ関数を意味します。C++98 には特殊メンバ関数が 4 つあります。
 * デフォルトコンストラクタ、デストラクタ、コピーコンストラクタ、コピー代入演算子です。もちろん生成される条件があり、クラス
 * が上記関数を明示的に宣言していないけれど、コードでは使用している場合です。デフォルトコンストラクタが生成されるのは、クラス
 * が 1 つも宣言していない場合です（コンストラクタには実引数が必須と指定すると、コンパイラはデフォルトコンストラクタを生成しない）。
 * 生成された特殊メンバ関数は暗黙に public かつ inline になり、また、仮想デストラクタを持つ基底クラスを継承した派生クラスの
 * デストラクタ以外は、非仮想関数になります。基底クラスが仮想デストラクタを持っていれば、コンパイラが生成する派生クラスのデストラクタ
 * も仮想デストラクタになります。
 * C++11 から、ムーブコンストラクタとムーブ代入演算子の 2 つが追加されました。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_3_modern_c++_special_member_function.cpp -o ../bin/main
*/
#include <iostream>
#include <cassert>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](const auto message, const auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * このコンセプトがどれほどの価値があるのかは正直よく分からない、なくてもいいかな。
 * あるいは、ptr_print_error を constexpr 宣言してしまうとか。
*/

template <class Error>
concept Reasonable = requires (Error& e) {
    e.what();
};
template <class Error>
         requires Reasonable <Error>
void (*ptr_print_error)(Error) = [](const auto e) -> void {
    cout << "ERROR: " << e.what() << endl;
};

int test_debug() {
    puts("--- test_debug");
    try {
        auto pi = 3.141592;
        ptr_lambda_debug<const char*,const decltype(pi)&>("pi is ", pi);
        ptr_lambda_debug<const char*,const decltype(pi)*>("pi addr is ", &pi);
        throw runtime_error("It's test error.");
        return EXIT_SUCCESS;
    } catch(exception& e) {
        printf("e type is ... %s\n", typeid(e).name());
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * C++11 から、ムーブコンストラクタとムーブ代入演算子の 2 つが追加されました。
 * それぞれのシグネチャを挙げます。
*/

int main(void) {
    puts("START 項目 17 ：自動的に生成される特殊メンバ関数を理解する ===");
    if(0.01) {
        int ret1 = -1;
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", ret1 = test_debug());
        assert(ret1 == 1);      // 意図的に Error を発生させているため
    }

    puts("=== 項目 17 ：自動的に生成される特殊メンバ関数を理解する END");
}