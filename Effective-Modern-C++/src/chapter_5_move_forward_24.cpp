/**
 * 5 章 右辺値参照、ムーブセマンティクス、完全転送
 * 
 * 項目 24 ：転送参照と右辺値参照の違い
 * ※ユニバーサル参照とは著者の造語と聞いたことがあるがそんなことないのかな、一般的には転送参照と呼ばれる。
 * 
 * 重要ポイント
 * - 関数テンプレートの仮引数の型が、推論した型 T に対する T&& の場合、または auto&& により宣言したオブジェクトの場合は、
 *   その仮引数やオブジェクトは転送参照である。
 * - 宣言した型の形式が厳密に type&& でない場合、または型の推論を伴わない場合は、type&& は右辺値参照を表す。
 * - 右辺値により初期化した転送参照は右辺値参照に対応する。また、左辺値により初期化した転送参照は左辺値参照に対応する。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_5_move_forward_24.cpp -o ../bin/main
*/
#include <iostream>
#include <chrono>
#include <ctime>
#include <vector>
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
 * ある型 T の右辺値参照を宣言するには T&& と記述します。このためソースコードに「T&&」と記述されているのを見れば、これは
 * 右辺値参照だと受け取れるのは当然に思えます。しかし、あぁなんとしたことか、事はそう簡単ではありません。
*/

class Widget {
public:
    Widget(): name{""}
    {}
    Widget(const std::string& _name): name{_name}
    {}
    // ...
    std::string getName() noexcept {
        return name;
    }
private:
    std::string name;
};

void sample(void) {
    puts("=== sample");
    Widget&& w1 = Widget("Chasire");    // 右辺値参照

    auto&& w2 = w1;                     // 右辺値参照ではない
    // Widget&& w3 = w1;                   // これだとコンパイルエラーになる：）
    ptr_lambda_debug<const char*,const std::string&>("(w2) name is ", w2.getName());
}

/**
 * 実際には「T&&」には 2 つの意味があります。1 つはもちろん右辺値参照です。この参照は期待通りに動作し、バインドするのは
 * 右辺値のみであり、その『レゾンデトル（存在理由）』はムーブ元になれるオブジェクトの表現にあります。
 * 「T&&」が持つもう 1 つの意味は、右辺値参照か左辺値参照の『どちらか』一方を表すことです。ソースコード上では右辺値参照
 * と見分けが付きませんが（どちらも「T&&」）、あたかもあたかも左辺値参照であるかのように振る舞えます（すなわち「T&」）。
 * この 2 つの性質を併せ持つため、（右辺値参照のように）右辺値にバインド可能であり、また（左辺値参照のように）左辺値にも
 * バインド可能です。
 * さらに、const オブジェクト、非 const オブジェクト、volatile オブジェクト、非 volatile オブジェクトにも、また、const 
 * かつ volatile なオブジェクトにもバインド可能です。実質的に『どんなもの』にもバインド可能です。このような前例のない
 * 柔軟性を備えた参照には専用の名前が相応しいでしょう。著者は『ユニバーサル参照』と呼んでいます。
 * ※『転送参照（forwarding reference）』と一般には呼ばれる。
 * 
 * 転送参照の一般的な使用は関数テンプレートの仮引数です。
 * 使用場面のもう 1 つは auto による宣言です。先に挙げた例でも使用しています。
 * 両者に共通するのは『型推論』です。
 * 型推論を伴わない「T&&」は右辺値参照を表します。
*/

void process(const Widget& lvalArg) {
    puts("--- process 左辺値を処理");
}
void process(Widget&& rvalArg) {
    puts("--- process 右辺値を処理");
}

void foo(Widget&& param) {              // 右辺値参照
    puts("--- foo");
}

template <class T>
void buzz(T&& param) {                  // 右辺値参照ではない、転送参照と呼ばれる、型の推論を伴う。
    puts("--- buzz");
    ptr_lambda_debug<const char*,const char*>("TYPE is ", typeid(param).name());
    process(std::forward<T>(param));
}

/**
 * 転送参照は参照なのですから、初期化が必須です。転送参照が右辺値参照を表すのか左辺値参照を表すのかを決定するのは初期化子です。
 * 初期化子が右辺値ならば、転送参照は右辺値参照、初期化子が左辺値ならば、転送参照は左辺値参照を表します。
 * 
 * ※ そもそもなぜこの右辺値参照や転送参照が必要なんだっけ？ ポインタを専有所有する、std::unique_ptr （これらはコピー演算は不可）
 * などがあるためだと思ってるけど。つまり、ムーブ演算と関係が深いと考えている。
 * Foo(Foo&& rhs), Foo& operator=(Foo&& rhs), ムーブ演算は、右辺値参照か転送参照になるため。
*/

void sample2(void) {
    puts("=== sample2");
    Widget w1("Alice");
    buzz(w1);               // buzz には左辺値が渡される。param の型は Widget&  （左辺値参照）
    buzz(std::move(w1));    // buzz には右辺値が渡される。param の型は Widget&& （右辺値参照）
}

/**
 * 参照が転送参照になるためには型推論が必須ですが、それだけではありません。参照を宣言する『形式』としても正当である必要がありますが、
 * この形式には制約がきわめて多く、厳密に「T&&」と記述しなければなりません。
*/

template<class T>
void bar(std::vector<T>&& param) {      // param は右辺値参照
    puts("--- bar");
}

