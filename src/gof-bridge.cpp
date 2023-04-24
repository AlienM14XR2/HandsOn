/**
    GoF Bridge

ハンドルと実装を分離して、それぞれを独立して拡張できる。
ハンドル側の機能追加の自由と実装側の汎用性と独立性の確保が
可能という解釈だけど。

これは、使ったことないし。何かのライブラリで使ったことがあるかもしれないが。
うん、意識したことはないかな。

- 基本的な形から。
    - interface Implementer ... 直訳すると実装となる。
    - Implementer の派生クラス（複数可）
    - Abstraction（ハンドル） ... 直訳すると抽象概念、今回の主役、Bridge（橋渡し） をImplementer に行っている メンバ変数に Impementer 型（インタフェース）のオブジェクトを持つ。
    - Abstraction の派生クラス（機能追加の自由を確保できる、）

あぁ、Implementer のクラス継承とAbstraction のクラス継承はそれぞれ違うので
各自の独立性と拡張性が確保できるということかな。
Abstraction はメンバ変数（インタフェース）でImplementer の機能を利用でき、
Abstraction の派生クラスは（無論クラススコープによるが）、基底クラスのメンバ関数を通して
Implementer を利用できるし、自身の拡張性も犠牲にしない。相互に継承関係がないからね。
という解釈だね。

- いいイメージ、アイデアがあれば、自分で応用してみる。
*/
#include <iostream>
#include <cmath>
#include <string>
#include <cassert>
#include "stdio.h"

using namespace std;

template<class M,class D>
void (*ptr_lambda_debug)(M,D) = [](auto message,auto debug) -> void {
    cout << message << '\t' << debug << endl;
};

// 何かの計算をImplementer で実装し
// 無関係なものをAbstraction で実装する。
// うん、リファレンスで読み飛ばした、ユーティリティをImplementer に、
// そして、やはり読み飛ばした、文字列をAbstraction にしてみようか。
// ユーティリティから、平方根、累乗、指数関数を利用してみる。
// 文字列はなんでもいいや：）

/**
  インタフェース、Implementerクラス。

  始めて、「class... ArgTypes」を使ってみた。以前から気になっていたものをここで取り入れた。
  やはり、強力な武器だな：）その理由は派生クラスで明らかになる、はず：）
*/
template<class T,class... ArgTypes>
class Implementer {
public:
    Implementer() {}
    Implementer(const Implementer<T,ArgTypes...>& own) {
        *this = own;
    }
    virtual ~Implementer() {
        ptr_lambda_debug<const string&,const int&>("called Implementer Destructor ... ",0);
    }
    virtual T compute(ArgTypes...) const = 0;
};
class Sqrt final : public virtual Implementer<double,const double&> {
public:
    Sqrt(){}
    Sqrt(const Sqrt& own) {
        ptr_lambda_debug<const string&,const int&>("Sqrt copy constructor ... ",0);
        *this = own;
    }
    ~Sqrt() {
        ptr_lambda_debug<const string&,const int&>("called Sqrt Destructor ... ",0);
    }
    virtual double compute(const double& x) const override {
        return sqrt(x);
    }
};
int test_Sqrt() {
    cout << "----------------------------------- test_Sqrt" << endl;
    try {
        Sqrt sqrt;
        double answer = sqrt.compute(4.0);
        ptr_lambda_debug<const string&,const double&>("sqrt.compute(4.0) is ",answer);
        assert(answer == 2.0);
        Implementer<double,const double&>& interface = static_cast<Implementer<double,const double&>&>(sqrt);
        answer = interface.compute(81.0);
        ptr_lambda_debug<const string&,const double&>("interface.compute(81.0) is ",answer);
        assert(answer == 9.0);
        // コピーコンストラクタの動作が気になる。
        Sqrt leftCopy = sqrt;
        answer = leftCopy.compute(36);
        ptr_lambda_debug<const string&,const double&>("leftCopy.compute(36) is ",answer);
        assert(answer == 6.0);
        // これはエラーになるはず。----------- A
        answer = sqrt.compute(-2.0);
        ptr_lambda_debug<const string&,const double&>("sqrt.compute(-2.0) is ",answer);
    } catch(exception& e) {
        // A はたしかに不正な数値設定だが、compute が exception を throw してないし、C++ にはない。
        // うん、だからここにくることはない。実行結果をみれば、不正な数値の結果は出力される。
        cerr << e.what() << endl;
        return -1;
    }
    return 0;

    // うん、問題はなさそうだね。（現状の自分のスキルによる解釈だけどね：）
    // クラスを定義した際にはコンストラクタ、コピーコンストラクタ、デストラクタは用意することを自分のルールで定めている。以前、既に触れたことだ、Builder パターンで。
    // そこで、やはり、テストにおいてはその動作確認を行うべきではないのか、ということが少し気になった。
    // これは、実際の現場を経験していないから、どうあるべきかといことが明確に言えないが、実装したものに対するテストが
    // 一対存在するのが当然な気はしている。無論、これを続けるかと問われれば、No なのだけど。
    // 学習段階をもう一段上げた場合はおそらくUnit Test を導入し、行うと思う、コピーコンストラクタは実はC++ の裏側で非常に
    // 利用されているのではないか、あるいは未熟なコーディングでは発生しうるものだと考えるからだ。
    // C++ におけるOOPはこれらと密接に関係した上に成り立っている、したがって、カプセル化、継承、ポリモーフィズムよりも
    // 遥かに重要なのが、コンストラクタ、コピーコンストラクタ、デストラクタだとオレは思う。
}

