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

#define CMD_SIZE                2048
#define CMD_SPLIT_SIZE           512
#define CMD_DATA_MAX_INDEX      1024
typedef struct {
    int no;
    char data[CMD_SPLIT_SIZE];
    // 例えば、この構造体（クラス）のメンバ関数に半角スペースを除去するものを用意する。
} CMD_DATA;

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
    '(' の検知による状態変化を管理する。
    @see fetch 関数。
*/
int manageFrom(int* flg, char* tmp, int* counter) {
    // フラグの状態遷移
    if( (*flg) == 0 ) {
        (*flg) = 1;
    } else if( (*flg) == 2 ) {
        (*flg) = 3;
    }
    initCmd(tmp);       // tmp の初期化
    (*counter) = 0;     // カウンタの初期化
    return 0;
}
/**
    ')' の検知による状態変化を管理する。
    @see fetch 関数。
*/
int manageTo(int* flg, char* destc, char* destv, char* tmp) {
    if( (*flg) == 1 ) {
        copyCmd(destc,tmp,strlen(tmp));
        (*flg) = 2;
        ptr_lambda_debug<const string&,const char*>("\ndestc is ", destc);
    } else if( (*flg) == 3 ) {
       copyCmd(destv,tmp,strlen(tmp));
        (*flg) = 4;    // この値は本来使わないけどデバックに利用している、確認のため：）
        ptr_lambda_debug<const string&,const char*>("\ndestv is ", destv);
    }
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
        int hitFrom = 0;    // 1: cols のはじまり、2: 中間 3: vals のはじまり、4: おしまい。
        int len = strlen(cmd);
        char tmp[CMD_SPLIT_SIZE] = {"\0"};
        int j = 0;
        for(int i=0; i<len; i++) {
            if('(' == cmd[i]) {
                manageFrom(&hitFrom, tmp, &j);
            } else if(')' == cmd[i]) {
                manageTo(&hitFrom, destc, destv, tmp);
            }
            if((hitFrom == 1 || hitFrom == 3) && cmd[i] != '(') {
                tmp[j] = cmd[i];
                j+=1;
            }
        }
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
    文字列を分割し、CMD_DATA配列のメンバ変数 data に格納する。
*/
int splitData(char delim, const char* src, CMD_DATA* dest) {
    return 0;
}
/**
    step_c 関数。

    destc destv を ',' で分割取得する。
    分割後、前後の半角スペースは除去する。（全角は知らん、コマンドに全角を使うな、以上だ：）
    CMD_DATA の配列に格納する（予定。

    destc is 	COL_1,COL_2
    destv is 	I'm Jack., "What's up ?"

    区切り文字、この場合は ',' ひとつの関数でまとめられるはずだ：）
*/
int step_c(char* cols, char* cvals, CMD_DATA* cdCols, CMD_DATA* cdVals) {
    cout << "--- step_c (split data.)" << endl;
    ptr_lambda_debug<const string&,const int&>("Play and Result ... splitData cols ",splitData(',',cols,cdCols));
    return 0;
}
/**
    メンバ変数 cmdData の初期化を行う。
*/
int initCmdData(CMD_DATA* cmdd, int maxIndex) {
    for(int i=0; i<maxIndex ; i++) {
        cmdd[i].no = -1;
        initCmd(cmdd[i].data);
    }
    // デバッグ
    printf("initCmdData ... cmdd[0].no is %d\t cmdd[0].data is %s\n", cmdd[0].no, cmdd[0].data);
    printf("initCmdData ... cmdd[CMD_DATA_MAX_INDEX-1].no is %d\t cmdd[CMD_DATA_MAX_INDEX-1].data is %s\n", cmdd[CMD_DATA_MAX_INDEX-1].no, cmdd[CMD_DATA_MAX_INDEX-1].data);
    return 0;
}

/**
    step_b 関数。
    destv のみ対象のダブルクォートの除去作業といえる。

    destc is 	COL_1,COL_2
    destv is 	"I'm Jack.", "\"What's up ?\""

    この二つの文字列をそれぞれ ',' で分割して何かの多重ポインタ配列にいれようと思うが、どうかな？
    まずは単純に ',' で分割するところからはじめる。
    少し面倒そうな destv から "" の中身だけを取り出すことをやってみたい。

    '\' は無視し、次の1文字を許可するフラグが必要かな。

    ※実を言うと例のアセンブリ言語が怖すぎて、こっちに逃げてきた：）
    レジストリ番号による意味、別名をはじめに覚える必要があると思った。
*/
int step_b(char* cols, char* vals, char* cvals) {
    cout << "--- step_b (cleanup data)" << endl;
    int j = 0;
    int escape = 0;
    // vals から取り組む
    int len = strlen(vals);
    for(int i=0; i < len; i++) {
        if(vals[i] == '"' && escape == 0) {
            // ignore 何もしない：）
        } else if(vals[i] == '\\') {
            escape = 1;
            // ignore 何もしない：）
        } else {
            printf("%c",vals[i]);   // デバッグ
            cvals[j] = vals[i];
            j+=1;
            if(escape == 1) {
                escape = 0;
            }
        }
    }
    // ここまでで、ダブルクォート内の必要な情報のみ取得できた、Escape を利用したシステム予約語との併用も可能。
    printf("\n");
    cvals[j] = '\0';
    ptr_lambda_debug<const string&,const char*>("cols is ",cols);
    ptr_lambda_debug<const string&,const char*>("cvals is ",cvals);
    // CMD_DATA 型配列、ここでの宣言が妥当かどうか、少し考慮の余地があると思う。
    CMD_DATA cdCols[CMD_DATA_MAX_INDEX], cdVals[CMD_DATA_MAX_INDEX];
    initCmdData(cdCols,CMD_DATA_MAX_INDEX);
    initCmdData(cdVals,CMD_DATA_MAX_INDEX);
    ptr_lambda_debug<const string&,const int&>("Play and Result ... step_c",step_c(cols,cvals,cdCols,cdVals));
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
    char cols[CMD_DATA_MAX_INDEX] = {"\0"};
    char vals[CMD_DATA_MAX_INDEX] = {"\0"};
    ptr_lambda_debug<const string&,const int&>("Play and Result ... fetchColsVals",fetchColsVals(cols,vals,reconcCmd));
    char cleanVals[CMD_DATA_MAX_INDEX] = {'\0'};
    ptr_lambda_debug<const string&,const int&>("Play and Result ... step_b",step_b(cols,vals,cleanVals));
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