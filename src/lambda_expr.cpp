/**
ラムダ式（C++11）-----------------------------------------------------------

lambda expression

https://cpprefjp.github.io/lang/cpp11/lambda_expressions.html
「ラムダ式(lambda expressions)」は、簡易的な関数オブジェクトをその場で
 定義するための機能である。
 この機能によって、「高階関数(関数を引数もしくは戻り値とする関数)」を
 より使いやすくできる。そして...
```
 auto plus = [](int a, int b) { return a + b; };
int result = plus(2, 3); // result == 5
```

流石、大先生は言うことが、うちのリファレンス先生とは違うのだよ。
うちの先生のはじめのことばは次のだもの。
「ラムダ式は、簡潔に関数オブジェクトを記述する機能です。以下のように記述します。」
そして...
```
[キャプチャ](仮引数リスト)->戻り値の型{ 複合分 }
```
はぁ？

プロブラマがほしい情報、サンプルとともにズバッと言ってくださる、大先生は
流石です。ここはうちのは、ほっときます。大先生に師事を仰ぐとしよう。



ラムダ、これは利用したことはあっても、中身をよく知らない。

気になったのは （C++20）から導入された、*this のキャプチャ。

*this をキャプチャすると、そのクラスのオブジェクトをコピーするので、寿命切れを
心配する必要がないとのこと。

これは、写経してみるか。

ジェネリックラムダ（C++14）だと

初期化キャプチャ（C++14）だと

ジェネリックラムダのテンプレート構文（C++20）なに？

一応、動きそうなものから、写経かな。
*/

#include <iostream>

using namespace std;

struct F {
  auto operator()(int a, int b) const -> decltype(a + b)
  {
     return a + b;
  }
};
/**
 * 大先生ね。
 * 
ラムダ式によって自動的に定義される関数オブジェクトは、それぞれが
一意な型名を持ち、その型名をユーザーが知る方法はない。そのため、
ラムダ式によって定義された関数オブジェクトを変数に持つためには、
autoを使用して型推論するか、std::functionクラスの変数に保持するかの、
いずれかの方法をとることになる。

ラムダ式からその外側の変数を使用するには、「キャプチャ(capture)」という機能を
使用する。ラムダ式の先頭にある[ ]は「ラムダ導入子(lambda-introducer)」
と呼ばれ、どの変数をどのようにキャプチャするかを、
このラムダ導入子のなかで指定する。キャプチャの方式としては、
参照かコピーのいずれかを選択できる。
*/
void f()
{
  int x = 3;

  // この時点で見える自動変数を参照でキャプチャし、
  // 参照した変数xを書き換える
  auto f = [&] { x = 1; };

  // 個別にキャプチャ方法を指定する場合は、
  // 以下のように、&の次に変数名を指定する
  // auto f = [&x] { x = 1; };

  f(); // x == 1
  cout << "x is " << x << endl;
  // 流石、大先生、納得です。
}

void g()
{
  int x = 3;

  // この時点で見える自動変数をコピーでキャプチャし、
  // コピーした変数xをラムダ式内で使用する
  auto f = [=] { return x + 1; };

  // 個別にキャプチャ方法を指定する場合は、
  // 以下のように、変数名を指定する
  // auto f = [x] { return x + 1; };

  int result = f(); // result == 4
  cout << "result is " << result << endl;
  // 流石、大先生、納得です。
}

// this のキャプチャ
class X {
    int x_ = 1;
public:
    void foo() {
        auto f = [this] {
            // ラムダ式内で、Xクラスのメンバ変数やメンバ関数を参照する。
            // private メンバ関数も呼び出せる。ん？特別みたいな言い方だけど
            // クラススコープ内の話なのだから、別段、普通ではないのか。
            return x_ + bar();
        };
        // ここからオレのDebug
        int ret = f();
        cout << "ret 大先生の力をかりて、うちの先生のDEBUG ようやく確認できた。=== " << ret << endl;

        // あれ、ゴメンナサイ、リファレンス先生。
        // 次の一行の写経にしくじったオイラのミスっぽいな。
        // うん、遠回りして、寧ろ理解が深まったと前向きに捉えよう。
        // 大先生のとこにも行ったし。
        cout << f() << endl;    // 4 が出力される。
    }
    // このメソッドはリファレンスにはないよ。
    int getx_() {
        return x_;
    }
private:
    int bar() const {
        return 3;
    }
};