class Pow final : public virtual Implementer<long double,const long double&,const long double&> {
public:
    Pow() {}
    Pow(const Pow& own) {
        *this = own;
    }
    ~Pow() {}
    virtual long double compute(const long double& x, const long double& y) const override {
        return pow(x,y);
    }
};
int test_Pow() {
    cout << "----------------------------------------------- test_Pow" << endl;
    Pow pow;
    long double ans = pow.compute(2.0,2.0);
    ptr_lambda_debug<const string&,const long double&>("pow.compute(2.0,2.0) is ",ans);
    assert(ans == 4);
    // これは単なる個人的な興味
    ans = pow.compute(2.0,16.0);
    ptr_lambda_debug<const string&,const long double&>("pow.compute(2.0,16.0) is ",ans);
    ans = pow.compute(2.0,32.0);
    ptr_lambda_debug<const string&,const long double&>("pow.compute(2.0,32.0) is ",ans);
    ans = pow.compute(2.0,64.0);
    ptr_lambda_debug<const string&,const long double&>("pow.compute(2.0,64.0) is ",ans);
    // この変な数字になるの空き（飽き）時間に調べたら面白そう：）
    // ハノイの塔の表現か。
    return 0;
}
/**
    2 x 2 x 2 は 2 の 3 乗と表現され、累乗と言われる。
    この時の 3 を指数という。

    https://juken-mikata.net/how-to/mathematics/taisuukansuu.html
    対数とは、上の例でいうと、指数3を導き出すための公式、それが対数だと。
    △ の3 乗が 8 だということが分かれば、△を導きだせる。
    うん、これだけ頭に入れといたらたぶん平気だろう：）

    では、指数関数（exp）と対数関数（log）だ。
*/
class Exp final : public virtual Implementer<double,const double&> {
public:
    Exp() {}
    Exp(const Exp& own) {
        *this = own;
    }
    ~Exp() {}
    virtual double compute(const double& x) const override {
        // e を底とする指数関数（e の x 乗）を計算する。
        return exp(x);
    }
};
class Log final : public virtual Implementer<double,const double&> {
public:
    Log() {}
    Log(const Log& own) {
        *this = own;
    }
    ~Log() {}
    virtual double compute(const double& x) const override {
        // e を底とする対数関数（x が e の何乗か）を計算する。
        return log(x);
    }
};
int test_Exp_Log() {
    cout << "------------------------------------------- test_Exp_Log" << endl;
    Exp exp;
    Log log;
    for(double i = 0.0; i < 10.0 ; i+=1.0) {
        double e = exp.compute(i);
        printf("%f = exp.compute(%f)\t",e,i);
        double logVal = log.compute(e);
        printf("%f = log.compute(%f)\n",logVal,e);
        // 流石 printf お前は凄いな：）
    }
    return 0;
}

// ここから、Abstraction （インタフェース）とその派生クラスの宣言と定義。
// 具体的な文字列操作はなんでもいいのだけれど、数字繋がりから、数字を文字列にする
// という非常にあっさりとしたものにしたい。

template<class T,class ... ArgTypes>
class Abstraction {
protected:
    Implementer<T,ArgTypes...>* implementer;
public:
    // 良かれと思ったことが今回全て余計なことだったと。
    // Abstraction(Implementer<double,const double&>& impl){
    //     implementer = &impl;
    // }
    // Abstraction(const Abstraction& own) {
    //     implementer = own.implementer;
    // }
    virtual ~Abstraction() {}
    // これが上手くいったら以前試した関数による置き換えを発展させたものにしたい。
    // 関数オブジェクトを変数に持つというものだが、果たして期待通りにいくのか。
    // 逆に不透明な感じになるのかもね。
    virtual string operation(ArgTypes...) const = 0;
};
template<class T,class ... ArgTypes>
class ToString final : public virtual Abstraction<T,ArgTypes ... > {
public:
    // ToString() {}
    ToString(Implementer<T,ArgTypes ... >& impl) {
        this->implementer = &impl;
    }
    ToString(const ToString& own) {
        this->implementer = own.implementer;
    }
    ~ToString() {}
    virtual string operation(ArgTypes...x) const override {
        double ans = this->implementer->compute(x...);
        ptr_lambda_debug<const string&,const int&>("this->implementer->compute(x) is ",ans);
        return to_string(ans);
    } 
};
int test_ToString() {
    cout << "----------------------------------------- test_ToString" << endl;
    Sqrt sqrt;
    Implementer<double,const double&>& interface = static_cast<Implementer<double,const double&>&>(sqrt);
    ToString<double,const double&> dtos(interface);
    ptr_lambda_debug<const string&,const int&>("interface is Sqrt.",0);
    string result = dtos.operation(4.0);
    ptr_lambda_debug<const string&,const string&>("result is",result);
    return 0;
}
int main() {
    cout << "START Bridge ===============" << endl;
    ptr_lambda_debug<const string&,const int&>("It's my life :)",0);
    if(1) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_Sqrt());
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_Pow());
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_Exp_Log());
    }
    if(2) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_ToString());
    }
    cout << "=============== Bridge END" << endl;
}