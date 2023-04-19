/**
    Prototype パターン

    まず、基本となるクラス群を用意してみる。
*/
#include <iostream>
#include <cassert>
#include <thread>
#include <future>
#include <vector>

using namespace std;

template <class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << message << '\t' << debug << endl;
};


template <class T>
class Prototype {
public:
    virtual ~Prototype() {} 
    virtual T clone() const = 0;
};

class Screw final : public virtual Prototype<Screw> {
    string type;
    int size;
public:
    Screw():type{""},size{-1} {}
    Screw(const string& t,const int& s) {
        type = t;
        size = s;
    }
    Screw(const Screw& own) {}
    ~Screw() {
        ptr_lambda_debug<const char*,const int&>("Called Screw Destructor.",0);
    }
    string getType() noexcept {
        return type;
    }
    int getSize() noexcept {
        return size;
    }
    virtual Screw clone() const override {
        Screw screw = Screw(type,size);
        return screw;
    }

};
int test_Screw() {
    cout << "--------------------------- test_Screw" << endl;
    Screw screw = Screw("test",10);
    Screw clone = screw.clone();
    cout << "screw is " << &screw << endl;
    cout << "clone is " << &clone << endl;
    assert( screw.getSize() == clone.getSize() );

    // Screw* screws = new Screw[5];
    // cout << "sizeof(*screws) is " << sizeof(*screws) << endl;
    Screw screws[5];
    cout << "sizeof(screws) is " << sizeof(screws) << endl;
    for(int i=0; i < 5 ; i++ ) {
        screws[i] = clone.clone();
    }
    cout << "screws[4].getSize() is " << screws[4].getSize() << endl;
    // delete [] screws;
    return 0;
}
/**
  この例でいうと、Prototype の派生クラスであるScrew のclone メンバ関数が処理を担っている。
  原型、ひな型の複製を上記で行っている。

  うん、ここからさらに、どんなふうに発展させてみようか？ここからが本当の、本来オレがやるべき学習のはずだ。
  これは、使い方次第でとても強力な武器にもなりそうなんだよね。クローンをキーワードに発想を展開させたら
  クローンの中でやっていることが複雑であればあるほど、その利用者は恩恵を受けられる。
  今回は、基底クラスのPrototype のジェネリック、T を派生クラスのScrew にしているが、別にそれはなんでもいい。

  今、面倒、複雑ということから、Thread Promise Future このワードが浮かんだ。
  なるほど、うん、楽しそうじゃないか：）では組み込んでみるか。

*/

// promise future
// worker compute
// thread 

int compute() {
    try {
        // do something.
        return 0;
    } catch(exception& e) {
        return -1;
    }
}
void worker(promise<int> promise_) {
    try {
        promise_.set_value(compute());
    } catch(exception& e) {
        promise_.set_exception(current_exception());
    }
}

/**
 スレッド、Promise Future の試行錯誤のクラス。
 結論から言えば、使えない。ThreadPrototype のメンバ関数同様
 クラススコープのメンバ関数であることは同じ、やってることは
 ThreadPrototype のメンバ関数と変わらない、よって、結果も同じ。
 コンパイルエラー：）
*/
// class Computing final {
// public:
//     Computing(){}
//     ~Computing(){}

//     int compute() {
//         try {
//             // do something.
//             ptr_lambda_debug<const string&,const int&>("compute ... ",0);
//             return 0;
//         } catch(exception& e) {
//             return -1;
//         }
//     }
//     void worker(promise<int> promise_) {
//         try {
//             promise_.set_value(compute());
//         } catch(exception& e) {
//             promise_.set_exception(current_exception());
//         }
//     }
// };

// 今回これは勉強になったし、調べる課題かも知れない。
// うん、上記の関数は本来、ThreadPrototype のメンバ関数にする予定だった、
// しかし、コンパイラに叱られた。スレッドに関しては、まだまだ、知らないことが
// 沢山ありそうだ。

