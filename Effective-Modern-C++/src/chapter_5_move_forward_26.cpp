/**
 * 5 章 右辺値参照、ムーブセマンティクス、完全転送
 * 
 * 項目 26 ：転送参照をとるオーバーロードは避ける
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_5_move_forward_26.cpp -o ../bin/main
*/
#include <iostream>
#include <chrono>
#include <set>

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](const auto message, const auto debug) -> void {
    std::cout << "DEBUG: " << message << "\t" << debug << std::endl;
};

template <class Error>
concept ErrReasonable = requires(Error& e) {            // このコンセプトはやはりあまり意味がないのかもな。
    e.what();
};
template <class Error>
requires ErrReasonable<Error>
void (*ptr_print_error)(Error) = [](const auto e) {
    std::cout << "ERROR: " << e.what() << std::endl;
};

void (*ptr_print_now)(void) = [](void) -> void {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::cout << std::ctime(&t) << std::endl;
};

int test_debug_and_error() {
    puts("=== test_debug_and_error");
    try {
        ptr_print_now();
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
 * 仮引数に任意の名前をとり、日付と時刻をログ出力し、名前をグローバルデータに追加する関数を開発
 * するとしましょう。まず浮かぶのは次のような関数でしょう。
*/

std::multiset<std::string> names;           // グローバル変数

void logAndAdd(const std::string& name) {
    puts("--- logAndAdd");
    auto now = std::chrono::system_clock::now();
    // log(now, "logAndAdd");
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    ptr_lambda_debug<const char*,const char*>("time is ", std::ctime(&t));
    names.emplace(name);
}

/**
 * 上例は悪いコードではありませんが、最高性能でもありません。呼び出し側として次の 3 通りを考えてみましょう。
 * 
 * std::string petName("Derla");
 * logAndAdd(petName);                      // 左辺値の std::string
 * 
 * logAndAdd(std::string("Persephone"));    // 右辺値の std::string
 * 
 * logAndAdd("Patty Dog");                  // 文字リテラル
 * 
 * 最初の呼び出しでは、logAndAdd の仮引数 name は変数 petName にバインドされ、logAndAdd 内で name は最終的に
 * names.emplace に渡されます。この name は左辺値のため、names にコピーされます。logAndAdd に渡されたものが
 * 左辺値である以上（petName）、このコピーを回避する方法はありません。
 * 
 * 2 番目の呼び出し例では、仮引数 name は右辺値にバインドされます（"Persephone" から明示的に作成された一時オブジェクトの std::string）。
 * name 自身は左辺値のため names にコピーされますが、原則としてムーブ可能であるとも分かっています。この呼び出し例ではコピーのコストが
 * かかりますが、ムーブするだけで避けられるコストです。
 * 
 * 最後の呼び出し例でも仮引数 name は右辺値にバインドされますが、今度は "Patty Dog" から暗黙に作成された std::string の一時オブジェクト
 * です。2 番目の呼び出し例同様に name は names にコピーされますが、logAndAdd に元々渡された実引数は文字列リテラルです。仮に文字列リテラル
 * を直接 emplace へ与えたとすると、std::string の一時オブジェクトを作成する必要は一切ありません。代わりに emplace が std::mutiset 内に
 * 文字列リテラルから std::string オブジェクトを直接作成します。この 3 番目の呼び出し例でも std::string をコピーするコストがかかりますが、
 * ずっとコスト安なムーブコストですら実は『払う必要はありません』。
 * 
 * logAndAdd を変更し転送参照をとるようにすると、上記 2 番目、3 番目の呼び出し例にある非効率性を改善できます（項目 24 参照）。項目 25 に
 * 従い、この転送参照を emplace へ std::forward するのです。
*/

int test_logAndAdd() {
    puts("=== test_logAndAdd");
    try {
        logAndAdd("2024-02-14-A");
        for(auto n: names) {
            ptr_lambda_debug<const char*,const decltype(n)&>("n is ", n);
        }

        std::string petName("Derla");
        logAndAdd(petName);

        logAndAdd(std::string("Persephone"));

        logAndAdd("Patty Dog");

        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

template <class T>
void logAndAdd_V2(T&& name) {
    puts("--- logAndAdd_V2");
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    ptr_lambda_debug<const char*,const char*>("time is ", std::ctime(&t));
    names.emplace(std::forward<T>(name));    
}

int test_logAndAdd_V2() {
    puts("=== test_logAndAdd_V2");
    try {
        std::string petName("Derla");
        logAndAdd_V2(petName);                      // 先の例と同様に multiset へ左辺値をコピー

        logAndAdd_V2(std::string("Persephone"));    // コピーではなく 右辺値をムーブ

        logAndAdd_V2("Patty Dog");                  // 一時オブジェクトの std::string をコピーではなく
                                                    // multiset 内で std::string を作成
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

// std::string nameFromIdx(int idx)       // idx に対応する名前を返す
// {
//     // ... TODO data access
//     std::string result;
//     result = "Derek";
//     return result;
// }
// void logAndAdd_V2(int idx) {
//     puts("--- logAndAdd_V2");
//     auto now = std::chrono::system_clock::now();
//     std::time_t t = std::chrono::system_clock::to_time_t(now);
//     ptr_lambda_debug<const char*,const char*>("time is ", std::ctime(&t));
//     names.emplace(nameFromIdx);
// }

/**
 * 私の環境では、上記のサンプルはコンパイルエラーになった。
 * 書籍が言いたいことは、転送参照をとる関数のオーバーロードは避けるという説明のためサンプルだった。
 * 転送参照とオーバーロードを組み合わせて使用するのは、ほぼ常に悪手ということだった。
 * 理由は、転送参照を用いた関数はプログラマが通常期待する以上にずっと多くの実引数を吸収してしまうため。
*/

int main(void) {
    puts("START 項目 26 ：転送参照をとるオーバーロードは避ける ===");
    if(0.01) {
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_debug_and_error());
    }
    if(1.00) {
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_logAndAdd());
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_logAndAdd_V2());
    }
    puts("=== 項目 26 ：転送参照をとるオーバーロードは避ける END");
    return 0;
}