/**
 * 6 章 ラムダ式
 * 
 * 項目 32 ：クロージャ内にオブジェクトをムーブする場面では初期化キャプチャを用いる
 * 
 * デフォルトでは、ラムダから生成したクロージャクラス内のメンバ関数 operator() は const です。これにより、ラムダ本体内では、クロージャ内の
 * 全メンバ変数が const になるという効果が生まれます。
 * 
 * 重要ポイント
 * - クロージャ内へオブジェクトをムーブするには C++14 の初期化キャプチャを使用する。
 * - C++11 では手書きクラスや std::bind により初期化キャプチャをエミュレートする。
 * 
 * g++ -O3 -DDEBUG -std=c++20 -pedantic-errors -Wall -Werror chapter_6_lambda_32.cpp -o ../bin/main
*/
#include <iostream>
#include <memory>
#include "stdio.h"
#include "stdlib.h"
#include "memory.h"
#include "string.h"

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](const auto message, const auto debug) -> void {
    std::cout << "DEBUG: " << message << '\t' << debug << std::endl;
};

template <class Error>
concept ErrReasonable = requires(Error& e) {
    e.what();
};
template <class Error>
requires ErrReasonable<Error>
void (*ptr_print_error)(Error) = [](const auto e) -> void {
    std::cout << "ERROR: " << e.what() << std::endl; 
};

