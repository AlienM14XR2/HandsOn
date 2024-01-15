/**
 * 8 章 Type Erasure パターン
 * 
 * ガイドライン 32 Type Erasure パターンでの継承階層の置換を検討する
 * 
 * 本書において繰り返し提言していること。
 * - 依存関係を最小化せよ。
 * - 関心を分離せよ。
 * - 継承よりコンポジションを優先せよ。
 * - 既存コードに干渉しない解を優先せよ。
 * - 参照セマンティクスより値セマンティクスを優先せよ。
 * 
 * External Polymorphism パターンの値セマンティクスベースの実装モデル。
 * この技術を一般に Type Erasure （型消去）技術と言います。
 * 
 * 著者が知る限り、この技術が最初に考案されたのは 2000 年の Kevin Henney の論文です。
 * これは標準ライブラリにも採用されており、すでに紹介した、std::function や std::any
 * がそれにあたります。
 * std::function は、callable を値ベースに抽象化します。また、std::any は仮想的にどんな
 * ものでも（原語は anything。名前の由来）、その内容／機能を隠蔽したまま、コンテナライク
 * な値に抽象化します。
 * 
 * 解明 Type Erasure パターン
 * Type Erasure パターンの核となる 3 つのデザインパターンとは、External Polymorphism パターン（分離及び既存コードに干渉しないようにする。）、
 * Bridge パターン（構造の分離、値セマンティクスベースの実装を担う。）、Prototype パターン（用いない場合もある。作成した値型のコピーセマンティクスを実現する。）です。
 * しかし、この 3 つに限定されているわけではなく、組み合わせは文脈や意味付け、実装次第で変わることもあります。
 * この 3 つの組み合わせの重要な点は、既存コードに干渉せず、関係性を希薄にする抽象化のラッパクラス作成にあります。
 * 
 * Type Erasure パターン
 * 目的：次の性質を持つ拡張可能な型セットを対象に、既存コードに干渉せず値ベースに抽象化する。
 *       - 潜在的に非多態
 *       - 意味的に同一動作
 *       - 互いに関係を持たない
 * 
 * この目的は、正確さを損なわずにできるだけ簡潔に定義しましたが、内容を補足します。
 * 
 * 値ベース
 *      Type Erasure パターンの目的は、コピー可能、ムーブ可能、推論可能な値型の作成にある。
 *      しかしこの値型は正則な値型の要件を厳密には満たしておらず、二項演算では一部に制限がある、
 *      単項演算では最適に動作する。
 * 
 * 非干渉的
 *      Type Erasure パターンは、External Polymorphism パターンを用いた非干渉的な外部抽象化の
 *      作成を目的としている。この抽象化が要件とする動作を持つ既存の型は、すべて自動的に対象
 *      となり、修正する必要はない。
 * 
 * 拡張可能で互いに関係を持たない型セット
 *      Type Erasure パターンはオブジェクト指向の原則を確固たる基盤にしており、型の追加を容易
 *      にする。しかし型同士は関係を持つべきではなく、基底クラスを介した共通動作などは持たない。
 *      その代わり要件を満たす型ならば、変更することなく追加可能である。
 * 
 * 潜在的に非多態
 *      External Polymorphism パターンの時にも述べたが、型には継承を強制すべきではない。仮想
 *      関数なども持たせるべきではなく、多態な動作とは分離すべきである。しかし、基底クラスを
 *      持つ型、仮想関数を持つ型を完全排除するわけではない。
 *      
 * 意味的に同一動作
 *      すべての型を抽象化することが目的ではなく、同じ動作（同じ規約）を持ち、LSP に従い、要件
 *      を満たす一連の型を、意味的に抽象化することを目的とする。可能であれば、要件を満たさない
 *      型はコンパイルエラーとすべきである。     
 * 
 * ガイドライン 32 の要約
 * - 非多態ながら意味的に同じ動作を持ち、互いに関係を持たず、拡張可能な一連の型を既存コードに干渉せず、
 *   値ベースに抽象化する目的を持って、Type Erasure パターンを適用する。
 * 
 * - Type Erasure パターンは External Polymorphism パターン、Bridge パターン、Prototype パターンを
 *   組み合わせたデザインパターンと考える。
 * 
 * - Type Erasure パターンの長所を理解する。同時にその制限も忘れてはならない（等価演算の扱い解釈について）。
 * 
 * - Type Erasure パターンという用語は、限定された処理セットを持つ型の新規追加を容易にする目的を持つ
 *   デザインパターン、という意味でのみ使用する。
 * 
 * e.g. compile.
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_8_type_erasure.cpp -o ../bin/main
*/
#include <iostream>
#include <any>
#include <cstdlib>
#include <string>
#include <cassert>
#include <memory>
#include <utility>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M, D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

