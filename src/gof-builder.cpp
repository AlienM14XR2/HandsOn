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
    IBuilder(const IBuilder& own) {}
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

int main() {
    cout << "START Builder ===============" << endl;
    ptr_lambda_debug<const string&,const int&>("Here we go :)",0);
    if(1) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_Basic_Builder());
    }
    cout << "=============== Builder END" << endl;
    return 0;
}