int test_debug_and_error() {
    puts("=== test_debug_and_error");
    try {
        // ptr_print_now();
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
 * 
 * External Polymorphism について復習したくなった。
 * 
 * - 概念として、何らかのコンセプトを表現する基底クラスがある、Model の基底クラス。
 * - DefaultStrategy と operator 関数
 * - Model と Strategy の分離、Model に Strategy の依存注入を行う。
 * - 具体的な処理を担う Strategy クラス。operator() 関数の実装、この関数は仮引数の型（Model）によりオーバーロードする。
*/

/**
 * Coffee Break C 言語
 * 
 * 以前、考えた簡易メモリ管理を再実装してみる。
 * 
 * - {} 任意のブロックの上下でそれぞれ、メモリの取得と解放を行う仕組み。
*/

// void c_version(void) {
//     printf("C supported version __STDC_VERSION__ is %ld\n",__STDC_VERSION__);
// }


void debug_ptr_d(const char* message, const int* debug) {
    printf("DEBUG: %s\tvalue = %d\taddress = %p\n",message,*debug,(void*)debug);
}

class Gadget {
};

void block_a() {
    puts("--- block_a");
    using HEAP_SIZE = std::size_t;
    const HEAP_SIZE _INT_HEAP_SIZE_ = 100;
    HEAP_SIZE intCounter = 0;
    int* INT_HEAP = (int*)malloc(sizeof(int*)*_INT_HEAP_SIZE_);
    auto useIntHeap = [&_INT_HEAP_SIZE_, &intCounter, &INT_HEAP] {
        int* heap = nullptr;
        if(intCounter < _INT_HEAP_SIZE_) {
            heap = &INT_HEAP[intCounter++];
        }
        return heap;
    };

    const HEAP_SIZE _GADGET_HEAP_SIZE_ = 10;
    HEAP_SIZE gadgetCounter = 0;
    Gadget* GADGET_HEAP = new Gadget[_GADGET_HEAP_SIZE_];
    auto useGadgetHeap = [&_GADGET_HEAP_SIZE_, &gadgetCounter, &GADGET_HEAP] {
        Gadget* heap = nullptr;
        if(gadgetCounter < _GADGET_HEAP_SIZE_) {
            heap = &GADGET_HEAP[gadgetCounter++];
        }
        return heap;
    };
    if(1)   // この記述も必要ない。
    {
        puts("------ in block");
        ptr_lambda_debug<const char*,const decltype(_INT_HEAP_SIZE_)&>("INT HEEP SIZE: ", _INT_HEAP_SIZE_);
        int x = 99;
        int* px = useIntHeap();
        px = &x;
        debug_ptr_d("x",&x);
        debug_ptr_d("px",px);

        Gadget g;
        Gadget* pg = useGadgetHeap();
        pg = &g;
        ptr_lambda_debug<const char*, const Gadget*>("g addr is ", &g);
        ptr_lambda_debug<const char*, const Gadget*>("pg addr is ", pg);
        // ... このブロックでは必ず useIntHeap を通してメモリを利用して、個別の malloc、free は行わないこと。
    }
    puts("--- free");
    free(INT_HEAP);
    delete [] GADGET_HEAP;
    /**
     * 上例のようにすれば、プロシージャでも関数でも、メモリの解放忘れは極力防げるはず。
     * 結局 raw ポインタの効率には、コピー演算もムーブ演算も敵わないんだから。
     * しかし上例が、ラムダ式を使っているように、これは C コンパイラでのコンパイルは不可能、したがって C++ コンパイラを利用する必要がある。
     * 即ち、 std::unique_ptr と std::make_unique を使うべきだということは、肝に銘じておくこと。
    */
}

int test_block_a() {
    puts("=== test_block_a");
    try {
        block_a();
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

/**
 * ここから本項に戻る。
 * 値キャプチャでも参照キャプチャでも目的に適わない場合があります。クロージャにムーブ専用オブジェクトを持ち込みたい場合
 * （std::unique_ptr や std::future など）、C++11 ではどうすることもできません。コピーはコスト高だけれどムーブはコスト安なオブジェクトをクロージャ
 * に持ち込む場合は（標準コンテナの大半がそう）、コピーするよりムーブしたい気持ちがずっと強いでしょう。しかし、この場合でも、C++ では実現する方法
 * がありません。
 * しかし、C++14 では違います。C++14 ではオブジェクトをクロージャ内へムーブする直接的な記述が可能です。C++14 標準のコンパイラを使用しているならば、
 * 喜びも新たに読み進めてください。C++11 でも擬似的にムーブキャプチャする方法はあります。
 * 
 * 標準化委員会が導入したのは新方式です。ムーブキャプチャは新方式がなせる技の 1 つに過ぎません。この新方式を『初期化キャプチャ』と言います。
 * 
 * 初期化キャプチャでは次のものを記述できます。
 * 1. ラムダから生成するクロージャクラスの『メンバ変数の名前』
 * 2. メンバ変数を初期化する式
 * 
 * 初期化キャプチャにより、std::unique_ptr をクロージャ内へムーブする方法を示します。
*/

class Widget {
public:
    // ... 
    bool isValidated() const {
        return true;
    }
    bool isProcessed() const {
        return false;
    }
    bool isArchived() const {
        return true;
    }
};

int test_lambda_and_move() {
    puts("=== test_lambda_and_move");
    try {
        auto pw = std::make_unique<Widget>();

        auto func = [pw = std::move(pw)]() -> auto {
            return pw->isValidated() && pw->isArchived();
        };
        /**
         * 面白いことに「=」の左辺と右辺ではスコープが異なります。左辺のスコープはクロージャクラスのスコープですが、右辺のスコープはラムダを定義
         * した時点のスコープ。上例では、「=」の左辺にある pw という名前はクロージャクラスのメンバ変数を表し、右辺にある pw はラムダ直前で宣言し
         * たオブジェクト、すなわち std::make_unique 呼び出しで初期化した変数を表します。
        */

        ptr_lambda_debug<const char*, const bool&>("func result is ", func());
        return EXIT_SUCCESS;
    } catch(std::exception& e) {
        ptr_print_error<const decltype(e)&>(e);
        return EXIT_FAILURE;
    }
}

int main(void) {
    puts("START 項目 32 ：クロージャ内にオブジェクトをムーブする場面では初期化キャプチャを用いる ===");
    if(0.01) {
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_debug_and_error());
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_block_a());
    }
    if(1.00) {
        ptr_lambda_debug<const char*, const int&>("Play and Result ... ", test_lambda_and_move());
    }
    puts("=== 項目 32 ：クロージャ内にオブジェクトをムーブする場面では初期化キャプチャを用いる END");
}