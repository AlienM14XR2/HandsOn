/**
うん、つい先日やった、クラスのメンバ関数とメンバ変数のポインタ、
これは HandsOn に載せたいな、きっと忘れるから。

そして今は使い道が分からなくなった。
それについて、少しDebug（cout 出力するだけだよ。）検証してみる。

Yes 私は勘違いをしていた。結果、クラスメンバ関数のポインタの使い道が
よく分からなくなった。結局、実体は必要なんだよね。
だったらその実体で呼べばいいじゃん。
プラグのイメージだったんだけど、それは間違ってないと思う。

ファクトリがあって、何らかのクラスオブジェクトを生成する。
ファクトリはそのオブジェクトの処理スコープは持たない、作るだけ。
処理スコープを持つ Executor その間に何かいるのか、実行者である
Executorがオブジェクトの実体からメンバ関数を呼ぶのではないと。
やっぱり、イメージが沸かないな。あぁ、インタフェースなのかな、Pure Virtual
でなくても、何らかの基底クラスにプラグインして実行するのかな。
それなら分かるな。今はそんな理解だ、未来のオレよ、ついて来ているのか？

pointer_class_members.cpp

これはストラテジーを使ってみるか、プラグインとも呼ばれるらしいが。
Java Comparator がこれかな。ラムダ以前は無名クラスか個別にクラス実装
して、実現する必要があったもの。
デザインパターンの多くがプラグインな気が少しだけしてる、そんなことないか。
シングルトンとかテンプレートメソッドは違うもんな。まぁんなこたぁいいか。

プログラムなんて結局どのように使うか、発想次第なところもあるしね。
今、ふとメンバ関数のポインタはテストには使いがってがいいかも、なんて
思うし。 

デザインパターンは開発におけるブループリントとして利用すべし。
開発中の問題解決で使うくらいなら、その前によく考えて（設計して）
実装を始めろ。と、うん、これも正論だな。
間違ってないと思う、しかし、こうも思う。
オブジェクト指向言語の基本、カプセル化、継承、ポリモーフィズム。
この上にGoFは成り立っている。言語を学ぶ上では必要ないとも。

オブジェクト指向プログラミングを学び始めた初心者が、基本と応用
を両立させるのは困難だろう。

ブループリントになる以前、
GoFも結局次の轍を踏んだ結果、その有効な解決作となったはずだ。
開発中に設計の見直しを迫る問題が発生、OOPで取りうる有効な方法を模索、
問題解決、パターン化。
開発の原初から存在しているはずがない。
が、今はあるでしょ、という意見なのだろうな。

うっせ、と一喝したくなった。
車輪の再開発は止めろ、とはよく言われるし、自分もそう考える。
しかし、学習するには、車輪の再開発だけではなく、そのプロセスを経験すること
がいかに重要なことか。

では、なぜC++を復習、学習中なのにGoFを使うのか。
理由は簡単だ、作りたいものがないと言語の吸収効率が悪いからだ。

最高の教材は自分の作りたいものを作ることだと思う。
今私にはそれがない。なので、GoFを極小のアプリに見立てているに過ぎない。

だいぶ、前置きが長くなったが、基本、上記コンセプトのもと、GitHubのC++は
作って、学習していく。

免責事項、デザインパターンを完全に踏襲していないこともあるので注意、目的は
C++の学習であり、その効率的な方法、手段としてGoFを用いている、GoFの学習ではない。
*/
#include <iostream>
#include <cstring>

using namespace std;

struct Person {
    string name = "";
    int level = -1;
    int power = -1;
    int intelligent = -1;

    // Person コンストラクタの糖衣構文だよな。
    // 第一引数のおかげでstringを関数内部に隠蔽できてるだけかな。
    static Person toPerson(const char* na, const int& lev, const int& pow, const int& intel) {
        string name = na;
        Person p(name,lev,pow,intel);
        return p;
    }
    Person():name{""},level{-1},power{-1},intelligent{-1} {}
    Person(const string& na, 
        const int& lev, 
        const int& pow, 
        const int& intel) 
    {
        name = na;
        level = lev;
        power = pow;
        intelligent = intel;
    }
};

