/**
 * 5 章 右辺値参照、ムーブセマンティクス、完全転送
 * 
 * 項目 25 ：右辺値参照には std::move を、転送参照には std::forward を用いる
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_5_move_forward_25.cpp -o ../bin/main
*/
#include <iostream>
#include <chrono>
#include <memory>
#include <vector>

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](const auto message, const auto debug) -> void {
    std::cout << "DEBUG: " << message << '\t' << debug << std::endl;
};

template <class Error>
concept EReasonable = requires(Error& e) {
    e.what();
};
template <class Error>
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
 * Coffee Break
 * コマンド・インジェクションを考える
*/

int test_escape_str() {
    puts("=== test_escape_str");
    try {
        std::string command_1{"INSERT INTO foo (name, email) VALUES ('derek', 'derek@loki.org')"};
        std::string command_2{"INSERT INTO foo (name, email) VALUES ('derek', 'derek@loki.org'); DROP TABLE foo"};
        // 基本的には、害のある文字コードをエスケープすればいいと思ってるけど、それでは甘いのだろうな。
        // セキュアな文字操作やセキュアなコーディング、ライブラリ等を次は調べてみるかな。
        // e.g. MySQL は C++ でも Prepared Statements があるからそれを使うのがまず、はじめにありきだとは思う。
        std::string command_3{"INSERT INTO foo (name, email) VALUES ('derek', 'derek@loki.org')\\; DROP TABLE foo"};
        ptr_lambda_debug<const char*,const std::string&>("command_1 is ", command_1);
        ptr_lambda_debug<const char*,const std::string&>("command_2 is ", command_2);
        ptr_lambda_debug<const char*,const std::string&>("command_3 is ", command_3);
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * 書籍に戻る。
 * 右辺値参照がバインドするのはムーブ可能なオブジェクトのみです。右辺値参照の仮引数があれば、その
 * バインドするオブジェクトはムーブ可能であると『判断できます』。
 * 具体的に言えば、ムーブコンストラクタ、ムーブ代入演算子など。
 * 
 * 右辺値という性質を活用できるようオブジェクトを関数へ渡す必要がある場合は、オブジェクトにバインド
 * された仮引数を右辺値へキャストします。項目 23 でも述べましたが、このキャストは、std::move が行う
 * ことと言うよりも、std::move がなんのために作られたかという話になります。
 * 一方、転送参照は （項目 24 参照）、常にムーブ可能オブジェクトにバインドされているとは『限りません』。
 * 転送参照を右辺値へキャストするのは、オブジェクトが右辺値により初期化された場合のみに限定するべきで、
 * これを実行するのが、std::forward であることは項目 23 で述べました。
 * 
 * 
 * ※ 備忘録： std::move, std::forward はどちらも右辺値へのキャストを行う。違いは、std::move はすべ
 * て右辺値へのキャストを行い、std::forward は 転送参照（右辺値参照 || 左辺値参照 どちらもありえる）
 * の場合でも、右辺値参照可能な場合のみ、右辺値へのキャストを行う。
*/

struct SomeDataStructure {
    ~SomeDataStructure() {
        puts("------ SomeDataStructure Destructor");
    }
};

class Widget {
public:
    Widget(): name(""), p(std::make_shared<SomeDataStructure>(SomeDataStructure{}))
    {}
    Widget(const std::string& _name): name{_name}, p(std::make_shared<SomeDataStructure>(SomeDataStructure{}))
    {}
    Widget(const Widget& rhs) = default;                // コピーコンストラクタ ... ムーブ演算を明示的に宣言した場合は、=delete されたはず。
    Widget& operator=(const Widget& rhs) = default;     // コピー代入演算子 ... ムーブ演算を明示的に宣言した場合は、=delete されたはず。
    Widget(Widget&& rhs): name(std::move(rhs.name)), p(std::move(rhs.p))    // ムーブコンストラクタ rhs は間違いなくムーブ可能オブジェクト
    {
        puts("------ Widget Move Constructor");
    }
    // ...
    std::string getName() noexcept { return name; }
    template<class T>
    void setName(T&& newName) {             // newName は転送参照
        name = std::forward<T>(newName);
        // name = std::move(newName);          // コンパイル可能だけど、ダメダメダメ！
    }
private:
    std::string name;
    std::shared_ptr<SomeDataStructure> p;
};

void sample() {
    puts("=== sample");
    Widget w1("Chasire");
    Widget w2 = std::move(w1);
    ptr_lambda_debug<const char*,const std::string&>("(w2) name is ", w2.getName());
    std::string newName{"Alice"};
    w2.setName<std::string&>(newName);
    ptr_lambda_debug<const char*,const std::string&>("(w2) name is ", w2.getName());
    w2.setName<const std::string&>("Jabberwocky");
    ptr_lambda_debug<const char*,const std::string&>("(w2) name is ", w2.getName());
}

/**
 * Coffee Break
 * 
 * O/R Mapping を考えてみる
 * まずは最低限必要と思われる情報をクラスにする。
 * - 型情報
 * - 名前（カラム名）
 * - 値
*/

template <class T>
class DataField {
public:
    DataField(const std::string& _name, const T& _value): name(_name), value(_value)
    {}

    // ...
    std::pair<T,std::string> bind() {
        return {value, name};
    }

private:
    std::string name;
    T value;
};

/**
 * 上例のクラスを std::vector なりのコンテナに詰めたものが、レコードの 1 行を表現するのではないのか？
*/

int test_DataField() {
    puts("=== test_DataField");
    try {
        DataField<int> d1("id",1);
        auto[value, name] = d1.bind();
        ptr_lambda_debug<const char*,const decltype(value)&>("value is ", value);
        ptr_lambda_debug<const char*,const std::string&>("value type is ", typeid(value).name());   // Boost のライブラリの方が正確との情報があった。
        ptr_lambda_debug<const char*,const decltype(name)&>("name is ", name);
        /**
         * TODO Jack
         * 時間を見て、MySQL にある型と一致する C++ の型を網羅させる。
        */
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * 書籍に戻る。
 * 値戻しを行う関数の場合は関数から戻される右辺値参照に std::move() を実行しても、何も失うものはありません
 * （得るものがある可能性は大いにある）。
 * e.g.
 * 
 * Matrix operator+(Matrix&& lhs, const Matrix& rhs)
 * {
 *   lhs += rhs;
 *   return std::move(lhs);
 * }
 * 
 * 転送参照と std::forward の場合も似ています。約分していない可能性がある分数を表現するオブジェクト Fraction を
 * とる関数テンプレート reduceAndCopy を例に考えてみましょう。reduceAndCopy は約分し、その結果の値のコピーを返し
 * ます。元オブジェクトが右辺値ならば結果の値を戻り値へムーブするべきですが（コスト高なコピー演算を避けられる）
 * 元オブジェクトが左辺値の場合はコピーを作成する必要があります。
 * e.g.
 * 
 * template <class T>
 * Fraction reduceAndCopy(T&& frac) {
 *  frac.reduce();
 *  return std::forward<T>(frac);
 * }
 * 
*/

Widget makeWidget(const std::string& name) {
    Widget w(name);
    return w;
}

/**
 * 関数の戻り値用に割り当てたメモリ内でローカル変数 w をコンストラクトすれば w のコピーを不要とできる
 * 点については、とうの昔に指摘されています。この問題を 戻り値の最適化（Return Value Optimization, RVO）
 * と言い、C++ 標準では誕生以来ずっと明記しています。
 * 
 * 私はコピーされてるとばかり思っていた、戻り値の最適化されてたのか。
*/

int main(void) {
    puts("START  項目 25 ：右辺値参照には std::move を、転送参照には std::forward を用いる ===");
    if(0.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_debug_and_error());
    }
    if(0.02) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_escape_str());
    }
    if(1.00) {
        sample();
    }
    if(1.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_DataField());
    }
    puts("===  項目 25 ：右辺値参照には std::move を、転送参照には std::forward を用いる END");
    return 0;
}
