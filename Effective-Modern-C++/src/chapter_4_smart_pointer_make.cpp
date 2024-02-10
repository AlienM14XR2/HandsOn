/**
 * 4 章 スマートポインタ
 * 
 * 項目 21 ：new の直接使用よりも std::make_unique や std::make_shared を優先する
 * 
 * std::make_shared は C++11 の一部となっていますが、std::make_unique は C++14 から標準ライブラリに加えられたものです。
 * もし、C++11 しか使えなくても std::make_unique の基本バージョンは自作が容易ですから。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_4_smart_pointer_make.cpp -o ../bin/main
*/
#include <iostream>
#include <memory>

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](const auto message, const auto debug) -> void {
    std::cout << "DEBUG: " << message << '\t' << debug << std::endl;
};

template <class Error>
concept Reasonable = requires(Error& e) {
    e.what();
};

template <class Error>
requires Reasonable<Error>
void (*ptr_print_error)(Error) = [](const auto e) -> void {
    std::cout << "ERROR: " << e.what() << std::endl;
};

int test_debug() {
    puts("=== test_debug");
    try {
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
 * std::make_shared は C++11 の一部となっていますが、std::make_unique は C++14 から標準ライブラリに加えられたものです。
 * もし、C++11 しか使えなくても std::make_unique の基本バージョンは自作が容易ですから。
*/

template <class T, class... Args>
std::unique_ptr<T> make_unique(Args&&... params) {
    return std::unique_ptr<T>(new T(std::forward<Args>(params)...));
}

int test_my_make_unique() {
    puts("=== test_my_make_unique");
    try {
        std::unique_ptr<int> ip = make_unique<int>(99);
        ptr_lambda_debug<const char*,const int&>("ip val is ", *ip.get()); 
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

class Widget {

};

void sample() {
    auto upw1(std::make_unique<Widget>());          // make 関数を使用
    ptr_lambda_debug<const char*,const Widget*>("upw1 addr is ", upw1.get());
    
    std::unique_ptr<Widget> upw2(new Widget());     // make 未使用
    ptr_lambda_debug<const char*,const Widget*>("upw2 addr is ", upw2.get());

    auto spw1(std::make_shared<Widget>());          // make 関数を使用
    ptr_lambda_debug<const char*,const Widget*>("spw1 addr is ", spw1.get());

    std::shared_ptr<Widget> spw2(new Widget());      // make 未使用
    ptr_lambda_debug<const char*,const Widget*>("spw2 addr is ", spw2.get());

    /**
     * new を用いたバージョンでは、作成する型を 2 度記述していますが、make 関数を使用するバージョンでは 1 度です。
     * 型を繰り返し記述することは、ソフトウェア工学での重要な信条、コードの重複を避けよ、に反します。
    */
}

/**
 * make 関数が望ましい理由の 2 つ目は、例外安全性への対応です。何らかの優先度を考慮しつつ Widget を処理する関数
 * を考えてみましょう。
 * 
 * int computePriority();
 * 
 * processWidget(std::shared_ptr<Widget>(new Widgete)), computePriority());    // リソース解放漏れの恐れあり！！
 * 
 * 上例では new で作成した Widget の解放漏れが発生する恐れがあります。
 * その答えは、ソースコードをオブジェクトコードへ翻訳するコンパイラにあります。実行時に関数に渡す実引数は、その関数の
 * 実行開始前に評価する必要があるため、processWidget 呼び出しでは、processWidget が実行を開始できるようになる前に、次
 * の内容を実行しなければなりません。
 * 
 * - 「new Widget」という式を評価しなければならない。すなわち、Widget をヒープ上に作成しなければならない。
 * - new により生成されたポインタの管理に責任を持つ、std::shared_ptr<Widget> のコンストラクタを実行しなければならない。
 * - computePriority を実行しなければならない。
 * 
 * 上例の順序で実行するようコードを生成することは、コンパイラの要件とはされていません。new の結果がコンストラクタの実
 * 引数になるため、「new Widget」は std::shared_ptr コンストラクタを呼び出す前に実行しなければなりませんが、computePriority
 * は new や std::shared_ptr よりも前でも後でも構わないのです。ここで重要なのは『二者の間』、すなわちコンパイラが次の順序で
 * 実行するコードを生成する場合です。
 * 
 * 1. 「new Widget」
 * 2. computePriority
 * 3. std::shared_ptr コンストラクタ
 * 
 * 上記の順序で実行するコードが生成され、かつ実行時に computePriority が例外を発生させると、ステップ 1 でダイナミックに割り
 * 当てた Widget が解放漏れとなってしまいます。
 * std::shared_ptr に保持させ、ライフタイムの管理が開始されるのはステップ 3 であるためです。
 * std::make_shared を用いればこの問題を回避できます。呼び出し側のコードは次のようなものになります。
 * 
 * processWidget(std::make_shared<Widget>(), computePriority());            // リソース解放漏れの恐れがない
*/

/**
 * new の直接使用よりも make 関数を優先する理由は確固たるものです。しかし、ソフトウェア工学上の優位点、例外安全性、効率向上に
 * も関わらず、本項目は絶対に make 関数を用いよとは述べておらず、優先せよとしています。これには理由があり、make 関数を使用でき
 * ない、または使用すべきではない場面も存在するのです。
 * 
 * 1 つはカスタムデリータ、2 つ目はあるオブジェクトを作成する際、そのコンストラクタがオーバーロードされており、仮引数に std::initializer_list
 * をとるものと、とらないものがある場合、波括弧を記述したオブジェクト作成は std::initializer_list をとるコンストラクタにオーバーロード
 * 解決されます。波括弧ではなく丸括弧を記述した場合は、std::initializer_list をとらないコンストラクタです。
 * make 関数では、内部でオブジェクトのコンストラクタに仮引数を完全転送しますが、ここで使用するのは丸括弧でしょうか、それとも波括弧でしょうか？
 * 型にもよりますが、この答えは大きく差異となります。
 * 
 * std::unique_ptr で make 関数が問題となるのはこの 2 点だけです（カスタムデリータと波括弧による初期化）。
*/

/**
 * std::shared_ptr および make 関数では、問題点はもう 2つあります。いずれも極端な場合ですが、極端な世界に生きる開発者もいます。
 * クラスによっては自身で operator new や operator delete を定義するものがあります。これらの関数を定義したということは、そのクラス
 * オブジェクトに対してはグローバルなメモリ割当／解放関数が不適切であることを意味します。
 * 専用の operator new と operator delete を持つ型のオブジェクトの作成に make 関数を用いるのは悪手になります。
*/


int main(void) {
    puts("START 項目 21 ：new の直接使用よりも std::make_unique や std::make_shared を優先する ===");
    if(0.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_debug());
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_my_make_unique());
        sample();
    }
    puts("=== 項目 21 ：new の直接使用よりも std::make_unique や std::make_shared を優先する END");
    return 0;
}