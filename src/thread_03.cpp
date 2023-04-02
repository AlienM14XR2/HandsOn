/* * 
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
 * 
 * find . -type f | wc -l
 * でファイル数
 * 
 * ls -al
 * でそのファイルサイズを確認しながらやっている。
 * 
 * クラスを使おうか、学習の一環なのだし。
 * 全てのスレッドが完了した後はクラスでワンファイルにマージしてみよう。
 * そのクラスをコンポジットにしよう。絶対に派生クラスでディレクトリ階層を
 * 操作することはないんだけどさ。こっちは無駄を楽しもう、ある意味、勝負を
 * 投げたような発言に聞こえるかもしれないが。OOPは捨てたくないよね、実務
 * に影響でそうだし。
 * 
 * 軽量化に勝るチューニングなし。
 * 
 * これをコーディングに照らし合わせたら、コード量を減らし、演算を減らした
 * 徹底的な処理の最適化が必要なのだとは思う。それはさ、C でやろうよ。：）
 * 
 * ここで、おそらく同期が必要かなと思う、別のやり方もありそうだが、ここは
 * 無駄を省いて。。。同期の方が無駄なのか。
 * OK、前言撤回、ファイル名の取得は system 関数で ”ls” を試してみよう。
 * 後は、まぁいいか。
 * 
 * 
 * */

#include <iostream>
#include <thread>   // C++11
#include <future>   // C++11
#include <vector>
#include <iomanip>
#include <random>
#include <typeinfo>
#include <fstream>
#include <time.h>
#include <string>
#include <cstring>

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

void println(const string& message) {
    cout << message << endl;
}

