/**
 * 3 章 現代の C++ への移行
 * 
 * 項目 10 ：enum にはスコープを設ける
 * 
 * 重要ポイント
 * - C++98 スタイルの enum は現在ではスコープを持たない enum と呼ばれる。
 * - スコープを持つ enum の列挙子は enum 内でのみその名前が有効である、他の型へ変換する場合はキャストが必要である。
 * - スコープを持つ enum も持たない enum も、基礎とする型の指定が可能である。
 *   スコープを持つ enum の基礎とする型のデフォルトは int である。スコープを持たない enum の基礎とする型のデフォルトは存在しない。
 * - スコープを持つ enum は常に前方宣言可能である。スコープを持たない enum 基礎とする型を指定する場合にのみ前方宣言可能である。
 * 
 * e.g.) compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_3_modern_c++_enum.cpp -o ../bin/main
*/
#include <iostream>
#include <tuple>
#include <string>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](const auto message, const auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * スコープを持つ enum は「enum class」と宣言するため、enum class と表記することもあります。
 * スコープを持つ enum により名前空間が汚染される機会を減らせる点だけでも、スコープを持たない
 * enum よりも優先する充分な理由ですが、スコープをもつ enum の利点はもう 1 つあります。
 * 列挙子がとても強力に型付けされる点です。
 * スコープを持たない enum の列挙子は、汎用整数に暗黙に変換されます（また、整数型から浮動小数点型へも変換される）。
*/

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

/**
 * 次のような場合は、スコープを持つ enum では冗長になる。
 * スコープなし enum の方がスッキリと見える。
*/

using UserInfo = std::tuple<string,         // 名前
                            string,         // email
                            size_t>;        // 評価

enum struct UserInfoField {
    uiName,
    uiEmail,
    uiReputation,
};

enum GlobalUserInfoField {
    uiName,
    uiEmail,
    uiReputation,
};

void sample2() {
    puts("--- sample2");
    UserInfo userInfo{"jack", "jack@loki.org",0U};

    auto val = std::get<0>(userInfo);       // この 0 が一体何を指すものなのか、分かりづらいという問題がある。
    ptr_lambda_debug<const char*,const decltype(val)&>("val is ",val);

    auto val2 = std::get<(static_cast<int>(UserInfoField::uiName))>(userInfo);       // 非常に冗長に思うが、スコープを持つ enum ではこうなる
    ptr_lambda_debug<const char*,const decltype(val2)&>("val2 is ",val2);

    auto val3 = std::get<GlobalUserInfoField::uiName>(userInfo);                    // スコープなし enum の方がスッキリみえる
    ptr_lambda_debug<const char*,const decltype(val3)&>("val3 is ",val3);
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
        sample2();
    }
    puts("=== 項目 10 ：enum にはスコープを設ける END");
    return 0;
}