/**
 * 上例の bar を実行すると、型 T が推論されます（呼び出し側が明示的に指定するような特殊な場合は除く）。
 * しかし、param の型を宣言する形式は「T&&」ではなく「std::vector<T>&&」です。このため、文法により param
 * は転送参照となる可能性はゼロとなり、右辺値参照のみとなります。bar へ左辺値を渡そうとすると、コンパイラ
 * は喜々としてこれを指摘します。
*/

void sample3() {
    puts("=== sample3");
    std::vector<int> v{0,1,2};
    for(auto i: v) {
        ptr_lambda_debug<const char*,const decltype(i)&>("i is ", i);
    }
    // error: cannot bind rvalue reference of type ‘std::vector<int>&&’ to lvalue of type ‘std::vector<int>’
    // bar(v);     // コンパイルエラー：）これは 転送参照にはならないため、左辺値を渡すことが NG だから。

    // 次のように v を ムーブ（右辺値へのキャスト）を行うことで、コンパイルエラーは回避できる。
    bar(std::move(v));
}

/**
 * const 修飾を関数の仮引数に単に加えるだけでも、転送参照にはなれません。
*/

template <class T>
void cbuzz(const T&& param) {                  // param は右辺値参照
    puts("--- cbuzz");
    ptr_lambda_debug<const char*,const char*>("TYPE is ", typeid(param).name());
    process(param);
}

void sample4() {
    puts("--- sample4");
    Widget w1{"Chasire"};
    // cbuzz(w1);      // コンパイルエラー
    cbuzz(std::move(w1));
} 

/**
 * std::vector の push_back() と emplace_back() の違い。
 * push_back は 常に T の右辺値参照型の仮引数を宣言し、emplace_back は、概念的には同等でも、型推論を『伴います』。
 * つまり、転送参照になる。
*/

void sample5() {
    puts("--- sample5");
    std::vector<Widget> v;
    Widget w1{"Jack"};
    Widget w2{"Derek"};
    Widget w3{"Alice"};

    v.push_back(w1);    // これは私の予想だが、内部的には必ずムーブ演算が行われているということ。
    v.push_back(w2);
    v.push_back(w3);

    for(auto w: v) {
        printf("name is %s\n", w.getName().c_str());
    }
    ptr_lambda_debug<const char*,const std::string&>("(w3) name is ", w3.getName());

    /**
     * 以下は個人的な興味を掘り下げるコーディング。
    */
    // std::vector<Widget&> v2;    // コンパイルエラーだった：）
    using VecSharedPtr = std::vector<std::shared_ptr<Widget>>;
    VecSharedPtr v2;
    std::shared_ptr<Widget> w4 = std::make_shared<Widget>(Widget("Jabberwocky"));
    std::shared_ptr<Widget> w5 = std::make_shared<Widget>(Widget("Chasire")) ;
    std::shared_ptr<Widget> w6 = std::make_shared<Widget>(Widget("humptydumpty"));

    v2.push_back(w4);
    v2.push_back(w5);
    v2.push_back(w6);

    for(auto wup: v2) {
        ptr_lambda_debug<const char*,const std::string&>("name is ",wup.get()->getName());
    }
    /**
     * std::vector のようなコンテナでスマートポインタを利用する場合は std::shared_ptr の方が std::unique_ptr
     * より扱い易い。 では一度挫折した、std::unique_ptr に挑戦してみる：）
     * std::unique_ptr はポインタの専有故の扱いづらさがやはりある。すなわち私が如何にコピーや参照に助けられている
     * のかという表裏なのだろう。
    */
    puts("--- retry");
    using VecUniquePtr = std::vector<std::unique_ptr<Widget>>;
    VecUniquePtr v3;
    std::unique_ptr<Widget> w7 = std::make_unique<Widget>(Widget("Jollyroger"));
    std::unique_ptr<Widget> w8 = std::make_unique<Widget>(Widget("Skull")) ;
    std::unique_ptr<Widget> w9 = std::make_unique<Widget>(Widget("Bone"));
    v3.push_back(std::move(w7));
    v3.push_back(std::move(w8));
    v3.push_back(std::move(w9));
    for(std::size_t i = 0; i<v3.size(); i++) {
        ptr_lambda_debug<const char*,const std::string&>("name is ", v3.at(i).get()->getName());
    }

}

/**
 * auto で宣言した転送参照は、関数テンプレートの仮引数に用いる転送参照ほど一般的ではありませんが、C++11 でもたびたび
 * 登場しますし、C++14 ではさらに多く使用されます。C++14 のラムダ式では仮引数を auto&& と宣言できるためです。
 * 例えば、ある関数の所要実行時間を測定する C++14 のラムダ式は、次のように記述できます。
*/

auto timeFuncInvocation = [](auto&& func, auto&&... params) {
    puts("--- timeFuncInvocation");
    std::clock_t start = clock();
    std::forward<decltype(func)>(func)(
        std::forward<decltype(params)>(params)...       // params を与え func を実行
    );
    // stop timer and record elapsed time;     // タイマ停止、経過時間を記録
    std::clock_t end = clock();
    std::cout << "passed " << ((double)(end-start))/CLOCKS_PER_SEC << " sec." << std::endl;
    std::cout << "passed " << end-start << std::endl;
};

int main(void) {
    puts("START 項目 24 ：転送参照と右辺値参照の違い ===");
    if(0.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_debug_and_error());
    }
    if(1.00) {
        sample();
        sample2();
        sample3();
        sample4();
        sample5();
        timeFuncInvocation(sample5);
    }
    puts("=== 項目 24 ：転送参照と右辺値参照の違い END");
}