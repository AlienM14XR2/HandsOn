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
    // ... 次のメソッド以外にもあると思ってください。

    size_t size() const {
        return static_cast<const Derived&>(*this).size();
        /**
         * static_cast により自身を派生クラスの参照へ型変換し、派生クラスの size() 
         * をコールします。一見すると再帰コールですが（size() 関数が size() をコールする）、実際には
         * 派生クラスの size() メンバ関数をコールしています。
         * 
         * この凄さと利用シーンがまだイメージできない。
        */
    }
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
