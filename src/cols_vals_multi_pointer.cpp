/**
    cli.cpp の問題解決に向けた実験場。

    cols vals 多重ポインタ問題です。
    e.g. INSERT INTO FILE_NAME(COL_1,COL_2) VALUES ("I'm Jack.", ""What's up ?"")

    ここから、システム定義のカラム名とユーザ入力されたカラム名及びその値を抜き出す方法を考える。
    そもそも論だけど、システムのデフォルトカラムを決めていないな：）
    - ID        システム自動設定、プライマリキ。
    - EMAIL     ユーザ入力、必須。
    - NAME      ユーザ入力、必須。
    - PHONE_1   ユーザ入力、必須。
    - PHONE_2   ユーザ入力。
    - PHONE_3   ユーザ入力。
    - ADDRESS   ユーザ入力。
    - MEMO      ユーザ入力。
    - CREATE_AT システム自動設定。
    - UPDATE_AT システム自動設定。
*/
#include <iostream>

using namespace std;

template<class M,class D>
void (*ptr_lambda_debug)(M,D) = [](auto message,auto debug)-> void {
    cout << message << '\t' << debug << endl;
};

int sample_a() {
    cout << "----------------------- sample_a" << endl;
    return 0;
}

int main(void) {
    cout << "START Cols Vals Multi Pointer ===============" << endl;
    if(1.0) {
        ptr_lambda_debug<const string&,const int&>("Yeah Here we go !!",0);
    }
    if(1.1) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ",sample_a());
    }
    cout << "=============== Cols Vals Multi Pointer END" << endl;
    return 0;
}