string random_ds() {
    // cout << "-------------------- random_ds " << endl;
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
    // cout << fileName << endl;
    std::ofstream writer;
    writer.open(fileName, std::ios::out);
    try {
        int i = 0;
        for(; i < LOOP; i++) {
            writer << i << '\t' << data.email << '\t' << data.name << '\t' << data.address  << '\t' << data.entryAt << '\t' << data.memo << '\t' << data.password << '\t' << data.phone << '\t' << data.status << '\t' << data.updateAt <<  endl;
        }
        writer.close();
        // ptr_lambda_debug<const string&,const int&>("loop is ", i);
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
        vector<thread> threads(sum);
        for(thread& t: threads) {
            promise<int> p;
            future<int> f = p.get_future();
            t = thread{worker,move(p)};
            // オレはこの記述を軽視していた、関係ないと、違った、これがないと実行ファイルになった後もCPUを消費する、確認した。
            // Promise Future はここまでやってはじめて意味を持つ、決して忘れない。
            // リファレンスやGoogleで得た知見ではない。自分が経験したことだ、それが、大切なことを教えてくれる。
            int result = f.get();
            // ptr_lambda_debug<const string&,const int&>("worker ... write result is ",result); 
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

//
// C++ らしくここからはクラスの定義。
//

// GoF Composite
class Component {
public:
    virtual void merge() const = 0;
//    virtual string merge(const string& baseDir) const = 0;
    virtual ~Component() {
        ptr_lambda_debug<const string&,const int&>("DONE Component Destructor.",0);
    }
};
class Leaf final : public Component {
    string name;
    string outputMerge;
    Leaf():name{"-1"},outputMerge{"-1"} {}
public:
    Leaf(const string& path) {
        name = path;
        // outputMerge にマージしたファイル名を保存して。
        outputMerge = "initial_C++_fin.txt";
    }
    Leaf(const Leaf& own) {
        name = own.name;
        outputMerge = own.outputMerge;
    }
    virtual void merge() const override {
        // ファイルストリームを作成してワンファイルに出力する。
        // ここは淡々と二重ループかな。
        //
        // 1. initial_C++.txt からファイル名を取得する。(read)
        // 2. 1 で取得したファイル名から実際のデータが存在するファイルストリームを作成する。(read)
        // 3. スレッドで書き込んだデータの読み込み。
        // 4. スレッドで書き込んだデータの書き込み。
        // こんな手順を考えてる。
        ifstream reader;
        reader.open(name,std::ios::in);
        string line;
        ofstream writer;
        writer.open(outputMerge,std::ios::out);
        int i = 0;
        try {
            while(getline(reader,line)) {
                // ptr_lambda_debug<const string&,const string&>("line is ",line);
                ifstream reader_td;
                string td_fileName = "./tmp/" + line;
                reader_td.open(td_fileName,std::ios::in);
                string data;
                try {
                    while(getline(reader_td,data)) {
                        writer << data << endl;
                    }
                    reader_td.close();
                } catch(exception& e) {
                    cerr << e.what() << endl;
                    if( reader_td.is_open() ) {
                        reader_td.close();
                    }
                }
            }
            writer.close();
            reader.close();
        } catch(exception& e) {
            cerr << e.what() << endl;
            if( writer.is_open() ) {
                writer.close();
            }
            if( reader.is_open() ) {
                reader.close();
            }
        }
    }
    string getName() noexcept {
        return name;
    }
    string getOutputMerge() noexcept {
        return outputMerge;
    }
    ~Leaf() {
        ptr_lambda_debug<const string&,const int&>("DONE Leaf Destructor.",0);
    }
};
class Composite final : public Component {
    vector<Leaf> leaves;
    Composite() {}
public:
    Composite(vector<Leaf>& chirdren) {
        leaves = chirdren;
    }
    virtual void merge() const override {
        for(Leaf lf: leaves) {
            lf.merge();
        }
    }
    ~Composite(){
        ptr_lambda_debug<const string&,const int&>("DONE Composite Destructor.",0);
    }
};
// 一週間程度しか経ってないのに、C++ は難しいと思うね。
// 型に対して厳格だし、コンパイラも厳しい。でも、設計楽しい。
// OOP で色々考えを巡らす面白さなんだよね。でもね、C 言語、MOTHERの良さも
// 少しは分かってるつもりなんだ。OOP は自由に設計できるけど、コンパイルを
// 通過させるには、C++ の規格を理解してないといけない。GoF フォーマットから
// 更に厳格なフォーマットに落とし込む難しさを感じる。
//
// 対してMOTHER 彼女はね、プログラムの安全性なんて見てないんだ：）
// ただ眼の前にあるものを最適化、翻訳してるだけ。
// 実行時に何が起ころうが知ったこっちゃないんだよ。自由なんだよ彼女もそして
// プログラマもね。法、規律を作るも守るもプログラマに一任されている。
// そして、実行可能なものなのかも：）
//
// 趣味でやるなら、圧倒的に C 言語なんじゃないのかな。今はそう思ってる。
// C++ はOOP を理解してからの方が絶対に勉強効率はいいと感じてる。
// 別の言い方をすれば、C 言語の延長でやってると意味がない、C++ の半分を
// 理解しているに過ぎないとすら思う。C/C++ おもしろいね。
// とっても、対象的な親子だ。
// 
// こんなクラス群は本当はいらないけど、復習だから。：）

void test_Leaf(const char* cmd,const char* fileName) {
    println("---------------------------------- test_Leaf");
    auto ret = system(nullptr);
    if (ret != 0) {
        println("shell is available on the system!");
        ptr_lambda_debug<const string&,const char*>("cmd is",cmd);
        ptr_lambda_debug<const string&,const char*>("fileName is ",fileName);
        // ls ./tmp/ >> initial_C++.txt
        // 実行
        ret = system(cmd);

        // ワンファイルにまとめる。
        Leaf leaf(fileName);
        leaf.merge();
        ptr_lambda_debug<const string&,const string&>("merge file is ",leaf.getOutputMerge());

        char buf[256] = {'r','m',' ','\0'};
        strcat(buf,fileName);
        ptr_lambda_debug<const string&,const char*>("buf is ",buf);
        // rm initial_C++.txt
        // 実行
        ret = system(buf);
        ptr_lambda_debug<const string&,const int&>("ret is ", ret);
    }
    else {
        println("shell is not available on the system!");
    }
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
    test_Leaf("ls ./tmp/ >> initial_C++.txt","initial_C++.txt");
    clock_t end = clock();
    cout << "passed " << (double)(end-start)/CLOCKS_PER_SEC << " sec." << endl;

    // 感想
    ptr_lambda_debug<const string&, const int&>("これがオレのやり方だ",0);
    ptr_lambda_K<const string&,const int&>("兄貴、それで今度の改造で FC は何馬力ぐらいになったんだ？",0);
    ptr_lambda_R<const string&,const int&>("そうだな、だいたい 300 馬力ってとこだろう。",0);
    ptr_lambda_K<const string&,const int&>("！？ 兄貴、それじゃ前よりパワーダウン（C++ 速度低下）してんじゃんかよっ",0);
    ptr_lambda_R<const string&,const int&>("あぁ、走り（プログラム）っていうのは不思議なもので、時には馬力（C++ 速度）を落とした方がいいこともあるんだ（w",0);
    ptr_lambda_debug<const string&, const int&>("走りではそうかも知れんが、プログラムは速い方がいいよ、きっと。：）",0);
    ptr_lambda_debug<const string&, const int&>("うん、48 スレッド、48 のファイルを一つにまとめる処理を追加したから、以前より速くなることは決してないのだよ。スマンな Rスケ ：）",0);
    ptr_lambda_debug<const string&, const int&>("これでC++ の計測結果は出た、次はF原、天然ボケの C 言語。：）",0);
    cout << "=============== C VS C++ Rd.1 END" << endl;
    return 0;
}
