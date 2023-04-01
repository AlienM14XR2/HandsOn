/** 
 * =======================================================================
 * thread_03.cpp
 * 
 * 懐かしいぞ、再び会えて幸せだ。
 * 早速だが、本題だ。
 * 
 * ここでは、以前行った、48 スレッドのファイル書き込み
 * ＋ワンファイルへの合成を行う。
 * 
 * やばい、本気で忘れてるかも、ゴメンね。
 * 少し、記憶の整理も兼ねてオレの暖気運転をしてみるか。
*/

#include <iostream>
#include <thread>   // C++11
#include <future>   // C++11
#include <vector>
#include <iomanip>
#include <random>
#include <typeinfo>
#include <fstream>
#include <time.h>

#define THREAD_COUNT 48
#define LOOP         1000

using namespace std;

template<class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << "DEBUG: " << message << '\t' << debug << endl;
};
// 折角だから、彼らにも来てもらおうね。
template<class M, class D>
void (*ptr_lambda_R)(M,D) = [](auto message, auto debug) -> void {
    cout << "Rスケ: " << message << '\t' << debug << endl;
};
template<class M, class D>
void (*ptr_lambda_K)(M,D) = [](auto message, auto debug) -> void {
    cout << "Kスケ: " << message << '\t' << debug << endl;
};

string random_ds() {
    cout << "-------------------- random_ds " << endl;
    // メルセンヌ・ツイスター法による擬似乱数生成器を、
    // ハードウェア乱数をシードにして初期化
    std::random_device seed_gen;
    std::mt19937 engine(seed_gen());
    // 一様実数分布
    // [-1.0, 1.0)の値の範囲で、等確率に実数を生成する
    std::uniform_real_distribution<> dist1(-1.0, 1.0);
    double r1 = dist1(engine);
    if(r1 < 0) {
        r1 *= -1;
    }
    return std::to_string(r1);
}

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

string getRandomFileName() {
    string random = random_ds();
    string version = toStringDateTime();
    string fileName = version + random;
    return fileName;
}

/**
 * これをファイルに書き込む。
 * 既に、この時点でデータ量が M15 R7 より多いな。
 * M15 R7 の MongoDB へは名前プラスアルファだったと記憶してる。
 * DB ではないから、調節しようね。
 * 
 * email name これだけでいいでしょ。80 bytes 程度なら。
*/
struct MockPerson {
// 型は厳密ではない。
    long pid;           // Null NO  PRI    auto_increment
    string address;     // Null YES
    string email;       // Null YES UNI
    string entryAt;     // Null NO  mysql datetime(6) C++ での正しい型がなんなのか知らない。
    string memo;        // Null YES
    string name;        // Null YES
    string password;    // Null YES
    string phone;       // Null YES
    string status;      // Null NO
    string updateAt;    // Null NO mysql datetime(6) C++ での正しい型がなんなのか知らない。
};

int write() {
    MockPerson data;
    // これでフルでデータは埋めた。
    data.address = "Tokyo Japan.";
    data.email = "alienm14xr2@loki.org";
    data.entryAt = "2023-03-23 20:45:00";
    data.memo = "オレだ。";
    data.name = "Alien M14xR2";
    data.password = "alien5678";
    data.phone = "090xxxxyyyy";
    data.status = "1";
    data.updateAt = "2023-04-02 01:56:00";

    string fileName = getRandomFileName();
    fileName = "./tmp/" + fileName + ".txt";
    cout << fileName << endl;
    std::ofstream writer;
    writer.open(fileName, std::ios::out);
    try {
        int i = 0;
        for(; i < LOOP; i++) {
            writer << i << '\t' << data.email << '\t' << data.name << '\t' << data.address  << '\t' << data.entryAt << '\t' << data.memo << '\t' << data.password << '\t' << data.phone << '\t' << data.status << '\t' << data.updateAt <<  endl;
        }
        writer.close();
        ptr_lambda_debug<const string&,const int&>("loop is ", i);
        return 0;
    } catch(exception& e) {
        cerr << e.what() << endl;
        if(writer.is_open()) {
            writer.close();
        }
        return -1;
    }
}

void worker(promise<int> promise_) {
    try {
        promise_.set_value(write());
    } catch(exception& e) {
        promise_.set_exception(current_exception());
    }
}

void threads(const int& sum) {
    cout << "---------------------------- threads " << endl;
    // これは、何に使おうとしたのか、スマン忘れた。いらないはず。
    // mutex mutex_;
    try {
        vector<thread> threads(sum);
        for(thread& t: threads) {
            promise<int> p;
            future<int> f = p.get_future();
            t = thread{worker,move(p)};
            // オレはこの記述を軽視していた、関係ないと、違った、これがないと実行ファイルになった後もCPUを消費する、確認した。
            // Promise Future はここまでやってはじめて意味を持つ、決して忘れない。
            // リファレンスやGoogleで得た知見ではない。自分が経験したことだ、それが、大切なことを教えてくれる。
            ptr_lambda_debug<const string&,const int&>("worker ... write result is ",f.get()); 
        }
        // これでイメージ通りのはず :)
        for(thread& t: threads) {
            t.join();
        }
    } catch(exception& e) {
        cerr << "exception is ... " << e.what() << endl;
    }
    size_t multi = thread::hardware_concurrency();
    ptr_lambda_debug<const string&, const size_t&>("can I use some threads ? \t",multi);
    cout << "===== Lambda Rスケ =====" << endl;
}

int main() {
    cout << "START C VS C++ Rd.1 ===============" << endl;
    ptr_lambda_debug<const string&, const int&>("これがオレのデバッグだ",0);
    ptr_lambda_K<const string&,const int&>("兄貴、あのハチロク（C 言語）には相当なABS（管理機能）が付いてるぜ。",0);
    ptr_lambda_R<const string&,const int&>("Kスケ、ボケたこと言ってんなよ、ハチロク（C 言語）にABS（管理機能）なんか付いてるわけないだろ。",0);
    ptr_lambda_R<const string&,const int&>("そもそも40年も昔の車（言語）だ、ABS（管理機能）はおろか、\nノーマルでも100馬力程度（バッファオーバーフロー）の、\n旧車（バッファオーバラン上等）だ。",0);
    ptr_lambda_R<const string&,const int&>("Kスケがいうようにそのハチロクが、\n化け物だとしたら、それはドライバーだ。",0);
    // おふざけはここまでだ。
    // 普通に考えたらC++ の方が C より分が悪い、
    // 48 回ファイルの開閉を行う必要があるから。
    // まぁ後は以前のコピペか、ワンファイルの合成までは。
    clock_t start = clock();
    threads(THREAD_COUNT);
    cout << "THREAD_COUNT is \t" << THREAD_COUNT << endl;
    cout << "LOOP is \t" << LOOP << endl;
    clock_t end = clock();
    cout << "passed " << (double)(end-start)/CLOCKS_PER_SEC << " sec." << endl;
    cout << "=============== C VS C++ Rd.1 END" << endl;
    return 0;
}