// 意味は同じ、コードが短い方を使ってるだけ。
// template <typename T>
template <class T>
class Strategy {
public:
    /**
     * t1とt2を比較する。
     * 
     * t1 < t2 , return 1.
     * t1 == t2 , return 0.
     * t1 > t2, return -1.
     *
     * これ、virtual をつけてないんだけど、各派生クラスのオブジェクト
     * は override した、自分の関数が使われている。今回、virtual を
     * つけなかった理由は最後に述べる。
     *  
     * 基底クラスの参照もしくはポインタ経由で操作する場合、メンバ関数が 
     * virtual じゃないと実際のオブジェクトの型に関わらず、
     * 基底クラスのメンバ関数を呼び出してしまう。つまり、このままでは
     * ポリモーフィズム（多態性）を実現しているとは言えない。
     * 今回は、Strategy をインタフェースとして利用しなかったため
     * 問題にはならなかっただけ。
     * 
     * virtual にしなかったのは、t1 < t2, t1 == t2 がコンパイラに叱らないのか
     * 確認したかったから。C++ではoperator演算子のoverrideが必要なはずだから。
     * 今回は template も利用している。その複合的な結果が知りたかった。
     * この場合、コンパイルはパスしてもポリモーフィズムを利用した時に
     * 実行時エラーになるはず。
    */
    int compare(T t1, T t2) {
        cout << "Yeah I'm Strategy method." << endl;
        if( t1 < t2 ) {
            return 1;
        } else if( t1 == t2 ) {
            return 0;
        } else {
            return -1;
        }
    }
    /**
     * t1とt2を比較してね（この場合はお願いになる。
     * 
     * t1 < t2 , return 1.
     * t1 == t2 , return 0.
     * t1 > t2, return -1.
     * 
     * ポリモーフィズムを実現したかったらこっちを使う。
     * pure virtual にしたかったけど、コンパイラに叱られた。
     * おそらく、宣言だけで、定義が見当たらなかったからだろう。
     * 分かったから試してみる。
     * 
    */
    virtual int pureVirtualCompare(const T& t1, const T& t2) const=0;

    // 継承前提なので、仮想関数として一応、デストラクタは用意した。
    // コメントで言っていながら、virtualつけてないとは。
    virtual ~Strategy(){cout << "Debug. == Strategy Destructor." << endl;}
};

/**
 * Person オブジェクトの力の比較を行うクラス。
*/
class PowerComparator: public Strategy<Person> {
public:
    PowerComparator() {cout << "Debug. === PowerComparator Constructor." << endl;}
    int compare(const Person& p1, const Person& p2) {
        if(p1.power < p2.power) {
            return 1;
        } else if( p1.power == p2.power ) {
            return 0;
        } else {
            return -1;
        }
    }
    int pureVirtualCompare(const Person& p1, const Person& p2) const override {
        if(p1.power < p2.power) {
            return 1;
        } else if( p1.power == p2.power ) {
            return 0;
        } else {
            return -1;
        }
    }

};

/**
 * Person オブジェクトの知識の比較を行うクラス。
*/
class IntelligentComparator:public Strategy<Person> {
public:
    int compare(const Person& p1, const Person& p2) {
        if(p1.intelligent < p2.intelligent) {
            return 1;
        } else if(p1.intelligent == p2.intelligent) {
            return 0;
        } else {
            return -1;
        }
    }
    int pureVirtualCompare(const Person& p1, const Person& p2) const override {
        if(p1.intelligent < p2.intelligent) {
            return 1;
        } else if(p1.intelligent == p2.intelligent) {
            return 0;
        } else {
            return -1;
        }
    }

};
/**
 * こんな風にコードの中に埋没、溶け込みすぎるロジックを
 * 切り出して、見通しをよくしたりできる。そんな、ストラテジーパターンであった。
 * 
 * この例で言えば、Personのパラメータが増えて、その何らかの比較が必要に
 * なったら、一個クラスを追加してあげればいいと。それがプラグインとも呼ばれる
 * 所以なのかな、後付可能だからね。
 * 
 * 無論、発想力に乏しい私だから、世間同様に、比較を関数にしたが、比較だけに
 * 限った話でないことは、分かるよね。
 * 
 * あっ。
 * 目的を完全に見失っていた、orz
 * クラスのメンバ関数及び、メンバ変数のポインタの利用が、今回のメインだった。
 * はい、やりますよ。
*/
void test_Power_Compare() {
    cout << "----------------------- test_Power_Compare " << endl;
    string humptyName = "Humptydumpty";
    Person humpty(humptyName,9,9,9);
    string aname = "Alice";
    Person alice(aname,1,1024,1);

    PowerComparator powComparator;
    int ret = powComparator.compare(humpty,alice);
    // Aliceの方がバカ力で上回るはず、1 を期待する。
    // 見た目に反して、実は前衛において素手で敵をぶっ叩くのに向いているキャラ。
    // Alice は脳筋ですよね、やっぱり。
    // TODO C++ のアサーションを調べてみるか。
    cout << "ret is " << ret << endl;
}

void test_Intelligent_Compare() {
    cout << "----------------------- test_Intelligent_Compare " << endl;
    // 本当は toPerson() みたいな関数を設けるべき、今DRYの原則破りました。
    // ええ、上のコピペしましたよ、それが何か。
    string humptyName = "Humptydumpty";
    Person humpty(humptyName,9,9,9);
    string aname = "Alice";
    Person alice(aname,1,1024,1);

    IntelligentComparator intelComparator;
    int ret = intelComparator.compare(humpty,alice);
    // 僅差ではあるが、humptyが勝つ、よって -1 を期待する。
    cout << "ret is " << ret << endl;
}

