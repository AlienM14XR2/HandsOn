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

ファイルは $HOME/dev/c++/HandsOn/bin/tmp/ に置く。

*/
#include <iostream>
#include <thread>
#include <future>
#include <mutex>
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
template<class M>
void (*ptr_lambda_K)(M) = [](auto message) -> void {
    cout << "Kスケ: " << message << endl;
};
template<class M>
void (*ptr_lambda_R)(M) = [](auto message) -> void {
    cout << "Rスケ: " << message << endl;
};
template<class M>
void (*ptr_lambda_M)(M) = [](auto message) -> void {
    cout << "松本: " << message << endl;
};
//grim reaper
template<class M>
void (*ptr_lambda_Grim_Reaper)(M) = [](auto message) -> void {
    cout << "死神: " << message << endl;
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
    string fileName = version + random + ".txt";
    return fileName;
}

// 興味がある、少し本題とは逸れるが、これも試したい。（お前なら分かるよな、俺 :)
// 折角遠回りするのだ、ここから見える景色を楽しもう。
// キャストだよ、兄貴、オレが再三、催促したのに無視（忘）しやがって。
class IRepository {
public:
    // バッチ（笑　に見える、止めよう。
    // virtual void batch_w() 
    virtual void batchWrite() const = 0;
    virtual ~IRepository() {}
};

// pure virtual も持つものをInterface （インタフェース）とここでは呼ぶことにする。
// インタフェースへのアップキャストはできる、予想通り。しかし、ダウンキャストはできなかった。
// そこで、pure virtual ではないRepository を作って、ダウンキャストを試してみたい。
//
// ダウンキャストが出来なかったのは、単なる使い方が間違ってたから。
// 考え方はあってたよ。寧ろ、pure virtual を持たない、インタフェースではない
// こちらの方が、コンパイラに注意される、実行はできるけどね。
// なので、インタフェースを推奨で問題ない。
class Repository {
public:
    virtual void batchWrite() {
        ptr_lambda_M<const string&>("本当にやるんですか、Rスケさん。");
        ptr_lambda_R<const string&>("あぁ、これはプロジェクトとは関係ない、あくまで俺個人のことだからな。");
    }
};

/**
 * 大事な事を忘れるとこだった、動作確認をすれば、直ぐに気づくことだとは思うが。
 * リマインドだ、インタフェースで操作する場合は、派生クラスのメンバ関数に 
 * virtual 必須ですから。
 * 派生クラスが final とか、そんなん関係ないからね。
 * インタフェースで操作したいなら、派生クラスのメンバ関数は virtual に。
 * 大事なことなので2回言いました。
 * 
 * ここから、Repository を使った、アップキャスト、ダウンキャスト
 * の試行錯誤が続きますよ。（はい、本編関係なし。
*/

class TextRepo : public virtual IRepository  {      // ダウンキャストしたい場合は継承時、virtual を付ける。
public:
    virtual void batchWrite() const override {
        ptr_lambda_K<const string&>("インタフェースでの操作なんだろ、兄貴、そうだよな。");
        ptr_lambda_R<const string&>("焦るな Kスケ。今、それを確認するところなんだ。");
    }
};

class TextRepo_D : public virtual Repository {
public:
    virtual void batchWrite() {
        ptr_lambda_K<const string&>("兄貴と連絡が取れない？");
        ptr_lambda_K<const string&>("アニキには兄貴の考えがあってのことだろう。");
        ptr_lambda_K<const string&>("オレは兄貴を信じる。");
    }
};

class MySqlRepo final : public virtual IRepository {
public:
    virtual void batchWrite() const override {
        ptr_lambda_K<const string&>("兄貴がやりたかったことはこれか。");
    }
};

// これなら、コンパイラも理解してくれるはずだ。
class MySqlRepo_B final : public virtual TextRepo {
public:
    virtual void batchWrite() {
        ptr_lambda_K<const string&>("兄貴がやりたかったことはできたのか？");
        ptr_lambda_R<const string&>("あぁ、どうやらそのようだな、だが、少し考えを改める必要がありそうだ。");
        ptr_lambda_R<const string&>("今回は理想通りだったが、次も同じように行くとは限らないんだ。");
        ptr_lambda_debug<const string&, const int&>("Rスケが言ったようにこれはオレの理想的な使い方で定義だ。だが、次のものはコンパイラには warning 警告が出る。実行確認してみるといい。",0);
    }
};

class MySqlRepo_D final : public virtual TextRepo_D {
public:
    virtual void batchWrite() {
        ptr_lambda_R<const string&>("俺は、また先輩と走れて、満足です。");
        ptr_lambda_Grim_Reaper<const string&>("馬鹿かお前は、オレは本気でお前を... 。");
        ptr_lambda_R<const string&>("感じませんでしたか、来てたんですよ、かおりさんが。");
        ptr_lambda_Grim_Reaper<const string&>("... 。");
    }
};