class ThreadPrototype : public virtual Prototype<ThreadPrototype*> {
    promise<int> promise_;
    future<int> future_;
public:
    ThreadPrototype(){
        future_ = promise_.get_future();
    }
    ThreadPrototype(const ThreadPrototype& own){
        future_ = promise_.get_future();
    }
    virtual ~ThreadPrototype(){}
    virtual void doThread(const int& version) {
        ptr_lambda_debug<const string&,const int&>("IThreadPrototype doThread version is ... ",version);
        try {
            thread t{worker,move(promise_)};
            future_.get();
            t.join();
        } catch(exception& e) {
            cerr << e.what() << endl;
        }
    }
    // virtual void doThread(const Computing& computing) {
    //     ptr_lambda_debug<const string&,const int&>("IThreadPrototype doThread and args... ",0);
    //     try {
    //         // やはり、これでもダメだったか。クラススコープのメソッドの呼び出しが、叱られるようだね。
    //         thread t{computing.worker,move(promise_)};
    //         future_.get();
    //         t.join();
    //     } catch(exception& e) {
    //         cerr << e.what() << endl;
    //     }
    // }
    virtual ThreadPrototype* clone() const override {
        ThreadPrototype* threadProto = new ThreadPrototype();
        return threadProto;
    }
};
int test_Thread_Prototype() {
    cout << "--------------------------- test_Thread_Prototype" << endl;
    ThreadPrototype* threadProto = nullptr;
    threadProto = new ThreadPrototype();
    ThreadPrototype* tmp = threadProto->clone();
    try {
        threadProto->doThread(1);
        tmp->doThread(2);
        delete tmp;
        delete threadProto;
        // 内部でJoin しているので、こんな使い方にどれ程の意味があるのか、という気にはなるね。
        // スレッドをどんな目的で利用したいのか、という根本的な問に対する答えが明確でないといけないのか。
        // メインのプロセスと関係ない処理とした場合、Join する必要はないが、Heap メモリを利用していた場合。
        // その時、誰がその責務を負うのかという問題があるから、スレッドは管理が面倒、複雑だと言われるのだろうね。
        // うん、以前やったように、vector なりのコンテナを利用して、そのコンテナ内の処理は相互に干渉せず、また
        // 違った処理を行う。そんなのが、理想かな。前回のは結局ループなのでスレッドを利用する価値はあまりないとは思う。
        // 今はそんなふうに思ってる。それもこれも、C の速さが原因なのかな：）
    } catch(exception& e) {
        cerr << e.what() << endl;
        if( threadProto != nullptr ) {
            delete tmp;
            delete threadProto;
        }
        return -1;
    }
    return 0;
}

/**
  折角だから、Prototype とどれ程の関係性を持たせられるか、甚だ疑問ではあるが、現在、自分が考える
  最高の形のスレッド利用を実現してみたい。力試しかな。

  - スレッド内の処理は各々、別、同期はしない。ただし、メインスレッドとの同期は行う。（join.
  - Vector コンテナで矢継ぎ早にスレッドは起動させる。@see thread_02.cpp
  - クラスで管理できるものはクラス内に隠蔽する。thread future promise
  - worker とその内部関数、便宜的に worker’s method は複数用意する。

  現状の自分の知識と技術で実現できるのはこの辺りだろう。
  おおまかな、仕様は決まった。では早速実装してみる。 
*/
int compute_a(const int& version) {
    ptr_lambda_debug<const string&,const int&>("compute_a ... ",version);
    return 0;
}
int compute_b(const int& version) {
    ptr_lambda_debug<const string&,const int&>("compute_b ... ",version);
    return 0;
}
int compute_c(const int& version) {
    ptr_lambda_debug<const string&,const int&>("compute_c ... ",version);
    return 0;
}
void worker_a(promise<int> promise_, int version) {
    try {
        promise_.set_value(compute_a(version));
    } catch(exception& e) {
        promise_.set_exception(current_exception());
    }
}
void worker_b(promise<int> promise_, int version) {
    try {
        promise_.set_value(compute_b(version));
    } catch(exception& e) {
        promise_.set_exception(current_exception());
    }
}
void worker_c(promise<int> promise_, int version) {
    try {
        promise_.set_value(compute_c(version));
    } catch(exception& e) {
        promise_.set_exception(current_exception());
    }
}

class SandboxThread final : public virtual Prototype<SandboxThread*> {
    static const int THREAD_SIZE = 3;
public:
    SandboxThread(){
    }
    SandboxThread(const SandboxThread& own) {
    }
    ~SandboxThread() {}
    void doThread_A_B_C() {
        try {
            promise<int> promise_a, promise_b, promise_c;
            future<int> future_a = promise_a.get_future();
            future<int> future_b = promise_b.get_future();
            future<int> future_c = promise_c.get_future();
            vector<thread> threads(THREAD_SIZE);
            int i = 0;
            for(thread& t: threads) {
                switch(i) {
                    case 0: 
                        t = thread{worker_a,move(promise_a),move(i+1)};
                        future_a.get();
                        break;
                    case 1: 
                        t = thread{worker_b,move(promise_b),move(i+1)};
                        future_b.get();
                        break;
                    case 2: 
                        t = thread{worker_c,move(promise_c),move(i+1)};
                        future_c.get();
                        break;
                }
                i++;
            }
            for(thread& t: threads) {
                t.join();
            }
        } catch(exception& e) {
            cerr << e.what() << endl;
        }
    }
    virtual SandboxThread* clone() const override {
        return new SandboxThread();
    }
};
int test_Sandbox_Thread() {
    cout << "----------------------- test_Sandbox_Thread" << endl;
    try {
        SandboxThread sandbox;
        sandbox.doThread_A_B_C();
        SandboxThread* tmp = sandbox.clone();
        tmp->doThread_A_B_C();
        delete tmp;
    } catch( exception& e ) {
        cerr << e.what() << endl;
        return -1;
    }
    return 0;
}
// うん、スレッドはこんなんが、今は理想だと思う。
// 同じ処理やループ、同期を必要とするのは理想的ではない。
// 最終的には、prototype と関係なくなってる気もしなくはないね：）

int main() {
    cout << "START Prototype ===============" << endl;
    ptr_lambda_debug<const char*,const int&>("Here we go.",0);
    if(1) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ", test_Screw()); 
    }
    if(2) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ", test_Thread_Prototype());
    }
    if(3) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ", test_Sandbox_Thread());
    }
    cout << "=============== Prototype END" << endl;
    return 0;
}