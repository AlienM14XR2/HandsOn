/**
 * 1 章 型推論
 * 
 * 項目 3 ：decltype を理解する
 * 
 * decltype はまったくおかしな代物です。名前や式を与えるとその型を教えてくれます。通常は
 * 予想通りの型を教えてくれますが、頭をかきむしったり、確認のため仕様やオンライン上の Q&A 
 * サイトを探しまくらなければならないような結果を返すこともあります。
 * 
 * e.g. ) compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_1_type_inference_decltype.cpp -o ../bin/main
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall chapter_1_type_inference_decltype.cpp -o ../bin/main
*/
#include <iostream>
#include <vector>
#include <variant>
#include <cassert>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * まずは一般的な例からはじめましょう。すんなり受け入れられるありふれた例です。
 * テンプレート、auto の型推論の動作とは対象的に、decltype は、通常は、与えられた
 * 名前や式の正確な型をそのまま返します。
*/

bool foo(int lhs, double rhs) {
    puts("------ foo");
    int ret = lhs + rhs;
    ptr_lambda_debug<const char*,const int&>("ret is ", ret);
    return true;
}

void sample() {
    puts("--- sample");
    const int i = 0;
    decltype(i) ix = 3;
    ptr_lambda_debug<const char*,const int&>("ix+3 = ",ix+3);

    vector<int> vec = {3, 6, 9, 12, 15};
    decltype(vec) v = {18, 21, 24, 27, 30};
    for(int n: v) {
        ptr_lambda_debug<const char*,const int&>("n is ", n);
    }

    // 対象が関数であっても同じ
    foo(3, 6.6);
    decltype(foo(3, 6.6)) bar;
    decltype(foo) buzz;
    // これができるから何が嬉しいのか今はわからない。 関数の別名として利用できるわけではなくあくまで型の定義に過ぎない。
}

/**
 * C++11 での decltype の主要用途は、おそらく、戻り値が仮引数の型により決定される
 * 関数テンプレートの宣言でしょう
*/

template <class Container, class Index>
decltype(auto) authAndAccess(Container& c, const Index& i) {
    puts("------ authAndAccess");
    return c[i];
}

int test_authAndAccess() {
    puts("--- test_authAndAccess");
    try {
        std::vector<int> v{10, 11, 12, 13, 14};
        auto ret = authAndAccess(v, 4);
        ptr_lambda_debug<const char*,decltype(ret)&>("ret is ", ret);   // うん、言いたいことは分かるが、サンプルに説得力があまりない。
        
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}


/**
 * Coffee Break ： C 言語の関数ポインタについて
*/

int (*ptr_f)(int,double);

int add(int lhs, double rhs) {
    return (lhs + (int)rhs);
}
int multi(int lhs, double rhs) {
    return (lhs * (int)rhs);
}

int test_add_multi() {
    puts("--- test_add_multi");
    try {
        ptr_f = &add;
        ptr_lambda_debug<const char*,const int&>("ptr_f(3, 6.3) ... ", ptr_f(3, 6.3));
        ptr_f = &multi;
        ptr_lambda_debug<const char*,const int&>("ptr_f(9, 9.9) ... ", ptr_f(9, 9.9));
        /**
         * これも 単純なポリモーフィズムであり、Strategy パターンの原型と考えられないだろうか。
        */
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}


int main(void) {
    puts("START 項目 3 ：decltype を理解する ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    if(1.00) {
        sample();
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_authAndAccess());
        ptr_lambda_debug<const char*,const int &>("Play and Result ... ",test_add_multi());
    }
    puts("=== 項目 3 ：decltype を理解する END");
    return 0;
}
