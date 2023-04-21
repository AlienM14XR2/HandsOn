/**
  Builder パターン

  任意のオブジェクトの生成、その表現形式の定義。それをBuilderが行い
  作成過程（コンストラクタのTemplateMethod）をDirector が行う。

  かいつまんで言えば、コンストラクタのパターン、種類の話かな。
  そんな解釈。

  https://qiita.com/takutotacos/items/33cfda205ab30a43b0b1

  では、始めっかな。
  いつものように、基本から。
  気づきや興味が沸いてから発展させてみる。
*/
#include <iostream>
#include <cassert>

using namespace std;

template<class M,class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << message << '\t' << debug << endl;
};

struct Person {
    Person(){}
    Person(const Person& own){
        *this = own;
    }
    ~Person(){}
    int id;             // required
    string email;       // required
    string name;        // optional
    string address;     // optional
    int age;            // optional
};

template<class T>
class IBuilder {
public:
    IBuilder() {}
    IBuilder(const IBuilder& own) {
        *this = own;
    }
    virtual ~IBuilder() {}
    virtual void id(const int& id_) const = 0;
    virtual void email(const string& email_) const = 0;
    virtual void name(const string& name_) const = 0;
    virtual void address(const string& address_) const = 0;
    virtual void age(const int& age) const = 0;
    virtual T make() const = 0;
};

class PersonBuilder final : public virtual IBuilder<Person> {
    Person* person = nullptr;
public:
    PersonBuilder() {
        person = new Person();
    }
    PersonBuilder(const PersonBuilder& own) {
        person = own.person;
    }
    ~PersonBuilder() {
        ptr_lambda_debug<const string&,const int&>("called PersonBuiler Destructor ... ",0);
        delete person;
    }
    void id(const int& id_) const override {
        person->id = id_;
    }
    virtual void email(const string& email_) const override {
        person->email = email_;
    }
    virtual void name(const string& name_) const override {
        person->name = name_;
    }
    virtual void address(const string& address_) const override {
        person->address = address_;
    }
    virtual void age(const int& age_) const override {
        person->age = age_;
    }
    virtual Person make() const override {
        return *person;
    }
};

template<class T>
class Director final {
    IBuilder<T>* builder;
    Director():builder{nullptr} {}
public:
    Director(IBuilder<T>& interface) {
        builder = &interface;
    }
    T constructTestRequired() {
        builder->id(0);
        builder->email("test@loki.org");
        return builder->make();
    }
};
//
// ここまでで、オレの解釈による基本的かつ理想的なBuilderの原型は完成した。
//
int test_Basic_Builder() {
    cout << "---------------------------- test_Basic_Builder" << endl;
    try {
        PersonBuilder personBuilder;
        IBuilder<Person>& interface = static_cast<IBuilder<Person>&>(personBuilder);
        
        Director<Person> director(interface);
        Person test = director.constructTestRequired();
        assert(test.id == 0);
        assert(test.email == "test@loki.org");
        // 恥をさらせば、理想を実現させるこのテストが一番時間を費やした。
        // また、左辺値修飾の & とポインタへのアドレス代入の & 、ここの理解が
        // オレはあまいと痛感した。
        // だが、ネットに転がる、どの参考よりもオレの理想は高いことを実現したぞ：）
        // 分かるひとには分かるはず。

    } catch(exception& e) {
        cerr << e.what() << endl;
        return -1;
    }
    return 0;
}
/**
    ここから少し、想像を巡らし、発展系あるいは、コンストラクタのパターンについて
    進めていきたい。例えが、コンストラクタで、何らかのデータを管理しているものだが、
    データ管理クラス、構造体、その構築方法という意味で捉えるとこのBuilder パターンは
    非常に強力で、必要なことをDirector に隠蔽し、インスタンス変数を自由に構築する
    ことができるはずだ。Director を通して生成されたインスタンスあるいは演算結果は
    その中身が複雑で取り違いが起こりえるもの程、このパターンの意味は増すと考える。
    それが、最初に思った、Template Methodだ。Builder は自身が構築すべきものを一通り作る。
    しかし、その手順に関しては関与しない、それを受け持つのはDirector 、この中のメンバ関数
    がTemplate Methodパターンになり得る。

    例えば、環境変数を管理するプロパティファイルの生成、ローカル、ステージング、本番等
    その値は全て必須であり、異なることが容易に想像でき、できれば一つの場所で管理したいと
    考えるのが妥当ではないか。

    オレは算数も数学も得意ではないが、複雑な方程式の解や統計データの集計とその算出など
    実際はいくらでも考えうる。

    と、御託は以上だ。数字は苦手なので、プロパティファイルの例を具体的なクラスに書き起こしてみる。
    「結局、コンストラクタか」とか言わない：）
*/

struct DataSourceProperties {
    string url;
    string user;
    string password;
    int maxPoolSize;
    int minPoolSize;
    int maxLifeTime;
    DataSourceProperties() {}
    DataSourceProperties(const DataSourceProperties& own) {
        *this = own;
    }
};