// C++17 以降は、*this をキャプチャする方法が追加されました。
// *this をキャプチャすると、そのクラスのオブジェクトをコピーしますので、寿命切れ
// を心配する必要がありません。
// *this のキャプチャ
class Y {
    int y_ = 1;
public:
    void foo() {
        auto f = [*this] {
            return y_ + bar();
        };
        // ラムダ式で*this をコピーキャプチャしたあとに
        // メンバ変数を書き換えても、ラムダ式の呼び出しに影響はない。
        y_ = 2;
        cout << f() << endl;    // 4 が出力される。
    }
private:
    int bar() const {
        return 3;
    }
};

// 何もキャプチャしないラムダ式は、関数ポインタへの代入ができる。
void (*ptr_to_lambda)() = []{
    cout << "ウェーイ、オイラ、無名関数だよ。変なポインタに入れられた、萎えるわ。" << endl;
};
// これは面白かも。

// これはオイラが少し手をくわえたもの。
// コンパイルは通るのか、実行できるのか。
void (*ptr_to_lambda_x)(const char* cch) = [](const char* cch)->void{
    cout << "ウェーイ、オイラ、無名関数だよ。変なポインタに入れられた、" << cch << " 。" << endl;
};

// ジェネリックラムダ（C++14）、だと！、いやもうちょっと見たから。
// ラムダ式の仮引数には、具体的な型のほかに、auto（確か型推論してくれる）
// も指定できます。
auto myLambda = [](auto a, auto b) {
    cout << "I'm by val." << endl;
    return a + b;
};
auto myLambda_ref = [](const auto& a, const auto& b) {
    cout << "I'm by ref." << endl;
    return a + b;
};

// 初期化キャプチャ（C++14）、だと！
// ラムダ式は、変数を指定するキャプチャのほかに、初期化方法も指定できます。
int lx = 2;
// 変数 lx を ly という変数でコピーキャプチャ
auto myLambda_init_capture = [ly = lx](const int& lz) {
    return ly + lz;
};

// ジェネリックラムダのテンプレート構文（C++20）、だと！何？
/*```
[]<class T>(T a) {・・・}
[]<class T>(T* p) {・・・}
[]<class T, int N>(array<T,N>& arr) {・・・}
```*/
// 飽きた
// 関数テンプレートでいいや。
// また、戻ってくるかも、今はもういいや。

int main() {
    cout << "START ラムダ式（C++11）========================= " << endl;
    // これは、大先生のとこのサンプルね。
    auto plus = [](int a, int b){ return a+b; };
    int result_first_imp = plus(2,5);
    cout << "result_first_imp is " << result_first_imp << endl;
    f();
    g();

    // ここからうちのリファレンス先生の解説 DEBUG。
    // this のキャプチャ -----------------------------------------
    // うちの投げっぱなし先生のサンプルは動かして確認中ね。
    X x;
    x.foo();
    int ret = x.getx_();
    cout << "つまり、あくまでラムダ内の話なのだね。ret is " << ret << endl;

    // *this のキャプチャ -----------------------------------------
    Y y;
    y.foo();
    // *this のキャプチャは、非同期で処理を実行した最後にラムダ式を呼び出す、
    // というような状況で有用です。とうちのリファレンス先生が仰っております。

    // 関数ポインタでラムダ式を呼び出している。
    ptr_to_lambda();
    ptr_to_lambda_x("やる気でるわ");

    ret = myLambda(3,6);
    ret = myLambda_ref(3,6);
    cout << "myLambda(3,6) is " << ret << endl;
    cout << "myLambda_ref(3,6) is " << ret << endl;
    ret = myLambda_init_capture(10);
    cout << "myLambda_init_capture(10) is " << ret << endl;
    cout << "========================== ラムダ式（C++11）END" << endl;
    return 0;
}