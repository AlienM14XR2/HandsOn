/**
 * 4 章 スマートポインタ
 * 
 * 項目 20 ：不正ポインタになり得る std::shared_ptr ライクなポインタには std::weak_ptr を用いる
 * 
 * 逆説的に聞こえますが、std::shared_ptr のように振る舞いながらも、『対象リソースの所有権を共有しない』スマートポインタが有用に
 * なる場面もあります。つまり、オブジェクトのレファレンスカウントに影響しない std::shared_ptr ライクなポインタです。
 * std::shared_ptr では無縁の問題に対峙しなければなりません。ポインタが指す対象が破棄される問題です。真にスマートなポインタならば、
 * ポインタが『不正』になったことを追跡管理し、この問題に対応するでしょう。不正なポインタとは、ポインタが指すオブジェクトが存在し
 * ない状態を言います。この問題こそまさに std::weak_ptr というスマートポインタが対処するものです。
 * 
 * std::weak_ptr の有用性についてはまだ疑念があるかもしれません。その疑念は std::weak_ptr API を調べると強まるかもしれません。決して
 * スマートには見えない API です。std::weak_ptr はポインタを辿ることも、ヌルか否かを検査することもできません。std::weak_ptr は単独で
 * はスマートポインタにはなり得ず、std::shared_ptr に付属するものなのです。
 * 
 * キャッシュ機能や observer のリストを実装する場合に備え、std::weak_ptr が使用できると覚えておくと良いでしょう。
 * 
 * 重要ポイント
 * - 不正になる可能性がある std::shared_ptr ライクなポインタには std::weak_ptr を使用する
 * - std::weak_ptr を使用する可能性がある場面としては、キャッシュ、observer リスト、std::shared_ptr の循環防止がある。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_4_smart_pointer_weak_ptr.cpp -o ../bin/main
*/
#include <iostream>
#include <memory>
#include <unordered_map>

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
 * std::shared_ptr との関係は std::weak_ptr 作成時から始まります。通常は std::shared_ptr から std::weak_ptr を作成し、
 * std::shared_ptr が std::weak_ptr を初期化した時点から、両者は同じメモリ上の位置を指します。しかし、std::weak_ptr は
 * 対象オブジェクトのレファレンスカウントには影響しません。
*/

class Widget {

};

void sample() {
    puts("=== sample");
    /**
     * spw 作成語の対象 Widget のレファレンスカウント（RC）は 1
    */
    auto spw = std::make_shared<Widget>();

    /**
     * wpw は spw と同じ Widget を指すが RC は 1 のまま。
    */
    std::weak_ptr<Widget> wpw(spw);

    /**
     * RC は 0 になり Widget は破棄される wpw は不正ポインタになる。
    */
    spw = nullptr;

    /**
     * 不正ポインタとなった std::weak_ptr を 『expire された』と表現します。この状態は
     * 直接的に検査できます。
    */
    if(wpw.expired()) {
        puts("... wpw is expired.");
    }
}
/**
 * しかし、一般に必要とされる動作は、std::weak_ptr が expire されたかを調べ、expire されていなければ（不正ポインタでなければ）、
 * 対象オブジェクトにアクセスすることです。これは、言うは易し行うは難しです。std::weak_ptr は参照外し演算を備えていないため、その
 * ようなコードは記述できません。仮に参照外し演算を備えていたとしても、検査と参照外し演算をアトミックに実行しない以上、競合状態が
 * 発生してしまうでしょう。expired の呼び出しと参照外し演算の間に、他スレッドが再代入や、対象オブジェクトを指す最後の std::shared_ptr
 * の破棄などを実行すれば、対象オブジェクトは破棄され、自スレッドの参照外し演算は未定義動作となってしまいます。
 * ここで必要なのは、std::weak_ptr が expire されたか否かを検査し、されていなければ対象オブジェクトにアクセスする、アトミック演算です。
 * これは、std::weak_ptr から std::shared_ptr を作成することで実現できます。この処理には 2 つの形式があり、std::shared_ptr 作成時点で
 * std::weak_ptr が expire されている場合にどう対応するかで形式が異なります。
 * 1 つ目は std::weak_ptr::lock で std::shared_ptr を返します。この std::shared_ptr は std::weak_ptr が expire されていればヌルになります。
*/

void sample2() {
    puts("=== sample2");
    auto spw = std::make_shared<Widget>();
    std::weak_ptr<Widget> wpw(spw);

    std::shared_ptr<Widget> spw1 = wpw.lock();      // wpw が expire されていれば spw1 はヌル
    auto spw2 = wpw.lock();                         // 上例と同様だが auto を用いている

    /**
     * もう 1 つの形式は実引数に std::weak_ptr を与えた std::shared_ptr コンストラクタです。
     * この場合、std::weak_ptr が expire されていれば、例外がスローされます。
    */
    std::shared_ptr<Widget> spw3(wpw);
}

/**
 * std::weak_ptr は 安全に std::shared_ptr を扱うための補助なのだと思う。
*/

