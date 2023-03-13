/**
 * これは、短くまとめたい。
 * final class。そのクラスの継承を許さない、派生クラスを作らせないのが
 * 目的だよね。
 * 
 * Singleton、今回はこれにしよう、理由は特にない。
 * 作らせない繋がりかな。
*/
#include <iostream>
#include <cstring>
#include <iomanip>
#include <mutex>
#include <thread>

using namespace std;

/**
 * 未来のオレへ。
 * クラスのデフォルトスコープは private だよ。
 * アクセス指定子は記述してないけど、private ということ。
 * 
 * デフォルトコンストラクタを private として、自身をメンバ変数
 * にすることで、Singletonは実現できる、はず。
 * 少なくとも、基本はこれ。
 * 
 * C++でやってみる。
 * Singletonは予想以上に問題があることを知る。
 * https://refactoring.guru/ja/design-patterns/singleton/cpp/example
 * 読むべし。
 * 
 * 頭の片隅にはスレッドセーフのことはあった。
 * しかし、文法を学習中にはそこまで実践することは避けていた。
 * いずれまた通るはずだ。
 * 
 * うん、ほぼ上のURLのマルコピに近いな。
 * 
*/

string toStringDateTime() {
    time_t t = time(nullptr);
    const tm* localTime = localtime(&t);
    std::stringstream s;
    s << localTime->tm_year + 1900;
    // setw(),setfill()で0詰め
    s << setw(2) << setfill('0') << localTime->tm_mon + 1;
    s << setw(2) << setfill('0') << localTime->tm_mday;
    s << setw(2) << setfill('0') << localTime->tm_hour;
    s << setw(2) << setfill('0') << localTime->tm_min;
    s << setw(2) << setfill('0') << localTime->tm_sec;
    // std::stringにして値を返す
    return s.str();
}

class Singleton final {     // はい今回の文法、継承無効の山場です、これだけです。
    const string version = "Singleton-v-"+toStringDateTime();
    static Singleton* me;
    static std::mutex mutex_;
    Singleton() {
    }
public:
    void operator=(const Singleton &) = delete;
    Singleton(Singleton &other) = delete;
    ~Singleton() {
        delete me;
    }
    const string& getVersion() const {
        return version;
    }
    static Singleton* getInstance() {
        std::lock_guard<std::mutex> lock(mutex_);
        if( me == nullptr ) {
            me = new Singleton();
        }
        return me;
    }
};
// これはクラススコープでは記述不可能（構文エラー）だった。
// 宣言は可能だが、初期化は無理。
// 自身をメンバ変数にしているためだと思う。
// つまり、Singletonにしたら、初期化は外部で行う必要があるのかと。
//
// static だからといって必ずクラス外で初期化する必要はないし、構文エラー
// でもない。ただし、今回は自身のインスタンス化なのでクラススコープでは
// 不可能という意味。
Singleton* Singleton::me{nullptr};
std::mutex Singleton::mutex_;

void threadFoo() {
    // Following code emulates slow initialization.
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    Singleton* singleton = Singleton::getInstance();
    cout << singleton->getVersion() << "\t\n";
}

void threadBar(){
    // Following code emulates slow initialization.
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    Singleton* singleton = Singleton::getInstance();
    std::cout << singleton->getVersion() << "\t\n";
}

int main() {
    cout << "START main ========== " << endl;
    Singleton* sp = Singleton::getInstance();
    string version = sp->getVersion();
    cout << "version is " <<  version << endl;

    std::cout <<"If you see the same value, then singleton was reused (yay!\n" <<
                "If you see different values, then 2 singletons were created (booo!!)\n\n" <<
                "RESULT:" << endl;;

    std::thread t1(threadFoo);
    std::thread t2(threadBar);
    t1.join();
    t2.join();
    cout << "========== main END " << endl;
    return 0;
}

