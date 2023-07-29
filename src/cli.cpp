/**
    CLI その学習と実践。

    cli.c で始めたORx2 開発の足がかり、その発展と考えている。
    今後の開発はC++ も利用するが、ベースはC で行う予定。まずは、いつものテンプレソースファイルを用意し
    その後cli.c で有効だった関数を移植する予定。

    コンストラクタで必要な初期化、代入は行う、これは徹底すること。
    デストラクタで必ずnew したメモリは解放すること、これも徹底する。
    要検証、コピーコンスタントの *this = own; これだけでメンバ変数の再設定は必要ないのか確認すること。
    別ファイルでもOK。... 上記だけで良いことを確認した ... DONE. 

    基底クラスに再利用可能な関数はまとめること、都度リファクタを考慮して進めること。（焦る必要は一個もない：）
    念のため、各関数はその殆どが戻り値（int）を持つ、それはエラーハンドリングに利用する予定、そう予定だから未定だ。
    未定なので、現状考慮はされていない。

    ## システムカラム
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

    @author Jack
*/
#include <iostream>
#include <vector>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
using namespace std;

#define CMD_SIZE                2048
#define CMD_SPLIT_SIZE           512
#define CMD_DATA_MAX_INDEX      1024

// Debug 用の関数
template<class M,class D>
void (*ptr_lambda_debug)(M,D) = [](auto message, auto debug) -> void {
    cout << message << '\t' << debug << endl;
};
// C のprintf が優秀なのでほしい、そのデバッグも利用したい：）
void println(const char* message){
    printf("%s\n",message);
}
void ptr_d_debug(const char* message, int* debug) {
    printf("%s\tvalue=%d\taddr=%p\n",message,*debug,debug);
}
void ptr_lf_debug(const char* message, double* debug) {
    printf("%s\tvalue=%lf\taddr=%p\n",message,*debug,debug);
}
void ptr_str_debug(const char* message, char* debug) {
    printf("%s\tvalue=%s\taddr=%p\n",message,debug,debug);
}
void ptr_cstr_debug(const char* message, const char* debug) {
    printf("%s\tvalue=%s\taddr=%p\n",message,debug,debug);
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
    in の値、文字を大文字に変換し out に代入する。
*/
int upperStr(const char* in, char* out) {
    int i = 0;
    while(in[i] != '\0') {
          out[i] = toupper(in[i]);
        i++;
    }
    out[i] = '\0';
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
            return 0;
        } catch(exception& e) {
            cerr << e.what() << endl;
            return -1;
        }
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
            return 0;
        } catch(exception& e) {
            cerr << e.what() << endl;
            return -1;
        }
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
    システムカラム の比較をし検知を行うクラス。
    現状の仕組みならこのクラスひとつで事足りるはず。（複数の派生クラスはいらない：）
*/
class CompSysCol final : public virtual IStrategy<CMD_DATA> {
private:
    // 以下の属性はコンストラクタで再設定、正しい値を再代入すること。
    char name[CMD_SPLIT_SIZE] = {"\0"};
    int cno = -1;
    CompSysCol() {}
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
    CompSysCol(const char* n, const int& num) {
        int len = strlen(n);
        copyCmd(name, n, len);
        cno = num;
    }
    CompSysCol(const CompSysCol& own) {
        (*this) = own;
    }
    ~CompSysCol() {}
    virtual int compare(CMD_DATA* in) const override {
        return compare(name,in);
    }
};

/**
    任意の文字列から、"" で囲まれた間の X や Y といった特定の文字を読み飛ばす、本来の制御処理の対象外とする。
    抽象的に言うと以上の事柄。
    具体的に示すと下記になる。
    Values ("半角スペース","エスケープ文字+ダブルクォート");
    e.g. Values ("I'm Jack.","\"What's up ?\"");

    ICommandAnalyzer
*/
class ICommandAnalyzer {
protected:
    int cmdMaxIndex = -1;           // CMD_DATA のMax Index 数
    CMD_DATA* cmdData = nullptr;    // CMD_DATA スプリットしたコマンドを格納する配列。 
    CMD_DATA* cmdUpData = nullptr;  // CMD_DATA スプリットしたコマンドを格納する配列でシステムコマンドのみ大文字変換したもの。
    char* ptrOrgCmd = nullptr;      // この値は変更しない（orgCmdのchar 配列版だと考えてほしい）。