// このインタフェースはこのケースでは冗長かとも思う。
// これも学習の一環ぐらいに考えておこう。
// 理想を求めてみよう。たまに、仕事の頭になるのだね。
//
// 少し、脱線するが、C++20 からだったか、判然としないが、デフォルトコピーコンストラクタ等が
// 自動で作られる。しかし、学習を始めて思うことは、定義をして、しっかりと
// 実装まで考えておく方がいいということ。
// デフォルトコンストラクタ、デストラクタ、コピーコンストラクタの実装は必須だと考える。
// 少し躊躇、面倒だと思っているのが operator のオーバーライドかな。プロの現場ではやはり
// そこまで、考慮するのだろうか。三項演算のオーバーライドで全て兼ねることは知識としては
// 知っている、実践していないのだ。時間を見てやってみるか。
// 
// ついでだ、次のことも個人的なルールになっている。（現状ね、今後変更することもあるかも。）
// - インタフェース、あるいは基底クラスのデストラクタは仮想関数にする。
// - 派生クラスのオーバーライドした関数はインタフェースの利用の有無に関係なく仮想関数にする。
// - アップキャスト、ダウンキャストの使用の有無に関係なく、派生クラスは基底クラスの public virtual とする。
// どれも万が一の保険という意味あいが強いものばかりな気はするが、以上だ。

template<class P>
class IDataSourceBuilder {
public:
    IDataSourceBuilder() {}
    IDataSourceBuilder(const IDataSourceBuilder& own) {
        *this = own;
    }
    virtual ~IDataSourceBuilder() {}
    /*
    string url;
    string user;
    string password;
    int maxPoolSize;
    int minPoolSize;
    int maxLifeTime;
    */
    virtual void url(const string& url_) const = 0;
    virtual void user(const string& user_) const = 0;
    virtual void password(const string& password_) const = 0;
    virtual void maxPoolSize(const int& max_) const = 0;
    virtual void minPoolSize(const int& min_) const = 0;
    virtual void maxLifeTime(const int& lifeTime_) const = 0;
    virtual P make() const = 0;    
};
class DataSourceBuilder final : public virtual IDataSourceBuilder<DataSourceProperties> {
    DataSourceProperties* dataSource = nullptr;
public:
    DataSourceBuilder() {
        dataSource = new DataSourceProperties();
    }
    DataSourceBuilder(const DataSourceBuilder& own) {
        dataSource = own.dataSource;
    }
    virtual ~DataSourceBuilder() {
        ptr_lambda_debug<const string&,const int&>("called DataSourceBuilder's Destructor ... ",0);
        delete dataSource;
    }
    virtual void url(const string& url_) const override {
        dataSource->url = url_;
    }
    virtual void user(const string& user_) const override {
        dataSource->user = user_;
    }
    virtual void password(const string& password_) const override {
        dataSource->password = password_;
    }
    virtual void maxPoolSize(const int& max_) const override {
        dataSource->maxPoolSize = max_;
    }
    virtual void minPoolSize(const int& min_) const override {
        dataSource->minPoolSize = min_;
    }
    virtual void maxLifeTime(const int& lifeTime_) const override {
        dataSource->maxLifeTime = lifeTime_;
    }
    virtual DataSourceProperties make() const override {
        return *dataSource;
    }
};
template<class P>
class DataSourceDirector final {
    IDataSourceBuilder<P>* interface;
    DataSourceDirector():interface{nullptr} {}
public:
    DataSourceDirector(IDataSourceBuilder<P>& builder) {
        interface = &builder;
    }
    DataSourceDirector(const DataSourceDirector& own) {
        *this = own;
    }
    ~DataSourceDirector() {}
    P constructLocalMySQL() {
        interface->url("jdbc:mysql://localhost:3306/spring_local");
        interface->user("root");
        interface->password("foo5678");
        interface->maxPoolSize(10);
        interface->minPoolSize(1);
        interface->maxLifeTime(300000);
        return interface->make();
    }
    P constructStageMySQL() {
        interface->url("jdbc:mysql://stagehost:3306/spring_stage");
        interface->user("root");
        interface->password("bar5678");
        interface->maxPoolSize(20);
        interface->minPoolSize(10);
        interface->maxLifeTime(300000);
        return interface->make();
    }
    P constructProductMySQL() {
        interface->url("jdbc:mysql://producthost:3306/spring_product");
        interface->user("root");
        interface->password("bar5678");
        interface->maxPoolSize(100);
        interface->minPoolSize(50);
        interface->maxLifeTime(-1);
        return interface->make();
    }
};
int test_DataSourceBuilder() {
    cout << "--------------------------------- test_DataSourceBuilder" << endl;
    try {
        DataSourceBuilder builder;
        IDataSourceBuilder<DataSourceProperties>& interface = static_cast<IDataSourceBuilder<DataSourceProperties>&>(builder);
        DataSourceDirector director(interface);
        DataSourceProperties local = director.constructLocalMySQL();
        assert(local.url == "jdbc:mysql://localhost:3306/spring_local");
        DataSourceProperties stage = director.constructStageMySQL();
        assert(stage.url == "jdbc:mysql://stagehost:3306/spring_stage");
        DataSourceProperties product = director.constructProductMySQL();
        assert(product.url == "jdbc:mysql://producthost:3306/spring_product");
    } catch(exception& e) {
        cerr << e.what() << endl;
        return -1;
    }
    return 0;
}

int main() {
    cout << "START Builder ===============" << endl;
    ptr_lambda_debug<const string&,const int&>("Here we go :)",0);
    if(1) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_Basic_Builder());
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_DataSourceBuilder());
        // うん、似たようなものなので飽きたね。
    }
    cout << "=============== Builder END" << endl;
    return 0;
}