/**
 * うん、これも脇道で今回のクラスメンバ関数のポインタとは無関係だが
 * ポリモーフィズムの確認がしたかった。
 * 勉強になった、C++は面倒くさいと、Javaは楽チンだったのね。
 * 
 * - 基底クラスのポリモーフィズム実現メソッドには必ず virtual をつける。
 * e.g.
 * virtual int virtualCompare(const T& t1, const T& t2) const
 * 
 * - 基底クラスのポリモーフィズム実現メソッドには必ず参照修飾子をつける。
 *   次の例でいうと最後にある const が参照修飾子と呼ばれるもの。
 * e.g.
 * virtual int virtualCompare(const T& t1, const T& t2) const
 * 
 * - 派生クラスでは、ポリモーフィズム実現メソッドには必ず参照修飾子をつける。
 *   次の例でいうと const override
 * e.g.
 * int virtualCompare(const Person& p1, const Person& p2) const override
 * 
 * - インタフェースとして基底クラスの型で、派生オブジェクトを作る。
 * e.g.
 * const Strategy<Person>& iface = PowerComparator();
 * 
 * 感想。
 * レシピを知ってることと、その料理が作れることは別だということだね。
 * オレは料理研究家や低俗な批評家になりたいんじゃなく、料理人になりたい。
 * まだ、駆出しか。
 * 
*/
void executor() {
    cout << "----------------------- executor " << endl;
    Person nancy = Person::toPerson("Nancy",2,512,2);
    Person bob = Person::toPerson("bob",99,9,99);
    const Strategy<Person>& iface = PowerComparator();
    int ret = iface.pureVirtualCompare(nancy,bob);
    cout << "ret is " << ret << endl;
}

void executePolymorphism(const Strategy<Person>& iface, const Person& p1, const Person& p2) {
    cout << "----------------------- executePolymorphism " << endl;
    int ret = iface.pureVirtualCompare(p1,p2);
    cout << "ret is " << ret << endl;
}

int main() {
    cout << "START クラスメンバに対するポインタ ========== " << endl;
    test_Power_Compare();
    test_Intelligent_Compare();

    //
    // ここから、本題のクラスのメンバ関数とメンバ変数のポインタの実践を行う。
    // 中々ここまで、辿り着かなんだが、それが必要なのだよ、今の私には。
    // 後、明菜ちゃんの歌声が。
    //

    // PowerComparatorのメンバ関数である、compare()をポインタを利用して呼び出す。
    cout << "ここから本題のクラスメンバ関数のポインタの利用です。" << endl;
    // 以下がクラスのメンバ関数のポインタの宣言。
    int (PowerComparator::*c)(const Person&,const Person&) = &PowerComparator::compare;
    Person nancy = Person::toPerson("Nancy",2,512,2);
    Person bob = Person::toPerson("bob",99,9,99);
    PowerComparator powComparatorA;    
    int ret = (powComparatorA.*c)(nancy,bob);
    // 無論、脳筋のNancyの方が強いのでこの結果は、-1 になるはず。
    cout << "ret is " << ret << endl;

    // 型に厳しいC++、次のものはいけるのか
    // うん、現状のオレの知識では、ダメなことなのか、単なる構文エラーなのかが分からない。
    // リファレンス先生もここまでは言及していない。
    // 今は保留としておく。
//    int (Strategy::*sc)(T t1, T t2) = &Strategy::compare;

    executor();

    Person foo = Person::toPerson("Foo",5,100,250);
    Person bar = Person::toPerson("Bar",10,100,50);
    const Strategy<Person>& iface = PowerComparator();
    executePolymorphism(iface,foo,bar);
    const Strategy<Person>& iface2 = IntelligentComparator();
    executePolymorphism(iface2,foo,bar);
    // うん、楽しい。
    // こんな感じでリファレンスと,その実践を繰り返していきたいね。
    // リファレンスは読み易いけど、サンプルだけではどうしても足りない。
    // それを今後もGoFを的にして続けていくよ。
    // もう、このファイル、ストラテジーパターンでは限界が来た。
    // 次の適当な的をみつけよう。

/**
 * // 構造体のメンバ関数のポインタ 
 * int (Foo::*s)(int) = &Foo::square;
    Foo fooA;
    Foo fooB;
    // 構造体のメンバ変数のポインタ
    int Foo::*d5 = &Foo::data;
    fooA.*d5 = (fooA.*s)(5);
 */

    cout << "========== クラスメンバに対するポインタ END" << endl;
    return 0;
}
