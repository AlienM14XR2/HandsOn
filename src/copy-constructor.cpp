/**
    コピーコンストラクタの動作検証を行う。
    勿論個人的な興味だけだ：）

    - *this = own; これだけで、自身のメンバ変数の再代入はいらないのか？

    @Author Jack
*/
#include <iostream>

using namespace std;

template<class M,class D>
void (*ptr_lambda_debug)(M,D) = [](auto message,auto debug) -> void {
    cout << message << '\t' << debug << endl;
};

class Person final {
private:
    int no;
    string name;
    string email;
    Person():no{-1},name{nullptr},email{nullptr}{}
public:
    Person(const int& no_, const string& name_, const string& email_) {
        no = no_;
        name = name_;
        email = email_;
    }
    Person(const Person& own) {
        *this = own;    // Ok 分かった、この一行が優秀であることを理解した。
        ptr_lambda_debug<const string&,const int&>("\tDONE. Person Copy Constructor.",0);
    }
    ~Person() {}
    int& getNo() noexcept {
        return no;
    }
    string& getName() noexcept {
        return name;
    }
    string& getEmail() noexcept {
        return email;
    }
};
// 本来やってはいけないオブジェクトの値渡しを行う（コピーコンストラクタの確認のため：）
int copyPersonByVal(Person p) {
    ptr_lambda_debug<const string&,const int&>("p no is ",p.getNo());
    ptr_lambda_debug<const string&,const string&>("p name is ",p.getName());
    ptr_lambda_debug<const string&,const string&>("p email is ",p.getEmail());
    return 0;
}
int test_Person() {
    try {
        cout << "---------------------------- test_Person" << endl;
        Person jack(3,"Jack","jack@loki.org");
        ptr_lambda_debug<const string&,const int&>("jack no is ",jack.getNo());
        ptr_lambda_debug<const string&,const string&>("jack name is ",jack.getName());
        ptr_lambda_debug<const string&,const string&>("jack email is ",jack.getEmail());
        copyPersonByVal(jack);
    } catch(exception& e) {
        cerr << e.what() << endl;
        return -1;
    }
    return 0;
}
int main(void) {
    cout << "START Copy Constructor ===============" << endl;
    if(1) {
        ptr_lambda_debug<const string&,const int&>("Yeah Here we go !!",0);
    }
    if(2) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_Person());
    }
    cout << "=============== END Copy Constructor " << endl;
    return 0;
}