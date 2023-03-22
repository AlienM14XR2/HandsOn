/**
ここから、M15 R7 との戦いの準備だ。
MySQL への書き込みへの下準備になるかな。
まず、MySQLでやる前にファイルにデータを書き込むだけの処理をやってみる。

なんとなく、ファイル書き込みの方が難しい気がしている。
一般的には次のように考えるのではないか、大量の書き込みデータがある場合は
まず、スレッド毎にファイルを作成してしまう。その後、生成されたファイルを
一つにまとめる。なぜそうするかといえば、書き込み管理をスレッドでやりたく
ないからだ。

よし、null で書き込む、なんだっけ、忘れた。データを永続化しない 
/tmp/null だったかな、判然としないが。
/dev/null かな。

書き込みより前に、そもそも幾つスレッドがあげられるか試そう。
ちょっとだけ、算数だ。

key word
- 400 threads per 1 sec
- 800 through put per 1sec

リードの情報だからアテにはならないかもしれないが,
- Java + MongoDB 50 ms
- C++ + MySQL 2ms

ここだけ切り取れば、C++の方が25倍速いと言える。
25分の1が許容される範囲かな。

400 / 25 = 16

16 スレッドでスループットが800でれば五分だということかな。
あぁ、これだとぴったり60秒だから、負け確定か。
うん、じゃ切りよく20スレッドで。
50スレッドとかダメなのかな。

うん、試してみよ。
前述のことから、ひとっ飛びで DB 接続をやってみようかな。（もっと、楽しもうよ :)
スレッド数が 100 未満なら、コネクションを増やさずにいけるはず。

現状は、計算が楽なので、48 スレッドで 各スレッド内で 1000 ループ
とした。

OK、今日はここまで。次は各スレッド毎にファイルにデータを書き込む。
勿論、ファイルは個別に作成しよう。どうなるかな。

*/
#include <iostream>
#include <thread>
#include <future>
#include <mutex>
#include <vector>
#include <iomanip>

using namespace std;

template<class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};

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

int write() {
    // DBのコネクションを取得して次のループでレコードの書き込み。
    int i = 0;
    for(; i < 1000; i++) {
    }
    ptr_lambda_debug<const string&,const string&>("version is ", toStringDateTime());
    ptr_lambda_debug<const string&,const int&>("loop is ", i);
    return 0;
}

void worker(promise<int> promise_) {
    try {
        promise_.set_value(write());
    } catch(exception& e) {
        promise_.set_exception(current_exception());
    }
}

// void (*p_worker)(promise<int>) = worker;

void threads(const int& sum) {
    cout << "---------------------------- threads " << endl;
    try {
        for(int i = 0;i < sum; i++) {
            promise<int> p;
            future<int> f = p.get_future();
            thread t{worker,move(p)};
            ptr_lambda_debug<const string&,const int&>("ret is ",f.get()); 
            t.join();
            // これだと意図したものではない。これは単発実行に過ぎない
            cout << "===== A =====" << endl;
        }
        size_t multi = thread::hardware_concurrency();
        ptr_lambda_debug<const string&, const size_t&>("can I use some threads ? \t",multi);
    } catch(exception& e) {
        cerr << "exceptoin is ... \t" << e.what() << endl;
    } 
}

void threads_02(const int& sum) {
    cout << "---------------------------- threads_02 " << endl;
    mutex mutex_;
    vector<thread> threads(sum);
    for(thread& t: threads) {
        promise<int> p;
        future<int> f = p.get_future();
        t = thread{worker,move(p)};
    }
    // これでイメージ通りのはず :)
    for(thread& t: threads) {
        t.join();
    }
    size_t multi = thread::hardware_concurrency();
    ptr_lambda_debug<const string&, const size_t&>("can I use some threads ? \t",multi);
    cout << "===== B =====" << endl;
}

int main() {
    cout << "START スレッド02 =============== " << endl;
    // threads(48);
    threads_02(48);
    cout << "=============== スレッド02 END " << endl;
    return 0;
}
