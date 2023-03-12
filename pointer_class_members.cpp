/**
うん、つい先日やった、クラスのメンバ関数とメンバ変数のポインタ、
これは HandsOn に載せたいな、きっと忘れるから。
利用シーンも分かりやすいし、生成コストゼロでメンバ関数にアクセスできるなんて
素晴らしいしね。

pointer_class_members.cpp

これはストラテジーを使ってみるか、プラグインとも呼ばれるらしいが。
Java Comparator がこれかな。ラムダ以前は無名クラスか個別にクラス実装
して、実現する必要があったもの。

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
*/
#include <iostream>
#include <cstring>

using namespace std;


struct Person {
    string name = "";
    int level = -1;
    int power = -1;
    int interigent = -1;
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
    // 継承前提なので、仮想関数として一応、デストラクタは用意した。
    ~Strategy(){cout << "Debug. == Strategy Destructor." << endl;}
};

class PowerComparator: public Strategy<Person> {
public:
    int compare(const Person& p1, const Person& p2) {
        if(p1.power < p2.power) {
            return 1;
        } else if( p1.power == p2.power ) {
            return 0;
        } else {
            return -1;
        }
    }
};

class IntelligentComparator:public Strategy<Person> {

};



int main() {
    cout << "START クラスメンバに対するポインタ ========== " << endl;
    cout << "========== クラスメンバに対するポインタ END" << endl;
    return 0;
}
