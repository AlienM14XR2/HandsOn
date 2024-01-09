/**
 * 6 章 Adapter パターン、Observer パターン、CRTP パターン
 * 
 * CRTP（Curiously Recurring Template Pattern）... 直訳 ... 奇妙なことに繰り返し発生するテンプレート パターン。
 * 奇妙に再帰したテンプレートパターンという。
 * 
 * ガイドライン 26：型を静的に区別するには CRTP パターン
 * 
 * CRTP パターンが生まれた背景
 * C++ では性能は非常に重要視されます。仮想関数のオーバーヘッドですら許容できないという場面もあるほどです。
 * コンピュータゲームや超高頻度の株取引など、性能が重要視される場面では仮想関数は使用されません。
 * ハイパフォーマンスコンピューティング（HPC）でもそうです。HPC では、重要演算の一番内側のループなど、性能が
 * 重要視される場面では、仮想関数も含め、どんな種類の条件分岐も間接参照も使用しません。性能に関するオーバーヘッドが
 * 発生するためです。
 * 
 * 解明 CRTP パターン
 * CRTP パターンも基底クラスによる抽象化という一般的な考えを基にしていますが、基底クラスと派生クラスの関係を、
 * 仮想関数を用いて実行時に構築するのではなく、コンパイル時に構築します。
 * 
 * CRTP パターン
 * 目的：型ファミリのコンパイル時抽象化を定義する。
 * 
 * CRTP パターンの短所を分析
 * 最初に挙げる短所はもっとも大きな制約でもある、共通する基底クラスが存在しない点です。
 * その影響を強調するためにもう一度いいましょう。共通基底クラスは存在しません。
 * 実質的にすべての派生クラスには、それぞれ異なる基底クラスがあります。
 * 例えば、DynamicVectro<T> クラスには DenseVector<DynamicVector<T>> 基底クラスが、
 * StaticVector<T> クラスには DenseVector<StaticVector<T>> 基底クラスがあります。
 * 例えば、何らかのコレクション内に異なる型を保持するなどの場合では、CTRP パターンは正しい解にはなりません。
 * 
 * CTRP パターンは高い柔軟性を備えていない（Visitor パターンならば std::variant が使えますし、Adapter なども同様です）。
 * 
 * CRTP パターンは実行時多態ではなく、コンパイル時多態という機能しか備えていない点です。
 * なんらかの静的な抽象化の場面でのみ使えるデザインパターンであり、それ以外の場面で継承階層を置き換えるなどできません。
 * 
 * e.g. compile
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_6_CRTP.cpp -o ../bin/main
*/
#include <iostream>
#include <vector>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

/**
 * DenseVector という全種類の密集ベクトルをサンプルに解説している（書籍が）。
 * 
 * 次のクラスは、抽象化とすべての密ベクトルに共通のインタフェースを表現するという点はそのままに、
 * 派生クラスの具象実装にアクセスもコールもできるのです。
*/

template <class Derived>
struct DenseVector {
protected:
    ~DenseVector() = default;   // 仮想関数をすべて排除したい以上、仮想デストラクタもその対象になる。
                                // そのため、デストラクタは protected な非仮想関数として実装します。
    /**
     * 基底クラスのデストラクタは public な仮想関数、もしくは protected な非仮想関数とするべきである。
     * 
     * しかし、デストラクタをこのように定義すると、コンパイラは 2 つのムーブ（ムーブコンストラクタとムーブ演算子）
     * を生成できなくなります。
     * CRTP 基底クラスは通常中身を持たないためムーブするものなど存在せず問題にはならないが、五関数同時ルール
     * は肝に銘じておくべきです。
    */

    // ... 次のメソッド以外にもあると思ってください。
public:
    // using value_type     = typename Derived::value_type;
    // using iterator       = typename Derived::iterator;
    // using const_iterator = typename Derived::const_iterator;

    size_t size() const {
        return static_cast<const Derived&>(*this).size();
        /**
         * static_cast により自身を派生クラスの参照へ型変換し、派生クラスの size() 
         * をコールします。一見すると再帰コールですが（size() 関数が size() をコールする）、実際には
         * 派生クラスの size() メンバ関数をコールしています。
         * 
         * この凄さと利用シーンがまだイメージできない。
         * 
         * コンパイル時の関係構築、つまり、基底クラスは具象派生クラスとその実装詳細を抽象化し、それでいて実装詳細が
         * どこにあるかも全部分かる。
         * 
         * CRTP パターンでは、共通インタフェースを実装し、コールされたら static_cast し派生クラスへ転送するだけです。
         * そして性能はまったく低下しません。
        */
    }

    Derived& derived()          { return static_cast<Derived&>(*this); }
    Derived& derived() const    { return static_cast<const Derived&>(*this); }

    /**
     * ここまでの整理で、コードは綺麗に見通せるようになっただけでなく、DRY 原則にも従っており、かつ警戒すべき箇所などは見当たらない。
     * 次は、添字演算子と begin() end() 関数を実装します。
    */

    // decltype は、オペランドで指定した式の型を取得する機能である。
    // ただの auto では将来の変更に対応できない。
    // 派生クラスが返却するものそのまま返せば問題はなく、これを表現するのが decltype(auto) になる。
    decltype(auto) operator[](size_t index)        { return derived()[index]; }
    decltype(auto) operator[](size_t index) const  { return derived()[index]; }

    decltype(auto) begin()         { return derived().begin(); }
    decltype(auto) begin() const   { return derived().begin(); }
    decltype(auto) end()           { return derived().end(); }
    decltype(auto) end() const     { return derived().end(); }

    // ...
};

template <class T>
class DynamicVector : public DenseVector<DynamicVector<T>> {
public:
    void add(const T& value) {
        values_.push_back(value);
    }
    size_t size() const {
        return values_.size();
    }
private:
    std::vector<T> values_;
};

/**
 * 案外書籍が不親切で、次のテストは私の興味で実装したもの。
 * 書籍には記載はなかった：）
*/

int test_Vector() {
    puts("--- test_Vector");
    try {
        DynamicVector<int> dv;
        dv.add(0);
        dv.add(1);
        dv.add(2);
        dv.add(3);
        dv.add(4);
        ptr_lambda_debug<const char*,size_t>("dv.size() is ",dv.size());

        DenseVector<DynamicVector<int>>* interface = static_cast<DenseVector<DynamicVector<int>>*>(&dv);
        ptr_lambda_debug<const char*,size_t>("interface->size() is ",interface->size());

        DynamicVector<int> dv2 = interface->derived();
        ptr_lambda_debug<const char*,size_t>("dv2.size() is ",dv2.size());

        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

int main(void) {
    puts("START CRTP パターン ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const double&>("pi is ",pi);
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_Vector());
    }
    puts("=== CRTP パターン END");
    return 0;
}
