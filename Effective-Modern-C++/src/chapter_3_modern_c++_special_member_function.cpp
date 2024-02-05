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
 * 重要ポイント
 * - 特殊メンバ関数とはコンパイラ自らが生成するものである。デフォルトコンストラクタ、デストラクタ、コピー演算、ムーブ演算である。
 * 
 * - ムーブ演算は、ムーブ演算、コピー演算、デストラクタのいずれかも明示的に宣言していないクラスに対してのみ生成される。
 * 
 * - コピーコンストラクタは、コピーコンストラクタを明示的に宣言していないクラスに対してのみ生成され、ムーブ演算を宣言した場合には「=delete」される。
 *   コピー代入演算子は、コピー代入演算子を明示的に宣言していないクラスに対してのみ生成され、ムーブ演算を宣言した場合には「=delete」される。
 *   デストラクタを明示的に宣言したクラスに対するコピー演算の生成は非推奨とされている（これ知らなかった、つまり、リソース管理時はムーブ演算にしろと）。
 * 
 * - テンプレートメンバ関数が特殊メンバ関数の生成を抑制することはない。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_3_modern_c++_special_member_function.cpp -o ../bin/main
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall chapter_3_modern_c++_special_member_function.cpp -o ../bin/main
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

class Point {
private:
    double x,y;
public:
    Point() = delete;
    Point(double _x, double _y): x{_x}, y{_y}
    {}
    Point(const Point& own) = default;
    Point& operator=(const Point& lhs) = default;
    // ~Point() {}
    // ...
    double getx() noexcept { return x; }
    double gety() noexcept { return y; }

};
class Widget {
private:
    Point top{-10.0, 0.0}, bottom{-5.0, 5.0};
public:
    Widget(Point _top, Point _bottom): top{std::move(_top)}, bottom{std::move(_bottom)}                      // デフォルトコンストラクタ
    {}
    Widget(const Widget&) = default;        // デフォルトコピーコンストラクタ
    Widget(Widget&& rhs) {                  // ムーブコンストラクタ
        puts("------ move constructor");
        *this = rhs;
    }
    Widget& operator=(Widget&& rhs) {       // ムーブ代入演算子
        puts("------ move assignment operator");
        *this = rhs; 
        return *this; 
    }
    Widget& operator=(const Widget&) = default;
    // ...
    Point& getTop()    noexcept { return top; }
    Point& getBottom() noexcept { return bottom; }
};

