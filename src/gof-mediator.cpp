/**
  GoF Mediator パターン

  仲介者、直訳すると。
  http://www.javacamp.org/designPattern/mediator.html

  Facade Command Observer の各パターンは簡単になめてきた。
  なので、多少はMediator に関しても理解が容易になっているはず。
  うん、もう眠くなってしまった。テンプレートを作成して寝る。

  いつものように、クラス図を文章で説明してみるが、分かりづらいよね：）

  Mediator クラス、インタフェース（抽象基底クラス）があり、メンバ関数に次のものを持つ。
   - addColleague(colleague: Colleague)
   - consultation(colleague1: Colleague, colleague2: Colleague)
  ConcreteMediator クラス、Mediator の派生クラスがある。
   - 無論、基底クラスの関数をオーバーライドする必要がある。  
   - メンバ変数にColleague のMap を持つ。

  Colleague クラス、インタフェース（抽象基底クラス）があり、メンバ関数に次のものを持つ。
   - needsAdvice を持つ。
   - setColleague(colleatue: Colleague) を持つ。
  ConcreteColleague クラス、Colleague の派生クラス。
   - メンバ変数にMediator オブジェクトを持つ。
*/
#include <iostream>
#include <vector>

using namespace std;

template<class M, class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << message << "\t" << debug << endl;
};

class Colleague {
public:
    virtual void needsAdvice(Colleague*) const = 0;   // これの必要性がまだ分からないorz Mediator 管理、指示フラグのON/OFF かな。あぁColleague（同僚）が行うべきことか。
    virtual void setColleague(Colleague*) const = 0;
    virtual ~Colleague() {}
};
class Mediator {
public:
    virtual ~Mediator() {}
    virtual void addColleague(Colleague*) const = 0;
    virtual void consultation(const Colleague*, Colleague*) const = 0; 
};
class ConcreteColleague final : public virtual Colleague {
    ConcreteColleague() {}
    mutable Mediator* mediator;
public:
    ConcreteColleague(Mediator* media) {
        this->mediator = media;
    }
    ConcreteColleague(const ConcreteColleague& own) {
        *this = own;
        this->mediator = own.mediator;
    }
    ~ConcreteColleague() {}

    // ここまで来てようやく理解できたかも：） 
    // needsAdvice はやはり、Colleague（同僚）の個別のタスクであり、複数存在する、
    // 同僚達のタスク要求を仲介者であるMediator が取りまとめて、処理を回している。
    // だとするとBaseConcreteColleague のようなクラスでカプセル化できることがあるかもしれない。
    // 本来のタスク要求のみBaseConcreteColleagueの派生クラス が実装するのが望ましく思える。
    // いや、違う。
    // 内部で、Mediator のconsultation(this,other) を呼ぶのが正なのか。

    virtual void needsAdvice(Colleague* other) const override {
        cout << "needsAdvice ... ConcreteColleague." << endl;
        mediator->consultation(this,other);
    }
    virtual void setColleague(Colleague* coll) const override {
        mediator->addColleague(coll);  
    }
};
class ConcreteMediator final : public virtual Mediator {
    // operator[] のオーバーロード？を指摘されていると解釈し、Mapを止めた。
    // mutable map<const char&,Colleague*> colleagues;
    mutable vector<Colleague*> colleagues;

public:
    ConcreteMediator() {}
    ConcreteMediator(const ConcreteMediator& own) {
        *this = own;
        this->colleagues = own.colleagues;
    }
    ~ConcreteMediator() {}
    virtual void addColleague(Colleague* coll) const override {
        //  m1.insert(std::make_pair('a', 10));
        // colleagues.insert(std::make_pair(key, coll));
        colleagues.push_back(coll);
    }
    virtual void consultation(const Colleague* l, Colleague* r) const override {
        // ここで優先順位の高いものを調べて、実行順序、その交通整理をする。
        cout << "consultation ... ConcreteMediator." << endl;
    }
};
int test_Basic_Mediator() {
    cout << "----------------------------- test_Basic_Mediator" << endl;
    ConcreteMediator concreteMedi;
    Mediator* mediator = static_cast<Mediator*>(&concreteMedi);
    ConcreteColleague coll_a(mediator);
    ConcreteColleague coll_b(mediator);

    coll_a.needsAdvice(&coll_b);
    // なんだろ、ここまで来ても、ぜんぜんピンときてないんだよな。
    // もっと単純化できると思っちゃうんだな。
    // これは、設計時に見落とされ、それぞれバラバラに動いていたものに対する
    // 後付の交通整理というシナリオじゃないと、冗長に感じる。

    Colleague* colleague = static_cast<Colleague*>(&coll_a);
    mediator->addColleague(colleague);
    colleague = static_cast<Colleague*>(&coll_b);
    mediator->addColleague(colleague);
    
    return 0;
}

int main() {
    cout << "Start GoF Mediator ===============" << endl;
    ptr_lambda_debug<const string&,const int&>("Here we go.",0);
    if(1) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_Basic_Mediator());
    }
    cout << "=============== GoF Mediator END" << endl;
    return 0;
}