/**
 * std::weak_ptr の有用性についてはまだ疑念が残るかもしれません。一意な ID から読み取り専用オブジェクトを指すポインタを生成する factory 
 * 関数を考えてみましょう。factory 関数の戻り型に関する項目 18 に従い、次の関数は std::unique_ptr を返します。
 * 
 * std::unique_ptr<const Widget> loadWidget(WidgetID id);
 * 
 * 上例の loadWidget がコスト高であり（ファイルI/O やデータベースI/O を伴う）、同じID を何度も使用するのであれば、loadWidget と同じ内容
 * を実行するけれど、その結果をキャッシュする関数を開発すれば、妥当な最適化となるでしょう。しかし、1 度でも処理した Widget はキャッシュ
 * という重荷になり、それ自体で性能問題につながります。そのため、キャッシュした Widget をもう使用しなくなった時点での破棄が、もう 1 つの
 * 妥当な最適化となります。
 * このキャッシュ付き factory 関数の戻り型を std::unique_ptr とするのは適切ではありません。呼び出し側はキャッシュしたオブジェクトを指す
 * スマートポインタを受け取るべきであり、またオブジェクトのライフタイムを決定するのは呼び出し側であるべきです。同時にキャッシュにはオブ
 * ジェクトを指すポインタが必要です。しかし、factory 関数が返したオブジェクトの使用を終えた時点でそのオブジェクトは破棄され、キャッシュ
 * が不正ポインタになるため、キャッシュ内のポインタは不正になったことを検知できるようにしなければなりません。そのため、キャッシュ内のポ
 * インタは、不正ポインタを検知可能な std::weak_ptr にすべきです。std::weak_ptr が不正ポインタになったことを検知できるのは std::shared_ptr
 * がオブジェクトのライフタイムを管理する場合に限られる以上、このことは factory 関数の戻り型は std::shared_ptr とすべきことを意味します。
 * 
 * あまり綺麗ではありませんが、loadWidget に簡単なキャッシュ機能を単純に追加した例を挙げます。
*/

using WidgetID = std::size_t;

std::shared_ptr<const Widget> loadWidget(WidgetID id) {
    puts("--- loadWidget");
    // 実際はDB アクセス等があると仮定する。
    return std::make_shared<const Widget>();
}

std::shared_ptr<const Widget> fastLoadWidget(WidgetID id) {
    puts("--- fastLoadWidget");
    static std::unordered_map<WidgetID, std::weak_ptr<const Widget>> cache;

    auto objPtr = cache[id].lock();
    if(!objPtr) {
        objPtr = loadWidget(id);
        cache[id] = objPtr;
    }
    return objPtr;
}

void sample3() {
    puts("=== sample3");
    auto objPtr1 = fastLoadWidget(100U);
    ptr_lambda_debug<const char*,const Widget*>("objPtr1 Widget addr is ", objPtr1.get());

    auto objPtr2 = fastLoadWidget(100U);
    ptr_lambda_debug<const char*,const Widget*>("objPtr2 Widget addr is ", objPtr2.get());

}

/**
 * Coffee Break.
 * 
 * 個人的興味とその演習。
 * 関数ポインタも勿論スマートポインタの管理下に置けるはず。
*/

double add(const double& lhs, const double& rhs) {
    return (lhs + rhs);
}

double multi(const double& lhs, const double& rhs) {
    return (lhs * rhs);
}

int test_smart_pointer_and_func() {
    /**
     * コンパイルは通るが実行時にエラーだった、私の完敗だ：）
     * 関数ポインタは扱えないのかな、そんな訳ないとおもうけど。
     * うん、なんとか問題なく実行させることができたぞ。
    */
    puts("=== test_smart_pointer_and_func");
    try {
        using FuncPointer = double (*)(const double&,const double&);
        std::shared_ptr<FuncPointer> fpAdd = std::make_shared<FuncPointer>(add);
        auto ret1 = (*fpAdd.get())(3.0, 6.0);   // 関数をスマートポインタに設定するとそれは二重ポインタになるのか：）
        // ptr_lambda_debug<const char*,const char*>("add1 type is ", typeid(add1).name());
        ptr_lambda_debug<const char*,const decltype(ret1)&>("ret1 is ", ret1);

        std::shared_ptr<FuncPointer> fpMulti = std::make_shared<FuncPointer>(multi);
        auto ret2 = (*fpMulti.get())(3.0, 6.0);
        ptr_lambda_debug<const char*,const decltype(ret2)&>("ret2 is ", ret2);
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * キャッシュ機能や observer のリストを実装する場合に備え、std::weak_ptr が使用できると覚えておくと良いでしょう。
*/

int main(void) {
    puts("START 不正ポインタになり得る std::shared_ptr ライクなポインタには std::weak_ptr を用いる ===");
    if(0.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_debug());
    }
    if(1.00) {
        sample();
        sample2();
    }
    if(1.02) {
        sample3();
    }
    if(1.03) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ...", test_smart_pointer_and_func());
    }
    puts("=== 不正ポインタになり得る std::shared_ptr ライクなポインタには std::weak_ptr を用いる END");
    return 0;
}