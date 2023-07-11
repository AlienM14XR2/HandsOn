/**
    cli.cpp の問題解決に向けた実験場。

    cols vals 三次元配列とポインタ問題です。
    e.g. INSERT INTO FILE_NAME(COL_1,COL_2) VALUES ("I'm Jack.", ""What's up ?"")

    ここから、システム定義のカラム名とユーザ入力されたカラム名及びその値を抜き出す方法を考える。
    そもそも論だけど、システムのデフォルトカラムを決めていないな：）
    - ID        システム自動設定、プライマリキ 0。
    - EMAIL     ユーザ入力、必須               1。
    - NAME      ユーザ入力、必須               2。
    - PHONE_1   ユーザ入力、必須               3。
    - PHONE_2   ユーザ入力                     4。
    - PHONE_3   ユーザ入力                     5。
    - ADDRESS   ユーザ入力                     6。
    - MEMO      ユーザ入力                     7。
    - CREATE_AT システム自動設定               8。
    - UPDATE_AT システム自動設定               9。
*/
#include <iostream>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

using namespace std;

#define CMD_SIZE                2048
#define CMD_SPLIT_SIZE           512
#define CMD_DATA_MAX_INDEX      1024

template<class M,class D>
void (*ptr_lambda_debug)(M,D) = [](auto message,auto debug)-> void {
    cout << message << '\t' << debug << endl;
};
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
typedef struct {
private:
    int getFromToIndex(int* from, int* to) {
        try {
            int len = strlen(data);
            for(int i=0; i<len; i++) {
                if( data[i] == ' ' ) {
                    // ignore
                } else {
                    (*from) = i;
                    break;
                }
            }
            for(int i=len-1; i>=0; i--) {
                if( data[i] == ' ' ) {
                    // ignore
                } else {
                    (*to) = i;
                    break;
                }
            }
        } catch(exception& e) {
            cerr << e.what() << endl;
            return -1;
        }
        return 0;
    }
public:
    int no;                         // 番号、単なるインデックス。
    char data[CMD_SPLIT_SIZE];      // 文字データならなんでもいい。
    int cno;                        // カラム番号。
    /**
        メンバ変数 data の top と bottom の半角スペースを取り除く（無視する）。
        考え方としては ignore のフラグによる読み飛ばし。
        ※ここで元データである data の更新を行っている、つまり、オリジナルの書き換えをしている。
    */
    int trim() {
        try {
            int top = 0;        // 最初の文字のインデックス。
            int bottom = 0;     // 最後の文字のインデックス。
            getFromToIndex(&top,&bottom); 
            printf("top is %d\tbottom is %d\n",top,bottom);
            // 忘れてた、ここまででやりたいことの半分で、取得した index 分文字列を取得し、data を更新する必要がある。
            char tmp[CMD_SPLIT_SIZE] = {"\0"};
            int j = 0;
            for(int i=top; i<=bottom; i++) {
                tmp[j] = data[i];
                j+=1;
            }
            copyCmd(data, tmp, strlen(tmp));
            ptr_lambda_debug<const string&,const char*>("data is ",data);
        } catch(exception& e) {
            cerr << e.what() << endl;
            return -1;
        }
        return 0;
    }
} CMD_DATA;
/**
    GoF Strategy を利用した、システムカラムとユーザ入力カラムの比較を行うもの。
    その基底クラス（インタフェース）。
*/
template <class T>
class IStrategy {
protected:
    /**
        l == r then return 0.
        l > r then return -1.
        l < r then return 1.
        派生クラスはこのルールで実装すること。
        ```
        e.g.
        strcmp("EMAIL",cmd) == 0
        ```
    */
    virtual int compare(const char* sys, T* in) const = 0;
public:
    // 上記関数のラップ。。。うん冗長なのかな。
    // @see protected compare.
    virtual int compare(T* in) const = 0;
    virtual ~IStrategy() {}
};
/**
    システムカラム EMAIL の比較をし検知を行うクラス。
*/
class CompSysEmailCol final : public virtual IStrategy<CMD_DATA> {
private:
    const char* name = "EMAIL";
    static const int cno = 1;
protected:
    virtual int compare(const char* sys, CMD_DATA* in) const override {
        ptr_lambda_debug<const char*,const char*>("sys is ", sys);
        int ret = strcmp(sys,in->data);
        if( ret == 0) {
            in->cno = cno;
            return 0;
        } else if( ret > 0 ) {    // sys > in 正
            in->cno = -1;
            return -1;
        } else {    // sys < in 負
            in->cno = -1;
            return 1;
        }
    }
public:
    CompSysEmailCol() {}
    CompSysEmailCol(const CompSysEmailCol& own) {
        (*this) = own;
    }
    ~CompSysEmailCol() {}
    virtual int compare(CMD_DATA* in) const override {
        return compare(name,in);
    }

};
int test_CompSysEmailCol(CMD_DATA* in) {
    cout << "------------------- test_CompSysEmailCol" << endl;
    CompSysEmailCol compSysEmailCol;
    ptr_lambda_debug<const string&,const int&>("Play and Result ... compSysEmailCol.compare is ",compSysEmailCol.compare(in));
    printf("data is %s\tcno is %d\n",in->data,in->cno);
    return 0;
}
/**
    CMD_DATA のデバッグ出力を行う。
    初期化時に no を -1 にしないとダメだよ。
*/
int cmd_deta_debug(CMD_DATA* cmdd) {
    for(int i=0;; i++) {
        if(cmdd[i].no == -1) {
            break;
        }
        printf("no is %d\tdata is %s\tcno is %d\n",cmdd[i].no,cmdd[i].data,cmdd[i].cno);
    }
    return 0;
}
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
    CMD_DATA trim を呼び出す。