void test_Repository_Cast() {
    cout << "--------------------------- test_Repository_Cast" << endl;
    TextRepo text;
    IRepository& interface = static_cast<IRepository&>(text);
    interface.batchWrite();
    ptr_lambda_R<const string&>("あぁ、これは確かに、インタフェースでの操作には違いない。");
    ptr_lambda_R<const string&>("だが、アップキャストができることは、ある程度予想できたんだ、Kスケ。");
    ptr_lambda_R<const string&>("今、俺達がやらなければいけないこと、それが何か分かるか？、Kスケ。");
    ptr_lambda_K<const string&>("！？ なんなんだよ兄貴、それは！。");
    ptr_lambda_R<const string&>("ダウンキャストさ（キリッ");
    try {
        MySqlRepo& mysql = dynamic_cast<MySqlRepo&>(interface);
        mysql.batchWrite();
    } catch(exception& e) {
        cerr << e.what() << endl;
        ptr_lambda_R<const string&>("こいつは、とんだ大誤算だったな。");
        ptr_lambda_K<const string&>("！？ どうしたんだ兄貴。");
        ptr_lambda_R<const string&>("できない、このままでは、ダウンキャストはできないんだ、Kスケ。");
        ptr_lambda_R<const string&>("だが、俺のテクに乱れはないぜ（キリッ");
        // さすがにこれは無理か、実体である、TextRepo と MySqlRepo にはなんの関係もないからな。
        // なら、その関係があったらどうなのかな。
    }
    try {
        MySqlRepo_B* mysql = dynamic_cast<MySqlRepo_B*>(&interface); 
        mysql->batchWrite();
        // コンパイルは通るが実行時エラーになる。
        // オレはまだ諦めてないよ（飽きてないよ。
        // それはポインタにしてなかったからだね。
//        MySqlRepo_B& mysqlRef = dynamic_cast<MySqlRepo_B&>(interface);    // これは、NG。確かめたかったら、このコメントを外せば、確認できるよ。
    } catch(exception& e) {
        // これが出力されなければ、オレの勝ちだよ。
        cerr << e.what() << endl;
        ptr_lambda_R<const string&>("俺の負けだ、F原（GCC）、俺は手など抜いていない。");
        ptr_lambda_R<const string&>("お前の勝ちだよ、F原。");
    }
}

template <class T>
void printType(const T& x) {
    const type_info& type = typeid(x);
    if(type == typeid(Repository)) {
        ptr_lambda_debug<const string&,const int&>("type is Repository.",0);
    } else if(type == typeid(TextRepo_D)) {
        ptr_lambda_debug<const string&,const int&>("type is TextRepo_D.",0);
    } else if(type == typeid(MySqlRepo_D)) {
        ptr_lambda_debug<const string&,const int&>("type is MySqlRepo_D.",0);
    } else {
        ptr_lambda_debug<const string&,const int&>("type is unknown.",0);
    }
}

void test_Repository_Cast_D() {
    cout << "--------------------------- test_Repository_Cast_D" << endl;
    Repository repo;
    repo.batchWrite();

    TextRepo_D text;
    // text.batchWrite();
    printType(text);
    Repository& mock = static_cast<Repository&>(text);
    mock.batchWrite();
    printType(mock);

    MySqlRepo_D* mysql = dynamic_cast<MySqlRepo_D*>(&text);
    mysql->batchWrite();
}



//
// ここから、本編再開です。
//


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
    // DBのコネクションを取得して次のループでレコードの書き込み。まだだよ、それはどうするかまだ決めてない。
/*
  A. オレの予想だが、この関数は、スレッドセーフではないと思っている。
  複数のスレッドから、同時に書き込みに利用されたら、標準ライブラリのストリーム
  では無理だろう。次のことがよくわかっていない。

  B. スレッドは、独立してあり、各々メモリに乗る。別領域でこの関数が実行され
  た場合はスタックとして扱われるのか、それであれば、関数内の変数はAuto変数と
  して、安全に管理されているのか。独立して、すべてがメモリにのるのか。

  オレはPromiseとFutureを利用して、worker を通してこの関数を利用している。
  それは、B を期待しているからだ。この仕組みなしでは A だと思っている。

  実際色々と疑心暗鬼なのだが、始めてのファイル書き込みは、上手くいったよう
  に見える。まだ、スレッドは 2 だしな。
  ここから、少しずつスレッド数を増やして、CPUの動向を見守るかな。
  スレッド 8 も問題なかった、書き込みの件数もすべてあっていた（目視確認によるもの、ただし、ファイルのバイト数も全て同じなので、問題なしと判断した。

  では、一気に上げて 16 だ。それらしくなってきたな、CPUコアの一つが頂点近くをついた。
  32 ではどうなるのか。成功したぞ。次が最初の目標であった 48 スレッドだな。

  find . -type f | wc -l
  でファイル数

  ls -al
  でそのファイルサイズを確認しながらやっている。

  48 スレッドだな。（ここまで、音楽も聞きながら非常にCPUも安定している。：）
*/  
    MockPerson data;
    data.email = "alienm14xr2@loki.org";
    data.name = "Alien M14xR2";

    string fileName = getRandomFileName();
    fileName = "./tmp/" + fileName;
    cout << fileName << endl;
    std::ofstream writer;
    writer.open(fileName, std::ios::out);
    try {
        int i = 0;
        for(; i < LOOP; i++) {
            writer << i << '\t' << data.email << '\t' << data.name << endl;
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
    try {
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
    } catch(exception& e) {
        cerr << "exception is ... " << e.what() << endl;
    }
    size_t multi = thread::hardware_concurrency();
    ptr_lambda_debug<const string&, const size_t&>("can I use some threads ? \t",multi);
    cout << "===== B =====" << endl;
}

int main() {
    cout << "START スレッド02 =============== " << endl;
    // threads(48);
    // test_Repository_Cast();
    // test_Repository_Cast_D();
    clock_t start = clock();
    threads_02(THREAD_COUNT);
    cout << "THREAD_COUNT is \t" << THREAD_COUNT << endl;
    cout << "LOOP is \t" << LOOP << endl;
    clock_t end = clock();
    cout << "passed " << (double)(end-start)/CLOCKS_PER_SEC << " sec." << endl;
    cout << "=============== スレッド02 END " << endl;
    return 0;
}
