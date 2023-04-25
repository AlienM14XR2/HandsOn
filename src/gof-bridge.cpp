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
#include <functional>
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
    virtual ~Implementer() {
        ptr_lambda_debug<const string&,const int&>("called Implementer Destructor ... ",0);
    }
    virtual T compute(ArgTypes...) const = 0;
};
int test_Implementer() {
    cout << "-------------------------------- test_Implementer" << endl;
    // 純粋仮想関数を持つクラスは、直接オブジェクト化できないと思っているが。うん、正解。
    // では、コンストラクタ、コピーコンストラクタは不要なのではないのか？
    // それを、各関数をコメントアウトして確認してみたい。うん、これも正解。
    // つまり、インタフェースにコンストラクタとコピーコンストラクタはいらない。
    // OK. 内部的に利用しているとばかり思っていたがこの場合は問題なしと。
    // Implementer<double,const double&> impl;     // pure virtual function があるから、コンパイルエラー。
    return 0;
}
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
    // 学習段階をもう一段上げた場合はおそらくUnit Test を導入し、行うと思う、コピーコンストラクタは意識しづらいく、内部の
    // 処理もオレには分かりづらい、特にインタフェースであった場合、その挙動がどういったものなのか把握できていない。
    // うん、ここに課題が見つかった。次はこれをほり下げてみるか。
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
    対数とは、上の例でいうと、指数3 = 2（底）とその結果 8 を用いて表現される公式、それが対数だと。
    指数を表現した公式という解釈だけど、無論、自信はない：）
    △ の3 乗が 8 だということが分かれば、△を導きだせる。
    うん、これだけ頭に入れといたらたぶん平気だろう：）
    なので、この２つを切り離して語るのは止めたい。

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
        // 流石 printf お前は凄いな、C の中でピカイチの関数だ：）
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
    virtual ~Abstraction() {}
    // これが上手くいったら以前試した関数による置き換えを発展させたものにしたい。
    // 関数オブジェクトを変数に持つというものだが、果たして期待通りにいくのか。
    // 逆に不透明な感じになるのかもね。
    virtual string operation(ArgTypes...) const = 0;
};

// 関数オブジェクトを利用してみる。
// まずは、普通に関数宣言とその定義（相変わらずプロトタイプ宣言は使わないのね：）
double exp_operation(const double& x) {
    return exp(x);
}
double log_operation(const double& x ) {
    return log(x);
}
// 関数オブジェクトの指定を行う。（C++11、<functional> ヘッダ）
function<double(const double&)> func_obj_exp = exp_operation;
function<double(const double&)> func_obj_log = log_operation;

template<class T,class ... ArgTypes>
class ToString final : public virtual Abstraction<T,ArgTypes ... > {
public:
    ToString(void) {
        this->implementer = nullptr;
    }
    ToString(Implementer<T,ArgTypes ... >& impl) {
        this->implementer = &impl;
    }
    ToString(const ToString& own) {
        this->implementer = own.implementer;
    }
    ~ToString() {}
    virtual string operation(ArgTypes...x) const override {
        double ans = this->implementer->compute(x...);
        ptr_lambda_debug<const string&,const int&>("this->implementer->compute(x...) is ",ans);
        return to_string(ans);
    }
    // 指数関数と対数関数の結果を文字列として返却すること。
    // left が指数関数の結果で right が対数関数の結果にしようか。
    pair<string,string> operationByFuncObj(const double& x) {
        string left,right;
        double exp = func_obj_exp(x);
        double log = func_obj_log(exp);
        left = to_string(exp);
        right = to_string(log);
        return {left,right};
    }
};
int test_ToString_Sqrt_Pow() {
    cout << "----------------------------------------- test_ToString_Sqrt_Pow" << endl;
    Sqrt sqrt;
    Implementer<double,const double&>& interface = static_cast<Implementer<double,const double&>&>(sqrt);
    ToString<double,const double&> dtos(interface);
    ptr_lambda_debug<const string&,const int&>("interface is Sqrt.",0);
    string result = dtos.operation(4.0);
    ptr_lambda_debug<const string&,const int&>("dtos.operation(4.0)",0);
    ptr_lambda_debug<const string&,const string&>("result is",result);

    Pow pow;
    Implementer<long double,const long double&,const long double&>& interface_2 = static_cast<Implementer<long double,const long double&,const long double&>&>(pow);
    ToString<long double,const long double&,const long double&> dtos_2(interface_2);
    ptr_lambda_debug<const string&,const int&>("interface is Pow.",0);
    result = dtos_2.operation(2,8);
    ptr_lambda_debug<const string&,const int&>("dtos_2.operation(2,8)",0);
    ptr_lambda_debug<const string&,const string&>("result is",result);

    return 0;
}
/**
    ここからは、自分が気になってる事柄を確認してみる。
    関数オブジェクトを変数に持つものから。
    たぶんもうBridge パターンは関係なくなるはず：）ゴメンねGoF Bridge。

    Abstraction の派生クラス（ToString）に別な関数を一個追加して、それで動作確認してみる。
    段階的に実装していこうね。
*/
int test_ToString_OperationByFuncObj() {
    cout << "----------------------------------- test_ToString_OperationByFuncObj" << endl;
    Sqrt sqrt;
    Implementer<double,const double&>& interface = static_cast<Implementer<double,const double&>&>(sqrt);
    ToString<double,const double&> dtos(interface);
    pair<string,string> result = dtos.operationByFuncObj(9.0);
    ptr_lambda_debug<const string&,const int&>("dtos.operationByFuncObj(9.0)",0);
    ptr_lambda_debug<const string&,const string&>("result.first(exp) is",result.first);
    ptr_lambda_debug<const string&,const string&>("result.second(log) is",result.second);
    // こんな使い方するなら、そもそも基底クラスのメンバ変数用意する必要がないね。
    // でも、これはこれでありな気もする。
    // Abstraction の派生クラスがImplementer に引っ張られ過ぎてるから。一晩たつと見え方も変わるな：）
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
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_ToString_Sqrt_Pow());
    }
    if(3) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_ToString_OperationByFuncObj());
    }
    cout << "=============== Bridge END" << endl;
}