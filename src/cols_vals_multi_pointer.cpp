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
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

using namespace std;

template<class M,class D>
void (*ptr_lambda_debug)(M,D) = [](auto message,auto debug)-> void {
    cout << message << '\t' << debug << endl;
};

/**
    コマンドの初期化を行う。
 */
int initCmd(char* cmd) {
    for(int i = 0; i < sizeof(cmd)/sizeof(cmd[0]); i++) {
        cmd[i] = '\0';
    }
    return 0;
}
/**
     コマンドのコピーを行う。
*/
int copyCmd(char* dest, const char* src, const int len) {
    int i = 0;
    for(;i < len; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0';
   return 0;
}


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
int fetch(char* destc, char* destv, const char* cmd) {
    try {
        int hitFrom = 0;    // 1: cols のはじまり、2: 中間 3: vals のはじまり。
        int len = strlen(cmd);
        char tmp[512] = {"\0"};
        int j = 0;
        for(int i=0; i<len; i++) {
            printf("%c",cmd[i]);
            if('(' == cmd[i]) {
                if( hitFrom == 0 ) {
                    hitFrom = 1;
                } else if( hitFrom == 2 ) {
                    hitFrom = 3;
                }
                initCmd(tmp);
                j = 0;
                printf("\nHit. hitFrom is %d\n",hitFrom);
            } else if(')' == cmd[i]) {
                ptr_lambda_debug<const string&,const char*>("tmp is ", tmp);
                if( hitFrom == 1 ) {
                    copyCmd(destc,tmp,strlen(tmp));
                    hitFrom = 2;
                    ptr_lambda_debug<const string&,const char*>("destc is ", destc);
                } else if( hitFrom == 3 ) {
                    copyCmd(destv,tmp,strlen(tmp));
                    hitFrom = 4;
                    ptr_lambda_debug<const string&,const char*>("destv is ", destv);
                }
                printf("\nHit. hitFrom is %d\n",hitFrom);
            }
            if((hitFrom == 1 || hitFrom == 3) && cmd[i] != '(') {
                tmp[j] = cmd[i];
                j+=1;
            }
        }
        printf("\n");
    } catch(exception& e) {
        cerr << e.what() << endl;
        return -1;
    }
    return 0;
}

int fetchColsVals(char* destc, char* destv, const char* cmd) {
    try {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... fetch",fetch(destc,destv,cmd));
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
    char cols[1024] = {"\0"};
    char vals[1024] = {"\0"};
    ptr_lambda_debug<const string&,const int&>("Play and Result ... fetchCols",fetchColsVals(cols,vals,reconcCmd));
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