    /**
        is End of Command.
        入力されたコマンドの終端を検知する。
        0 is no hit.
        1 is hit.
    */
    int isEOC(const char* c) {
        if(*c == ';' || *c == '\0') {
            return 1;
        }
        return 0;
    }
    /**
        メンバ変数 cmdData の初期化を行う。
    */
    int initCmdData() {
        cmdData = new CMD_DATA[cmdMaxIndex];
        for(int i=0; i<cmdMaxIndex ; i++) {
            cmdData[i].no = -1;
            initCmd(cmdData[i].data);
            cmdData[i].cno = -1;
        }
        return 0;
    }
    /**
        メンバ変数 cmdUpData の初期化を行う。
    */
    int initCmdUpData() {
        cmdUpData = new CMD_DATA[cmdMaxIndex];
        for(int i=0; i<cmdMaxIndex; i++) {
            cmdUpData[i].no = -1;
            initCmd(cmdUpData[i].data);
        }
        return 0;
    }
    // string から char 配列への変換を行う。
    int toArray(const string& str) {
        int size = str.size()+1;
        ptrOrgCmd = new char[size];
        std::char_traits<char>::copy(ptrOrgCmd,str.c_str(),size);
        return 0;
    }
    void debugArray() {
        printf("--- debugArray\n");
        for(int i = 0;;i++) {
            if(ptrOrgCmd[i] == '\0') {
                break;
            }
            printf("%c",ptrOrgCmd[i]);
        }
        printf("\n");
    }
    void debugCmdData(const CMD_DATA* cmdd) {
        printf("--- debugCmdData\n");
        for(int i=0 ;i < cmdMaxIndex; i++) {
            ptr_lambda_debug<const string&,const int&>("no is ",cmdd[i].no);
            ptr_lambda_debug<const string&,const char*>("data is ",cmdd[i].data);
        }
    }
    /**
        前提条件として、ptrOrgCmd がユーザ入力されたコマンドで初期化されているものとする。
        半角スペースをカウントして、いくつに分割できるのか予め見積もる。
    */
    int computeCmdDataMaxIndex() {
        try {
            if(ptrOrgCmd != nullptr) {
                cmdMaxIndex = 1;
                for(int i = 0;;i++) {
                    if( ptrOrgCmd[i] == ' ' ) {
                        cmdMaxIndex += 1;
                    } else if(isEOC(&ptrOrgCmd[i])) {
                        break;
                    }
                }
            }
        } catch(exception& e) {
            cerr << e.what() << endl;
            return -1;
        }
        return 0;
    }
    /**
        半角スペースでコマンドを分割して、CMD_DATA 型の cmdData に値を代入する。
    */
    int segmentCmd() {
        char tmp[CMD_SPLIT_SIZE] = {'\0'};
        int j = 0, k = 0;
        int limit = strlen(ptrOrgCmd);
        try {
            for(int i=0; i<limit ; i++) {
                if(ptrOrgCmd[i] != ' ' && ptrOrgCmd[i] != ';') {
                    tmp[j] = ptrOrgCmd[i];
                    j++;
                } else {
                    tmp[j] = '\0';
                    // デバッグ
                    ptr_str_debug("tmp is ",tmp);int len = strlen(tmp);ptr_d_debug("\tlen is ",&len);
                    if( len > 0 ) {
                        cmdData[k].no = k;
                        copyCmd(cmdData[k].data,tmp,len);
                        k++;
                    }
                    // tmp に関するデータのリセット
                    j = 0;
                    initCmd(tmp);
                }
                if(isEOC(&ptrOrgCmd[i])) {
                    break;
                }
            }
        } catch(exception& e) {
            cerr << e.what() << endl;
            return -1;
        }
        return 0;
    }
    virtual int toUpper() const = 0;        // 少し迷ったが、結局Protected の純粋仮想関数にした。ここで各コマンドの揺らぎを吸収してくれ。コンストラクタ内で利用してね。
    virtual int reconcate() const = 0;      // C++ まだまだ理解が足りない部分が多い、この方法以外で派生クラスで実装及び呼び出す術を知らない（現状できない：）。analyze()で呼び出す。
    virtual int manageFrom(int* flg, char* tmp, int* counter) const = 0;
    virtual int manageTo(int* flg, char* destc, char* destv, char* tmp) const = 0;
    virtual int extract() const = 0;
    virtual int removeDblQuote() const = 0;
    virtual int splitAndTrim() const = 0;
    virtual int splitData(char delim, const char* src, CMD_DATA* dest) const = 0;
    virtual int doTrim(CMD_DATA* splits) const = 0;
    // 以下が最後の移植になる。（cols_vals_multi_pointer.cpp の step_d に相当する。）
    virtual int fitColsVals() const = 0;
    // 上記の protected のメンバ関数群、これが正直イケてないと思ってる,移植が完了するまではこのままだが。
public:
    virtual int validation() const = 0;
    virtual int analyze() const = 0;
    virtual ~ICommandAnalyzer() {}
};
/**
    CommandInsert クラス
    インサートコマンドの解析を行う。

    e.g. insert into file_name(col_1,col_2) values ("I'm Jack.", "\"What's up ?\"");

    '(' から ')' 一回目はCols
    二回目はVals、Vals は""の中身のみを取得すること。
    cmdUpData を利用する。

    現状、cmdUpData に半角スペースで分割されたデータはある。
    それを再加工（半角スペース）で連結し直す。
    連結後、上記の処理を行う。
    再加工、連結したデータを保存する変数（文字列）と
    多重ポインタというより、多次元配列が近いかな、その内訳はシステム定義のCols、ユーザ入力されたCols、ユーザ入力されたVals。

    cols_vals_multi_pointer.cpp の移植作業（Step B）中で思ったこと。
    現時点でもそう感じているが、すべての移植作業とその動作確認が済んだ段階で、このクラス及び継承関係をもう一度よく
    考え直した方がいいだろう、平たく言えば、リファクタリングの必要性を強く感じている。

*/
class CommandInsert final : public virtual ICommandAnalyzer {
private:
    string orgCmd = "";                                     // 値を代入後、この値は変更してはいけない。
//    vector<string> splitCmd;                              // 最初に用意したけど、このまま利用しない可能性が高くなったぞ：）考えとけ：）
    mutable char reconcCmd[CMD_SIZE] = {"\0"};              // re concatenation command. 再連結されたコマンド。 
    mutable char cols[CMD_DATA_MAX_INDEX] = {"\0"};         // ユーザ入力されたカラムを分割して保持する。
    mutable char vals[CMD_DATA_MAX_INDEX] = {"\0"};         // ユーザ入力された値を分割して保持する。
    mutable char cleanVals[CMD_DATA_MAX_INDEX] = {"\0"};    // Trim 後の値を保持する。（ダブルクォート内の必要な情報取得、Escape を利用したシステム予約語との併用も可能。）
    mutable CMD_DATA cdCols[CMD_DATA_MAX_INDEX];            // 初期化必須、split and trim 後のユーザ入力カラムデータを保持する。
    mutable CMD_DATA cdVals[CMD_DATA_MAX_INDEX];            // 初期化必須、split and trim 後のユーザ入力値データを保持する。
    /**
        デフォルトコンストラクタ
    */
    CommandInsert() {} 
    /**
        仮引数 cmdd の初期化を行う。
    */
    int initCmdd(CMD_DATA* cmdd, int maxIndex) {
        for(int i=0; i<maxIndex ; i++) {
            cmdd[i].no = -1;
            initCmd(cmdd[i].data);
            cmdd[i].cno = -1;
        }
        // デバッグ
        printf("initCmdd ... cmdd[0].no is %d\t cmdd[0].data is %s\tcmdd[0].cno is %d\n", cmdd[0].no, cmdd[0].data, cmdd[0].cno);
        printf("initCmdd ... cmdd[CMD_DATA_MAX_INDEX-1].no is %d\tcmdd[CMD_DATA_MAX_INDEX-1].data is %s\tcmdd[CMD_DATA_MAX_INDEX-1].cno is %d\n", cmdd[CMD_DATA_MAX_INDEX-1].no, cmdd[CMD_DATA_MAX_INDEX-1].data, cmdd[CMD_DATA_MAX_INDEX-1].cno);
        return 0;
    }

protected:
    /**
        Values までをtoUpper する。
        values がシステムの予約語になったということでいいのか。（Yes そうなる。
        insert もそうなるのか。（Yes そうなる。
    */
    virtual int toUpper() const override {
        int ignore = 0;
        for(int i=0;i < cmdMaxIndex; i++) {
            cmdUpData[i].no = cmdData[i].no;
            if(ignore == 0) {
                upperStr(cmdData[i].data,cmdUpData[i].data);
                if(strcmp("VALUES",cmdUpData[i].data) == 0) {
                    ignore = 1;
                }
            } else {
                int len = strlen(cmdData[i].data);
                copyCmd(cmdUpData[i].data, cmdData[i].data, len);
            }
        }
        return 0;
    }
    /**
        半角スペースで分割されたコマンドを再連結する。
        cmdUpData の data を連結して reconcCmd に保存する。
    */
    int reconcate() const override {
        ptr_lambda_debug<const string&,const int&>("--- reconcate",0);
        try {
            int k = 0;
            int l = 0;
            for(int i=0; i < cmdMaxIndex; i++) {
                for(int j=0;; j++) {
                    if( cmdUpData[i].data[j] == '\0' ) {
                        k+=1;
                        if(k < cmdMaxIndex) {
                            reconcCmd[l] = ' ';
                        } else {
                            reconcCmd[l] = '\0';
                        }
                        l+=1;
                        break;
                    } else {
                        reconcCmd[l] = cmdUpData[i].data[j];
                        l+=1;
                    }
                }
            }
            ptr_lambda_debug<const string&,const int&>("k is ",k);
            ptr_lambda_debug<const string&,const char*>("reconcCmd is ",reconcCmd);
            // パッと見よさそうな気がするのだがな：）
        } 
        catch(exception& e) {
            cerr << e.what() << endl;
            return -1;
        }
        return 0;
    }
    /**
        '(' の検知による状態変化を管理する。
        @see extract 関数。
    */
    virtual int manageFrom(int* flg, char* tmp, int* counter) const override {
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
        @see extract 関数。
    */
    virtual int manageTo(int* flg, char* destc, char* destv, char* tmp) const override {
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
        ユーザ入力カラムとユーザ入力値の抽出を行う。
    */
    virtual int extract() const override {    // メンバ変数の操作なので仮引数は必要ないけど。@see cols_vals_multi_pointer.cpp fetch 関数。
        try {
            int hitFrom = 0;    // 1: cols のはじまり、2: 中間 3: vals のはじまり、4: おしまい。
            int len = strlen(reconcCmd);
            char tmp[CMD_SPLIT_SIZE] = {"\0"};
            int j = 0;
            for(int i=0; i<len; i++) {
                if('(' == reconcCmd[i]) {
                    manageFrom(&hitFrom, tmp, &j);
                } else if(')' == reconcCmd[i]) {
                    manageTo(&hitFrom, cols, vals, tmp);
                }
                if((hitFrom == 1 || hitFrom == 3) && reconcCmd[i] != '(') {
                    tmp[j] = reconcCmd[i];
                    j+=1;
                }
            }
        } catch(exception& e) {
            cerr << e.what() << endl;
            return -1;
        }
        return 0;
    }
    /**
        ダブルクォートの除去処理
    */
    virtual int removeDblQuote() const override {
        try {
            cout << "------ removeDblQuote (cleanup data)" << endl;
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
                    cleanVals[j] = vals[i];
                    j+=1;
                    if(escape == 1) {
                        escape = 0;
                    }
                }
            }
            printf("\n");
            cleanVals[j] = '\0';
            ptr_lambda_debug<const string&,const char*>("cols is ",cols);
            ptr_lambda_debug<const string&,const char*>("cleanVals is ",cleanVals);
        } catch(exception& e) {
            cerr << e.what() << endl;
            return -1;
        }
        return 0;
    }
    /**
        destc destv を ',' で分割取得する。
        分割後、前後の半角スペースは除去する。（全角は知らん、コマンドに全角を使うな、以上だ：）    
    */
    virtual int splitAndTrim() const override {
        try {
            ptr_lambda_debug<const string&,const int&>("Play and Result ... splitData cols ",splitData(',',cols,cdCols));
            ptr_lambda_debug<const string&,const int&>("Play and Result ... splitData cleanVals ",splitData(',',cleanVals,cdVals));
            // 下記処理を行うことで半角スペースの除去を行っている、つまり完全なる元データとは異なるものになる。
            ptr_lambda_debug<const string&,const int&>("Play and Result ... doTrim cdCols ",doTrim(cdCols));
            ptr_lambda_debug<const string&,const int&>("Play and Result ... doTrim cdVals ",doTrim(cdVals));
        } catch(exception& e) {
            cerr << e.what() << endl;
            return -1;
        }
        return 0;
    }
    /**
        delim による src 文字列の分割、その結果を dest[i].data に格納する。
    */
    virtual int splitData(char delim, const char* src, CMD_DATA* dest) const override {
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
        入力された文字列の前後の半角スペースの除去を行う。
        えっ？全角スペース。。。「コマンドに全角を使うな（無論入力値はいいけど）以上だ：）」
    */
    virtual int doTrim(CMD_DATA* splits) const override {
        try {
            for(int i=0;;i++) {
                if(splits[i].no == -1) {
                    break;
                } else {
                    splits[i].trim();
                }
            }
            return 0;
        } catch(exception& e) {
            cerr << e.what() << endl;
            return -1;
        }
    }
    /**
        ユーザ入力されたカラムとその値がシステムカラムのどれと一致しているのか判断し設定する。
        操作対象は cdCols cdvals になる。
        検証時の step_d では単発テストどまりだった、ここでは入力されたものすべてをループで検知する。

        - システムカラムとユーザ入力カラムの一致処理を行う。
        - ユーザ入力カラムとユーザ入力値の cno を同じにする。

        参考
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
    virtual int fitColsVals() const override {
        try {
            // ストラテジーによるユーザ入力カラムとシステムカラムの分類を行う。
            CompSysCol compSysEmailCol("EMAIL"      ,1);
            CompSysCol compSysNameCol("NAME"        ,2);
            CompSysCol compSysPhone1Col("PHONE_1"   ,3);
            CompSysCol compSysPhone2Col("PHONE_2"   ,4);
            CompSysCol compSysPhone3Col("PHONE_3"   ,5);
            CompSysCol compSysAddressCol("ADDRESS"  ,6);
            CompSysCol compSysMemoCol("MEMO"        ,7);

            for(int i=0; i<CMD_DATA_MAX_INDEX ;i++) {
                if(cdCols[i].no == -1) {break;}
                else if(compSysEmailCol.compare(&cdCols[i]) == 0) {continue;}
                else if(compSysNameCol.compare(&cdCols[i]) == 0) {continue;}
                else if(compSysPhone1Col.compare(&cdCols[i]) == 0) {continue;}
                else if(compSysPhone2Col.compare(&cdCols[i]) == 0) {continue;}
                else if(compSysPhone3Col.compare(&cdCols[i]) == 0) {continue;}
                else if(compSysAddressCol.compare(&cdCols[i]) == 0) {continue;}
                else if(compSysMemoCol.compare(&cdCols[i]) == 0) {continue;}
            }
            // デバッグ
            for(int i=0; i<CMD_DATA_MAX_INDEX; i++) {
                if(cdCols[i].no == -1) {break;}
                printf("no is %d\tdata is %s\tcno is %d\n",cdCols[i].no,cdCols[i].data,cdCols[i].cno);
            }
            // ユーザ入力カラムとユーザ入力値を一致させる。
            for(int i=0; i < CMD_DATA_MAX_INDEX ;i++) {
                if(cdCols[i].no == -1) {break;}
                cdVals[i].cno = cdCols[i].cno;
            }
            // debug
            for(int i=0; i < CMD_DATA_MAX_INDEX; i++) {
                if(cdVals[i].no == -1) {break;}
                printf("val data is %s\tcno is %d\n",cdVals[i].data,cdVals[i].cno);
            }
            return 0;
        } catch(exception& e) {
            cerr << e.what() << endl;
            return -1;
        }
    }
public:
    CommandInsert(const string& originalCommnad) {
        orgCmd = originalCommnad;
        cmdMaxIndex = -1;
        toArray(originalCommnad);   // @see 基底クラス
    debugArray();               // @see 基底クラス
        computeCmdDataMaxIndex();   // @see 基底クラス
    ptr_lambda_debug<const string&,const int&>("cmdMaxIndex is ",cmdMaxIndex);
        initCmdData();      // @see 基底クラス
        initCmdUpData();    // @see 基底クラス 中身はinitCmdData() と同じ、ループが増える分処理の無駄とも言えるが、あえて分けた。
        segmentCmd();       // @see 基底クラス
        // Values までをtoUpper する。
        toUpper();
    debugCmdData(cmdUpData);
        initCmdd(cdCols,CMD_DATA_MAX_INDEX);
        initCmdd(cdVals,CMD_DATA_MAX_INDEX);
    }
    CommandInsert(const CommandInsert& own) {
        *this = own;
    }
    ~CommandInsert() {
        ptr_lambda_debug<const string&,const int&>("CommandInsert Destructor ...",0);
        delete [] ptrOrgCmd;
        delete [] cmdData;
        delete [] cmdUpData;
    }
    virtual int validation() const override {
        return 1;   // 未実装なので 0 ではなく 1 を返却している。
    }
    /**
        Insert 構文の解析をする。
        - Upper するものしないもの。
            - CMD_DATA がいくつ必要か知る必要がある（システムで予め保持することもできるが：）        
            - 半角スペースで分割。
            - Values を検知。
            - Values より後はUpper しない。
            - Values 以前はUpper する。
        - Debug Upper されたものを再度半角スペースで結合する。（これが正しくできていれば、次に進めるはず：）
    */
    virtual int analyze() const override {
        // https://marycore.jp/prog/cpp/convert-string-to-char/
        // これからやることに、少し参考になったぞ：）ありがたい。結局調べるだけに留まったな、図書館に行ってから野暮用をすませて来たのだ、しかたない。
        // git の確認をする。
        // 20230621 やっとここの実装まで漕ぎつけたと思うのだが、どうだ？
        // cmdUpData を再連結し、reconcCmd に代入する。
        reconcate();
        // ここに cols_vals_muti_pointer.cpp の各Step を移植していく予定、まずは Step A... Cols と Vals の「抽出」から。
        ptr_lambda_debug<const string&,const int&>("Play and Result ... extract is ",extract());
        // 次は Step B ... destv(vals) のみ対象のダブルクォートの除去作業といえる。
        ptr_lambda_debug<const string&,const int&>("Play and Result ... removeDblQuote is ",removeDblQuote());
        ptr_lambda_debug<const string&,const int&>("Play and Result ... splitAndTrim is ",splitAndTrim());
        ptr_lambda_debug<const string&,const int&>("Play and Result ... fitColsVals is ",fitColsVals());
        return 0;
    }
};
int test_Command_Insert() {
    cout << "------------------------------------ test_Command_Insert" << endl;
    // テストデータがよくない、半角スペースを入れたものに変えて以降の動作確認は行うこと。   DONE.
    const string cmd = "insert into file_name(name,email,memo) values (\"  I'm Jack. \", \"   jack@loki.org   \",   \"\\\"What's up ?\\\"     \");";
//    const string cmd = "insert into file_name(col_1,col_2) values ('I\\'m Jack.', '\\'What\\'s up ?\\'');";
    ptr_lambda_debug<const string&,const string&>("cmd is ",cmd);
    CommandInsert cmdIns(cmd);
    ptr_lambda_debug<const string&,const int&>("Play and Result validation ... ",cmdIns.validation());
    ptr_lambda_debug<const string&,const int&>("Play and Result analyze ... ",cmdIns.analyze());
    return 0;
}

int monitoring();
int main(void) {
    cout << "START CLI ===============" << endl;
    if(1.0) {
        ptr_lambda_debug<const string&,const int&>("Yeah here we go.",0);
        println("It's println. C/C++.");
        int x = 3;
        ptr_d_debug("x is ",&x);
        double y = 3.141592;
        ptr_lf_debug("y is ",&y);
        char str[] = {"It's my life.\0"};
        ptr_str_debug("char str[] is",str);
        const char cstr[] = {"It's my PRIDE."};
        ptr_cstr_debug("const char cstr is ",cstr);
    }
    if(1.10) {
        ptr_lambda_debug<const string&,const int&>("Play and Result ... ",test_Command_Insert());
    }
    if(0) {
        monitoring();
    }
    cout << "=============== CLI END" << endl;
    return 0;
}

int handler(char*);
int cmd_exit(char*);
int cmd_create(char*);
int upper_str(const char*, char*);
int init_cmd(char*);
int init_io(char*,char*);
int monitoring() {
    char cmd[CMD_SIZE]={'\0'};
    char cmd_upper[CMD_SIZE]={'\0'};
    printf("ORx2> ");    
    scanf("%s", cmd);
    while(1) {
        upper_str(cmd,cmd_upper);
//        ptr_str_debug("cmd_upper is",cmd_upper);
        // handler には必ず、Upper された文字列を実引数にすること。
        if( handler(cmd_upper) == 1 ) {   // 1 が返却された時はループを抜ける。
            break;
        }
        init_io(cmd,cmd_upper);
    }
    return 0;
}
int init_cmd(char* cmd) {
    for(int i = 0; i < sizeof(cmd)/sizeof(cmd[0]); i++) {
        cmd[i] = '\0';
    }
    return 0;
}
/**
    これでは、問題があることに気付いた。
    Values はユーザの任意なので、これを大文字に強制的に変換してしまうのは違う。
    したがって、別途、Values 以降の（）内の値は無視するものが必要だ。
    
*/
int upper_str(const char* in, char* out) {
    int i = 0;
    while(in[i] != '\0') {
        out[i] = toupper(in[i]);
        i++;
    }
    return 0;
}
int init_io(char* cmd,char* cmd_upper) {
    printf("ORx2> ");    
    init_cmd(cmd);
    init_cmd(cmd_upper);
    scanf("%s", cmd);
    return 0;
}
int handler(char* cmd) {
    if(cmd_exit(cmd) == 1) {
        return 1;
    }
    cmd_create(cmd);
    return 0;
}
int cmd_exit(char* cmd) {
    if(strcmp("EXIT",cmd) == 0) {
        println("Bye :)");
        return 1;
    }
    return 0;
}
int cmd_create(char* cmd) {
    // ここからはTrim して半角スペースの除去が必要かもしれない。
    if(strcmp("CREATE",cmd) == 0) {
        println("It's create.");
    }
    return 0;
}
/**
    is End of Command.
        入力されたコマンドの終端を検知する。
    0 is no hit.
    1 is hit.
*/
int is_eoc(const char* c) {
    if(*c == ';' || *c == '\0') {
        return 1;
    }
    return 0;
}
