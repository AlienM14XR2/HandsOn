/**
 * 3 章 現代の C++ への移行
 * 
 * 項目 13 ：iterator よりも const_iterator を優先する
 * 
 * const_iterator とは、STL が備える、const を指すポインタ相当であり、変更を禁止された値を指します。
 * 可能な場面では常に const を使用せよと一般に言われており、この言葉を鵜呑みにすればイテレータを使用
 * する場面でもイテレータが指すものを変更する必要がなければ、const_iterator を使用すべきとなります。
 * 
 * 重要ポイント
 * - iterator よりも const_iterator を優先する。
 * - 汎用性を最大限備えたコードでは、非メンバ関数の begin、end、rbegin などを、同機能のメンバ関数よりも優先する。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_3_modern_c++_const_iterator.cpp -o ../bin/main
*/
#include <iostream>
#include <cassert>
#include <vector>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](const auto message, const auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

int test_debug() {
    puts("--- test_debug");
    try {
        auto pi = 3.141592;
        ptr_lambda_debug<const char*,const decltype(pi)&>("pi is ", pi);
        ptr_lambda_debug<const char*,const decltype(pi)*>("pi addr is ", &pi);
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << "ERROR: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}

/**
 * iterator を使用する C++98 コードを、C++11 の const_iterator へ対応させるのも非常に容易です。
 * 
 * 1983 を検索して、その位置に 1998 を挿入する。1983 がない場合は末尾に 1998 を追加する。
 * そのプログラム例。
*/

void sample() {
    puts("--- sample");
    std::vector<int> values = {2023, 2020, 1983, 1964, 1911};

    auto it = std::find(values.cbegin(), values.cend(), 1983);  // cbegin() と cend() の利用
    values.insert(it, 1998);
    for(auto val: values) {
        ptr_lambda_debug<const char*,const decltype(val)&>("val is ", val);
    }
    puts("---");
    /**
     * これで const_iterator を実用的に使用するコードとなりました！
    */

    std::vector<int> values2 = {2023, 2020};
    auto it2 = std::find(values2.cbegin(), values2.cend(), 1983);
    values2.insert(it2, 1998);
    for(auto val: values2) {
        ptr_lambda_debug<const char*,const decltype(val)&>("val is ", val);
    }

}

int main(void) {
    puts("START 項目 13 ：iterator よりも const_iterator を優先する ===");
    if(0.01) {
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_debug());
    }
    if(1.00) {
        sample();
    }
    puts("=== 項目 13 ：iterator よりも const_iterator を優先する END");
    return 0;
}