*/
int doTrim(CMD_DATA* splits) {
    for(int i=0;;i++) {
        if(splits[i].no == -1) {
            break;
        } else {
            splits[i].trim();
        }
    }
    return 0;
}
/**
    文字列を分割し、CMD_DATA配列のメンバ変数 data に格納する。
*/
int splitData(char delim, const char* src, CMD_DATA* dest) {
    try {
        int len = strlen(src);
        char tmp[CMD_SPLIT_SIZE] = {"\0"};
        int j = 0, k = 0;
        for(int i=0; i<len ; i++) {
            if(src[i] != delim) {
                tmp[j] = src[i];
                j+=1;
            } else {
                dest[k].no = k;
                copyCmd(dest[k].data, tmp, strlen(tmp));
                printf("dest[%d] is %s\n",k,dest[k].data);  // 本当に printf は優秀だわ：）
                k+=1;
                j = 0;
                initCmd(tmp);
            }
        }
        // 分割された最後の部分を検知しておく
        if(tmp[0] != '\0') {
            dest[k].no = k;
            copyCmd(dest[k].data, tmp, strlen(tmp));
            printf("dest[%d] is %s\n",k,dest[k].data);
        }
    } catch(exception& e) {
        cerr << e.what() << endl;
        return -1;
    }
    return 0;
}
/**
    step_d 関数。

    多次元配列による、システムカラム名、ユーザ入力カラム名とその値の管理と比較を行う。
    後々何のことか分からなくなりそうだが、今のイメージを言葉にすると以上になる。
    つまり、ユーザ入力されたカラムとその値がシステムカラムのどれと一致しているのか判断し設定するもの。

    CMD_DATA にメンバ変数を追加する（cno）、Column Numberを管理するもの。
    システムカラム名とユーザ入力カラム名が一致した際、ユーザ入力したカラム名の cno をシステムのものと合わせる。
    その時ユーザ入力したカラム名は既に no 管理されているので、ユーザ入力した値の no が同じものの cno も一致させる。
    これでユーザ入力された値がシステムのどのカラムに対応するものなのか判別できるはずだ。
    もっといい方法があるかもしれないが、今はこれしか思いつかない：）というか三次元配列ありきの処理なのだよ：）
    それを使ってみたかっただけだ。

    これはズルだ：）連続記録を強引に引っ張るためだ：）
*/
int step_d(CMD_DATA* cdCols, CMD_DATA* cdVals) {
    cout << "------ step_d (three dimensional array.)" << endl;
    try {
        ptr_lambda_debug<const string&,const int&>("7月だな、よい月にする：）",369);
        ptr_lambda_debug<const string&,const int&>("7月だな、よい月にはならなそうだな。。。少なくともプライベートは：）",999);
        // この次は、sysCols と cdCols の名前によるマッチングを行う。(次回はここからはじめること：）
        // アイデアを少し、システムとユーザ入力カラムの適合はGoF ストラテジー？でいこう、仕事をしていたら時間が無くなった orz
        // うん今日はここまでだね：）
        test_CompSysEmailCol(&cdCols[1]);

    } 
    catch(exception& e) {
        cerr << e.what() << endl;
        return -1;
    }
    return 0;
}
/**
    step_c 関数。

    destc destv を ',' で分割取得する。
    分割後、前後の半角スペースは除去する。（全角は知らん、コマンドに全角を使うな、以上だ：）
    CMD_DATA の配列に格納する（予定。

    destc is 	COL_1,COL_2
    destv is 	I'm Jack., "What's up ?"

    区切り文字、この場合は ',' この処理はひとつの関数でまとめられるはずだ：）
*/
int step_c(char* cols, char* cvals, CMD_DATA* cdCols, CMD_DATA* cdVals) {
    cout << "------ step_c (split data.)" << endl;
    ptr_lambda_debug<const string&,const int&>("Play and Result ... splitData cols ",splitData(',',cols,cdCols));
    ptr_lambda_debug<const string&,const int&>("Play and Result ... splitData cvals ",splitData(',',cvals,cdVals));
    // 次は、CMD_DATA に格納された値、文字列の前後の半角スペースを除去する処理を行うこと。
    ptr_lambda_debug<const string&,const int&>("Play and Result ... doTrim cdCols ",doTrim(cdCols));
    ptr_lambda_debug<const string&,const int&>("Play and Result ... doTrim cdVals ",doTrim(cdVals));
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
    システムカラムの初期化を行う。
    - ID        システム自動設定、プライマリキ。
    - EMAIL     ユーザ入力、必須。  it's item. cno is 1.
    - NAME      ユーザ入力、必須。  it's item. cno is 2.
    - PHONE_1   ユーザ入力、必須。  it's item. cno is 3.
    - PHONE_2   ユーザ入力。        it's item. cno is 4.
    - PHONE_3   ユーザ入力。        it's item. cno is 5.
    - ADDRESS   ユーザ入力。        it's item. cno is 6.
    - MEMO      ユーザ入力。        it's item. cno is 7.
    - CREATE_AT システム自動設定。
    - UPDATE_AT システム自動設定。

*/
int initSystemData(CMD_DATA* syscol) {
    cout << "------ initSystemData ：）" << endl;
    int i = 0, j = 1;    
    syscol[i].no = i;
    copyCmd(syscol[i].data,"EMAIL",strlen("EMAIL"));
    syscol[i].cno = j;
    i+=1;j+=1;
    syscol[i].no = i;
    copyCmd(syscol[i].data,"NAME",strlen("NAME"));
    syscol[i].cno = j;
    i+=1;j+=1;
    syscol[i].no = i;
    copyCmd(syscol[i].data,"PHONE_1",strlen("PHONE_1"));
    syscol[i].cno = j;
    i+=1;j+=1;
    syscol[i].no = i;
    copyCmd(syscol[i].data,"PHONE_2",strlen("PHONE_2"));
    syscol[i].cno = j;
    i+=1;j+=1;
    syscol[i].no = i;
    copyCmd(syscol[i].data,"PHONE_3",strlen("PHONE_3"));
    syscol[i].cno = j;
    i+=1;j+=1;
    syscol[i].no = i;
    copyCmd(syscol[i].data,"ADDRESS",strlen("ADDRESS"));
    syscol[i].cno = j;
    i+=1;j+=1;
    syscol[i].no = i;
    copyCmd(syscol[i].data,"MEMO",strlen("MEMO"));
    syscol[i].cno = j;
    i+=1;j+=1;
    syscol[i].no = -1;
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
    cout << "------ step_b (cleanup data)" << endl;
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
    CMD_DATA cdCols[CMD_DATA_MAX_INDEX], cdVals[CMD_DATA_MAX_INDEX], sysCols[CMD_DATA_MAX_INDEX];   // sysCols は今後不要と思われる。
    initCmdData(cdCols,CMD_DATA_MAX_INDEX);
    initCmdData(cdVals,CMD_DATA_MAX_INDEX);
    ptr_lambda_debug<const string&,const int&>("Play and Result ...... step_c",step_c(cols,cvals,cdCols,cdVals));
    // 第一実引数（システムカラム名）が nullptr になっている、これを直ぐに修正すること。// DONE.
    initSystemData(sysCols);    // この処理がいらなくなりそう。
    cmd_deta_debug(sysCols);    // この処理が不要になるかも。
    ptr_lambda_debug<const string&,const int&>("Play and Result ...... step_d",step_d(cdCols,cdVals));
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
//    char reconcCmd[] = {"INSERT INTO FILE_NAME(COL_1,COL_2) VALUES (\"I'm Jack.\", \"\\\"What's up ?\\\"\")\0"};
    char reconcCmd[] = {"INSERT INTO FILE_NAME(NAME,EMAIL,MEMO) VALUES (\"I'm Jack.\", \"jack@loki.org\",  \"\\\"What's up ?\\\"\")\0"};
    ptr_lambda_debug<const string&,const char*>("reconcCmd is ",reconcCmd);
    char cols[CMD_DATA_MAX_INDEX] = {"\0"};
    char vals[CMD_DATA_MAX_INDEX] = {"\0"};
    ptr_lambda_debug<const string&,const int&>("Play and Result ...... fetchColsVals",fetchColsVals(cols,vals,reconcCmd));
    char cleanVals[CMD_DATA_MAX_INDEX] = {'\0'};
    ptr_lambda_debug<const string&,const int&>("Play and Result ...... step_b",step_b(cols,vals,cleanVals));
    return 0;
}

int main(void) {
    cout << "START Cols Vals Multi Pointer ===============" << endl;
    if(1.0) {
        ptr_lambda_debug<const string&,const int&>("Yeah Here we go !!",0);
    }
    if(1.1) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ...... step_a",step_a());
    }
    cout << "=============== Cols Vals Multi Pointer END" << endl;
    return 0;
}