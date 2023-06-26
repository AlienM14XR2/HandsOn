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

/**
    fetchCols fetchVals で思うこと。
    from '(' to  ')' の発見と
    (COL_1,COL_2) ... ',' を検知して文字列を取り出す。
    ("I'm Jack.", ""What's up ?"") ',' を検知して文字列を取り出す
    ？？？
    同じではないのかな、ここまでは：）
    つまりこの処理が共通だとすればひとつの関数にまとめられる。
    中身の取り出す方法が異なるだけなら、そこを変えるだけだ、やりようはいくらでもある。
    通常フローでもいいし、関数ポインタでもいいだろう。どうかな：）
*/

int fetchCols(char* dest, const char* cmd) {
    try {

    } catch(exception& e) {
        cerr << e.what() << endl;
        return -1;
    }
    return 0;
}
int fetchVals(char* dest, const char* cmd) {
    try {

    } catch(exception& e) {
        cerr << e.what() << endl;
        return -1;
    }
    return 0;
}

/**
    以下のデータ整形まではプログラムで完了している。
    よってここから、Cols とVals を取り出す。
    最初の '(' から ')' まではCols。
    VALUES 以下の '(' から ')' まではVals。

    e.g. INSERT INTO FILE_NAME(COL_1,COL_2) VALUES ("I'm Jack.", ""What's up ?"")
*/
int step_a() {
    cout << "----------------------- step_a" << endl;
    char reconcCmd[] = {"INSERT INTO FILE_NAME(COL_1,COL_2) VALUES (\"I'm Jack.\", \"\\\"What's up ?\\\"\")\0"};
    ptr_lambda_debug<const string&,const char*>("reconcCmd is ",reconcCmd);
    char vals[1024] = {"\0"};
    char cols[1024] = {"\0"};
    ptr_lambda_debug<const string&,const int&>("Play and Result ... fetchCols",fetchCols(vals,reconcCmd));
    return 0;
}

int main(void) {
    cout << "START Cols Vals Multi Pointer ===============" << endl;
    if(1.0) {
        ptr_lambda_debug<const string&,const int&>("Yeah Here we go !!",0);
    }
    if(1.1) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... step_a",step_a());
    }
    cout << "=============== Cols Vals Multi Pointer END" << endl;
    return 0;
}