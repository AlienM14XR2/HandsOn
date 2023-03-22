/**
スレッド -----------------------------------------------------

<thread>
std::thread

cv修飾、メンバ関数の const cv 修飾は、スレッドセーフな設計では必須だと知る。
複数のスレッドから同時に同じオブジェクトのconstメンバ関数を呼び出しても、
データ競合は発生しません。と言い切るリファレンス先生。

スレッドを排他制御する
<mutext>
std::mutex

std::lock_guard
リソースの排他制御をシンプルに管理できる。
mutext等のlock() unlock() を持つクラスを第一引数にとり
コンストラクタ内で受け取った引数のオブジェクトのlock()を呼び出し、
管理しているオブジェクトのデストラクタが呼び出されると、unlock() 
を呼び出す。といっているな。

<future>
std::promise
std::future
でスレッド間で例外あるいは値、オブジェクトを扱うはず。

スレッドへ渡す引数
値、参照、move()などがある。参照で非constの場合は、std::ref() constな
参照型の時は std::cref()、明示的にコピーを避けたい場合がmove()。

>>> std::atomic
>>> によるロックをせず排他アクセスをする仕組み。
>>> アトミック変数は正しく利用するのが難しいといってるぞ。
>>> じゃ、いらない。

スレッドをまたいで値や例外を受け渡す
<future>
std::promise
std::future
これは必須だな。

非同期処理
std::async()

スレッドローカル変数
thread_local int tli = 0;


頭の整理をしよう。

- スレッドではGoFは使わない、既にネタがある。
- 最終目標というか、やってみたい事は、やはり M15 R7 との対決だ。
- 秒間 400 スレッド立ち上げ、48.000件（1レコードは80 bytes? 程度）を 50 秒台で
  喰らい尽くす、その速さ。リソースの配分しだいでは、40秒台中盤という
  記録もある。
- CPU の処理量、次は M14xR2 が圧倒的に不利な戦場だとは思う、ここで戦ってみたい。

予想、勝てないだろ、これは。 :)

M15 R7
```
JMeter + Node.js + MongoDB
```

M14xR2
```
C++マルチスレッド + MySQL
```
M15 の構成には一分のスキもない、プログラマ（オレ）が唯一の弱点であり、そのプログラマ
が介在した部分でさえ極小、という。

段階的に遊んでみようかな。

まずは、<future>
std::promise
std::future

これらの使い方をしっかりと理解したい。
*/
#include <iostream>
#include <thread>
#include <future>
#include <mutex>

using namespace std;

template<class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};


//
// スレッドをまたいで、値や例外を受け渡す。（<future> C++11）
// まずは、写経から。
//

int computeSomethig() {
    return 3;
}

void worker(promise<int> promise_) {        // この辺の考え方はすでに JavaScript で勉強したけど、こっちが元なんだろうな。
    try {
        // 何か処理をして、その結果を promise に設定する。
        // throw runtime_error("A. 動作確認エラーを発生させた。これは少し興味深いね、これはコアダンプにならないが。");
        promise_.set_value(computeSomethig());
        // throw runtime_error("B. これは出力すらされず、中止、コアダンプになるはず。実行時のエラーメッセージの通りなのだが、つまり、set_value()の後はいかなる処理もやってはいけない、return だと思っとく。");
    } catch(exception& e) {
        // 例外が発生したら、例外を promise に設定する。
        promise_.set_exception(current_exception());
    }
}

void test_sample_promise_future_thread() {
    cout << "-------------------------- test_sample_promise_future_thread " << endl;
    // 次のコードは一連のお決まりだな。
    promise<int> promise_01;
    future<int> future_01 = promise_01.get_future();
    thread thread_01{worker,move(promise_01)};
    // ここまでの動作確認かな。どうでもいいこと、でもなんか気になるの int 数値だったら、num それ以外は val や value って書きたい。
    // リファレンスは全部 value で統一してるけど。
    try {
        cout << "(future_.get()) num is ... \t" << future_01.get() << endl;
        // オレは意地悪なんじゃないよ、知りたいだけ。
        // throw runtime_error("C. 動作確確認エラー。");
    } catch(std::exception& e) {
        // cout << "error is " << e.what() << endl;    // 標準出力でいいの？サンプルだからかな。
        cerr << "error is \t" << e.what() << endl;
    }
    thread_01.join();
}

int main() {
    cout << "START スレッド01 =============== " << endl;
    ptr_lambda_debug<const string&,const int&>("デバッグの準備はOK？ 0 is OK.",0);
    test_sample_promise_future_thread();
    // OK promise future thread は試したぞ、自分の認識が深まったと思いたい。
    // 後は使い倒すしかないな。
    // thread 処理においてこの機構は必須だと思ってる。
    // mutex の排他制御は今回はやらない、以前やったし、それを見よう。（ final_class.cpp
    // Ok、次だ。
    cout << "=============== スレッド01 END" << endl;
    return 0;
}