int test_Widget() {
    puts("--- test_Widget");
    try {
        Widget widget{Point{0.0, 0.0}, Point{30.0, 60.0}};
        Widget w1 = std::move(widget);      // ムーブコンストラクタが呼ばれる。
        Widget w2 = Widget{Point{0.0, 0.0}, Point{33.3, 66.6}};
        w2 = std::move(widget);             // ムーブ代入演算子が呼ばれる。

        printf("w1 top(x,y) : bottom(x,y) is top(%.2f,%.2f): bottom(%.2f,%.2f)\n", w1.getTop().getx(), w1.getTop().gety(), w1.getBottom().getx(), w1.getBottom().gety());
        printf("w2 top(x,y) : bottom(x,y) is top(%.2f,%.2f): bottom(%.2f,%.2f)\n", w2.getTop().getx(), w2.getTop().gety(), w2.getBottom().getx(), w2.getBottom().gety());

        ptr_lambda_debug<const char*,const Widget*>("widget addr is ", &widget);
        ptr_lambda_debug<const char*,const Widget*>("w1     addr is ", &w1);
        ptr_lambda_debug<const char*,const Widget*>("w2     addr is ", &w2);
        return EXIT_SUCCESS;
    } catch(exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * 生成規則の動作は、対応するコピー関数と同様です。必要な場合にのみ生成され、その動作は非 static メンバ変数「メンバ単位ムーブ」です。
 * すなわち、ムーブコンストラクタは仮引数 rhs からクラスの 非 static メンバ変数をそれぞれムーブコンストラクトし、ムーブ代入演算子は
 * 同様にムーブ代入します。ムーブコンストラクタは基底クラス部分もムーブコンストラクタし（もしあれば）、ムーブ代入演算子も同様に基底
 * クラス部分を含めムーブ代入します。
 * 
 * コピーコンストラクタとコピー代入演算子は個々に独立しており、ユーザがどちらかを個別に宣言しても、問題ありません（コンパイラ
 * は必要があれば、特殊メンバ関数のもう一方を自動生成する）。しかし、ムーブに関してははこの規則は当てはまらず、ムーブコンストラクタ
 * とムーブ代入演算子は個別で宣言しても、どちらかが自動生成されることもなく、また、コピー代入演算子を無効（= delete）にします。
 * 
 * さっきこれに引っかかったから、明示的に 「Widget& operator=(const Widget&) = default;」を Widget クラスに宣言しなければいけなかったのか。
 * ここまで読んで納得した。
*/

/**
 * 読者は恐らく『三関数同時ルール』というガイドラインを耳にしたことがあるでしょう。三関数同時ルールとは、コピーコンストラクタ、コピー代入演算子、
 * デストラクタのいずれか 1 でも宣言する場合は、この 3 つをすべて宣言しなさいというもので、何らかのリソース管理を行うクラスでは、ほとんどの場合で
 * コピー演算の動作を引き継ぐ必要性があるという観測から導かれたルールです。ほぼ次のことが該当します。
 * 
 * （1）コピー演算の一方が何らかのリソース管理を伴うならば、もう一方のコピー演算でも同じリソース管理が恐らく必要になるだろう。
 * （2）恐らくクラスデストラクタでも同じリソースを管理することになるだろう（通常はリソースを解放する）。
 * 
 * C++11 では三関数同時ルールという考え方が効力を発揮し、コピー演算を宣言すれば、ムーブ演算の暗黙の生成を抑制すべきという観測結果も組み合わさり、
 * ユーザ宣言のデストラクタを持つクラスでは『ムーブ演算を生成しない』こととされました。
 * 最終的に、クラスムーブ演算が生成されるのは（必要に応じ）次の 3 つの条件がすべて真の場合のみです。
 * 
 * （1）クラスがコピー演算を宣言していない。
 * （2）クラスがムーブ演算を宣言していない。
 * （3）クラスがデストラクタを宣言していない。
 * 
 * つまり、余計なことは極力避けた方が実装が楽になると思った次第なのだが。
 * 別な言い方をするなら、リソース管理（クラス内でのメモリ取得 new と解放 delete）をする場合は上記の実装はプログラマが責任を持たないといけないこと
 * を意味する。
*/

/**
 * 上記の確認と演習を兼ねて、リソース管理を行うクラスを実装してみる。
 * 
 * - 何ら有効なクラスである必要はない。
 * - メモリの動的取得と解放を行えればそれでいいはず。
 * - タロットの「愚者」は旅のはじまりの暗示があるという。
*/

class Fool final {
private:
    string* memory = nullptr;
    // ...
    void copyProc(const Fool& own) {
        this->memory = new string();
    }
public:
    Fool()
    {
        memory = new string();
    }
    ~Fool() {
        puts("------ デストラクタ");
        ptr_lambda_debug<const char*,const Fool*>("this addr is ",this);
        if(memory) {
            delete memory;
        }
    }
    Fool(const Fool& own) {
        puts("------ コピーコンストラクタ");
        copyProc(own);
    }
    Fool& operator=(const Fool& lhs) {
        puts("------ コピー代入演算子");
        if(memory) {
            delete memory;
        }
        copyProc(lhs);
        return *this;
    }
    Fool(Fool&& rhs) {
        puts("------ ムーブコンストラクタ");
        this->memory = rhs.memory;
        rhs.memory = nullptr;
    }
    // Fool& operator=(Fool&& rhs) {        // これが循環して、コアダンプで終了する
    //     puts("------ ムーブ代入演算子");
    //     if(memory) {
    //         delete memory;
    //     }
    //     this->size = rhs.size;
    //     this->memory = rhs.memory;
    //     rhs = 0;
    //     rhs.memory = nullptr;
    //     return *this;
    // }

    // ...
    void intput(const char* src) {
        *memory = src;
    }
    string output() {
        string ret = "";
        if(memory) {
            ret = *memory;
        }
        return ret;
    }
};

/**
 * 上のクラスは別に特殊なことは何もしていない、ただ内部でメモリ（ヒープ）を動的に確保しているだけだ。
 * にも関わらず、考慮すべきことが格段に増えた。リソース管理はしない方がいいということがこれだけでも分かる。
*/

int test_The_Fool() {
    puts("--- test_The_Fool");
    try {
        Fool f1{};
        Fool f2 = f1;
        Fool f3{};
        f3 = f1;
        puts("--- before f4");
        Fool f4 = std::move(Fool{});
        Fool f5{};
        puts("--- before f5");
        f5 = std::move(Fool{});

        ptr_lambda_debug<const char*,const Fool*>("f1 addr is ",&f1);
        ptr_lambda_debug<const char*,const Fool*>("f2 addr is ",&f2);
        ptr_lambda_debug<const char*,const Fool*>("f3 addr is ",&f3);
        ptr_lambda_debug<const char*,const Fool*>("f4 addr is ",&f4);
        ptr_lambda_debug<const char*,const Fool*>("f5 addr is ",&f5);
        return EXIT_SUCCESS;
    } catch(exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * 上記のコーディングとコンパイル、及び実行結果から、私は次のように判断した。
 * コピー代入演算子とムーブ代入演算子の併用はできない、どちらか片方でよい。
 * クラス設計の戦略の問題だと結論付けた。
*/

int test_The_Fool_IO() {
    puts("--- test_The_Fool_IO");
    try {
        Fool f1{};
        f1.intput("Who is Jolly Rogers ?");
        string str1 = f1.output();
        ptr_lambda_debug<const char*,const string&>("str1 is ", str1);
        return EXIT_SUCCESS;
    } catch(exception& e) {
        ptr_print_error<decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * Try and Errors、実践することの重要性を改めて実感した。
 * C++ ではもう二度と char の配列は使わないと肝に銘じること。リファクタの前後をよく見比べてみよう。
 * そもそも Fool クラスは無意味なクラスで、リソース管理（メモリの動的取得と解放）とクラスのコピー、ムーブ
 * の確認が主たる目的だったが、char 配列を最初に採用したことで、『酷い旅』に出かけることになった。
*/

int main(void) {
    puts("START 項目 17 ：自動的に生成される特殊メンバ関数を理解する ===");
    if(0.01) {
        int ret1 = -1;
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", ret1 = test_debug());
        assert(ret1 == 1);      // 意図的に Error を発生させているため
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_Widget());
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_The_Fool());
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_The_Fool_IO());
    }

    puts("=== 項目 17 ：自動的に生成される特殊メンバ関数を理解する END");
}