int sample_1() {
    puts("--- sample_1");
    try {
        std::any a;         // create an empty 'any'
        a = 3;              // Storing an 'int' inside the 'any'
        a = "some string"s; // Replacing the 'int' with a 'std::string'

        // There is nothing we can do with the 'any' except for getting the value back.（any は値を取り出すことぐらいしか出来ない。）
        std::string s = std::any_cast<std::string>(a);
        ptr_lambda_debug<const char*,const string&>("s is ",s);
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

int sample_2() {
    /**
     * その他には、std::shared_ptr があります。与えられたデリータを内部に保持する際に型消去技術を使用しています。
    */
    puts("--- sample_2");
    try {
        // 専用デリータを指定し std::shared_ptr を作成する。デリータは型の一部ではない点に注意！
        std::shared_ptr<int> shared{
            new int(42)
            , [](int* ptr){
                delete ptr;
            }
        };

        // スコープから抜ければ std::shared_ptr は破棄され、int は専用デリータが delete する。
        int result = -1;
        ptr_lambda_debug<const char*,const int&>("*shared.get() is ",result = *shared.get()); 
        assert(result == 42);
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}

/**
 * 所有権を持つ Type Erasure パターンの実装
 * 図形とその描画に関してサンプル実装を行う。
*/

class Circle {
private:
    double radius;      //  半径
public:
    explicit Circle(double _radius):radius(_radius) {}
    double getRadius() const { return radius; }
};

class Square {
private:
    double side;        // 横幅
public:
    explicit Square(double _side):side(_side) {}
    double getSide() const { return side; }
};

/**
 * 上記の 2 つのクラスにおける最も重要な点は、どの基底クラスからも派生しておらず、自ら仮想関数を持つこともない、
 * 非多態なクラスであるということです。
*/

namespace detail {      // namespace を利用しているその意味を考えてみよう。

/**
 * ShapeConcept クラス
 * 仮想関数 draw() を持ち、図形描画の要件を表現します。
*/
class ShapeConcept {
public:
    virtual ~ShapeConcept() = default;
    virtual void draw() const = 0;
    virtual std::unique_ptr<ShapeConcept> clone() const = 0;
};

/**
 * OwningShapeModel クラス
 * ShapeConcept の派生クラスで図形とその描画の抽象処理を実装している。
 * 具体的な実装ではなくあくまでテンプレートによる実装である。
*/
template <class ShapeT
            ,class DrawStrategy>
class OwningShapeModel : public ShapeConcept {
private:
    ShapeT shape;
    DrawStrategy drawer;
public:
    explicit OwningShapeModel(ShapeT _shape, DrawStrategy _drawer):shape{std::move(_shape)}, drawer{std::move(_drawer)}
    {}
    void draw() const override {
        drawer(shape);
    }
    std::unique_ptr<ShapeConcept> clone() const override {
        return std::make_unique<OwningShapeModel>(*this);
    }
};

/**
 * Shape クラス
 * OwningShapeModel クラステンプレートのインスタンス化、ポインタ管理、
 * メモリ割当、ライフ管理等のラッパクラス。
*/
class Shape {
private:
    std::unique_ptr<detail::ShapeConcept> pimpl;
public:
    // Shape で最も重要なものが、このテンプレートコンストラクタ
    template <class ShapeT,class DrawStrategy>
    Shape(ShapeT _shape, DrawStrategy _drawer) {
        using Model = detail::OwningShapeModel<ShapeT, DrawStrategy>;
        // 新規作成したモデルを用いて pimpl を初期化すます。
        pimpl = std::make_unique<Model>(std::move(_shape), std::move(_drawer));
    }
    /**
     * このクラスが Bridge パターンになっている。
     * 渡された ShapeT と DrawStrategy を基に OwningShapeModel をコンストラクトし、
     * ShapeConcept を指すポインタとして保持します。
     * これにより実装詳細への橋（Bridge）、及び実際の図形クラスへの橋ができます。
     * しかし、pimpl の初期化とコンストラクタが完了すると、Shape はもう実際の型を覚えていません。
     * Shape はテンプレート引数も、具象型を返すメンバ関数も持っておらず、渡された型を保持するメンバ変数も持っていません。
     * ShapeConcept 基底クラスを指すポインタしか持っていないのです。
     * このため、実際の図形クラスは完全に消去されます。この動作がデザインパターンの名前、型消去（Type Erasure）の由来です。
     * 
     * この Shape クラスに欠けているものは実際の値型に必要な機能、コピー演算とムーブ演算です。
     * 幸いにも std::unique_ptr を用いているため、一部のみ対処すれば済みます。コンパイラが生成するデストラクタと 2 つの
     * ムーブ演算は十分に機能するため、対処しなければならないのはコピー演算 2 つだけです。
    */

    // コピーコンストラクタは、実装するのが難しい関数かもしれません。other の Shape 内にある、具象図形クラスを知らないためです。
    // しかし、ShapeConcept 基底クラスに clone() 関数があるため、具象クラスを知らないままコピーが得られます。
    Shape(const Shape& other) : pimpl( other.pimpl->clone() )
    {}

    // コピー代入演算子の簡便で手軽な実装方法は copy-swap イディオムです。
    Shape& operator=(const Shape& other) {
        // Copy-and-Swap Idiom
        Shape copy(other);
        pimpl.swap(copy.pimpl);
        return *this;
    }

    ~Shape() = default;
    Shape(Shape&&) = default;
    Shape& operator=(Shape&&) = default;
private:
    // この friend 関数はフリー関数ですが Shape 内で定義されているため、『隠された friend 関数』とも呼ばれます。
    // フレンド関数である以上、private なメンバ変数を使用でき、名前空間にも属せます。
    // この関数はメンバ関数とすることも勿論できます。
    // しかし、著者はフリー関数を選択しました。その理由は、ここでの最終目的が draw() 処理を切り出し、依存関係を削減する
    // ことにあるためです。
    friend void draw(const Shape& shape) {
        shape.pimpl->draw();
    }
};
}   // namespace detail

int test_Shape() {
    puts("--- test_Shape");
    try {
        Circle circle{3.14};
        // ラムダ式で描画 strategy を作成
        auto drawer = [](const Circle& c) {
            puts("------ circle drawer (strategy)");
            ptr_lambda_debug<const char*,const double&>("radius is ",c.getRadius());
        };

        // Shape 抽象化で図形と描画 strategy を組み合わせる。このコンストラクタが、渡した円とラムダ式を用いて、
        // detail::OwningShapeModel をインスタンス化する。
        detail::Shape shape1(circle, drawer);
        // コピーコンストラクトにより図形のコピーを作成する。
        detail::Shape shape2( shape1 );
        // コピーの方を描画しても結果は同じになる。
        draw( shape2 );
        return EXIT_SUCCESS;
    } catch(exception& e) {
        cout << e.what() << endl;
        return EXIT_FAILURE;
    }
}


int main(void) {
    puts("START Type Erasure パターンでの継承階層の置換を検討する ===");
    if(0.01) {
        double pi = 3.141592;
        ptr_lambda_debug<const char*,const int&>("pi is ", pi);
    }
    if(1.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",sample_1());
    }
    if(1.01) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",sample_2());
    }
    if(2.00) {
        ptr_lambda_debug<const char*,const int&>("Play and Result ... ",test_Shape());
    }
    puts("=== Type Erasure パターンでの継承階層の置換を検討する END");
    return 0;
}