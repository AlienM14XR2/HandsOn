/**
 * 3 章 現代の C++ への移行
 * 
 * 項目 15 ：可能な場面では常に constexpr を用いる
 * 
 * C++11 で最も混乱する新用語を表彰すれば、恐らく constexpr が受賞するでしょう。
 * オブジェクトに使用すれば本質的な意味は強化された const ですが、関数に使用した場合の意味は大きく異なります。
 * この混乱はくぐり抜ける価値があります。表現したい内容が constexpr にふさわしい場面では、間違いなく使用した
 * くなるでしょう。
 * 
 * constexpr は、その値が単なる定数というよりも、コンパイル時に既知の定数であることを概念的に表現します。
 * この基礎概念から出発し、関数に対して constexpr を用いた場合は他にも多くのニュアンスを含みます。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_3_modern_c++_constexpr.cpp -o ../bin/main
*/
#include <iostream>
#include <array>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](const auto message, const auto debug) -> void {
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
 * 配列の要素数をコンパイル時定数として返す（要素数のみを考慮するため、仮引数の配列に名前はない）。
*/
template <class T, std::size_t N>
constexpr std::size_t arraySize(T(&)[N]) noexcept {
    return N;
}

int test_arraySize() {
    puts("--- test_arraySize");
    try {
        int keyVals[] = {1,3,7,9,11,22,35};
        int mappedVals[arraySize(keyVals)];
        size_t mappedValsSize = sizeof(mappedVals) / sizeof(mappedVals[0]);
        ptr_lambda_debug<const char*,const size_t&>("mappedValsSize is ", mappedValsSize);
        // もちろん、現代の C++ 開発者ならば、組み込み配列よりも std::array の方が当然好みでしょう。
        std::array<int, arraySize(keyVals)> mappedVals2;
        ptr_lambda_debug<const char*,const size_t&>("mappedVals2 size is ", mappedVals2.size());
        
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}
/**
 * 1 章ですでに上記のようなサンプルがあった。
*/

constexpr
int pow( int base, int exp ) {
    if(exp == 0) return 1;      // 0 の階乗は 1
    int ret = base;
    for(int i = 1; i <exp; i++) {
        ret *= base;
    }
    return ret;
}

constexpr
int pow2(int base, int exp) {
    return (exp == 0 ? 1 : base * pow2(base, exp-1));
}

enum struct DB_POW_FIELDS {
    base,
    exp,
};

int readFromDB(const DB_POW_FIELDS field) {       // 本来は DB 接続し、実行時にはじめて値の返却ができるものと想定する。
    int ret = 1;
    switch(field) {
        case DB_POW_FIELDS::base:
            ret = 2;
            break;
        case DB_POW_FIELDS::exp:
            ret = 8;
            break;
        default:
            ret = 1;
            break;
    }
    return ret;
}

int test_pow() {
    puts("--- test_pow");
    try {
        constexpr auto numConditions = 8;
        auto result = pow(2, numConditions);
        ptr_lambda_debug<const char*,const decltype(result)&>("result is ", result);
        /**
         * 上記は pow() の実引数、base と exp がコンパイル時定数ならば、pow の結果もコンパイル時定数となる
         * 可能性があるという意味です。
        */

        auto base = readFromDB(DB_POW_FIELDS::base);
        auto exp  = readFromDB(DB_POW_FIELDS::exp);
        ptr_lambda_debug<const char*,const decltype(result)&>("result is ", pow2(base,exp));
        /**
         * base と exp のどちらか 1 つでもコンパイル時定数でなければ、pow の結果は実行時に算出されます。
         * つまり上記の readFromDB のような実行時に決定される場合でも呼び出し可能となる。
        */
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << "ERROR: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}

/**
 * constexpr 関数が受け取れる型、返せる型はリテラル型に制限されます。すなわち、実質的に、コンパイル時に決定される値を
 * 保持できる型です。C++11 では、コンストラクタおよびメンバ関数を constexpr にできるため、void 以外のすべての組み込み型
 * およびユーザ定義型がリテラル型になれます。
 * C++14 では 戻り値型が void でも constexpr の宣言が可能になった。
*/

class Point {
public:
    constexpr Point(double _x, double _y): x{_x}, y{_y}
    {}
    // ...
    constexpr double getX() noexcept { return x; }
    constexpr double getY() noexcept { return y; }
    constexpr void   setX( double _x ) noexcept { x = _x; }
    constexpr void   setY( double _y ) noexcept { y = _y; }
private:
    double x, y;
};

int test_Point() {
    puts("--- test_Point");
    try {
        Point p{3.33, 6.00};
        ptr_lambda_debug<const char*,const double&>("x is ", p.getX());
        ptr_lambda_debug<const char*,const double&>("y is ", p.getY());
        puts("---");
        p.setX(12.05);
        p.setY(9.99);
        ptr_lambda_debug<const char*,const double&>("x is ", p.getX());
        ptr_lambda_debug<const char*,const double&>("y is ", p.getY());
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << "ERROR: " << e.what() << endl;
        return EXIT_FAILURE;
    }
}

int main(void) {
    puts("START 項目 15 ：可能な場面では常に constexpr を用いる ===");
    if(0.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_debug());
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_arraySize());
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_pow());
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ", test_Point());
    }
    puts("=== 項目 15 ：可能な場面では常に constexpr を用いる END");
}