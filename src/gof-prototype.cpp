/**
    Prototype パターン

    まず、基本となるクラス群を用意してみる。
*/
#include <iostream>
#include <cassert>
#include <thread>
#include <future>

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

int main() {
    cout << "START Prototype ===============" << endl;
    ptr_lambda_debug<const char*,const int&>("Here we go.",0);
    if(1) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ", test_Screw()); 
    }
    if(2) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ", test_Thread_Prototype());
    }
    cout << "=============== Prototype END" << endl;
    return 0;
}