/**
    CLI その学習と実践。

    cli.c で始めたORx2 開発の足がかり、その発展と考えている。
    今後の開発はC++ も利用するが、ベースはC で行う予定。まずは、いつものテンプレソースファイルを用意し
    その後cli.c で有効だった関数を移植する予定。

    コンストラクタで必要な初期化、代入は行う、これは徹底すること。
    デストラクタで必ずnew したメモリは解放すること、これも徹底する。
    要検証、コピーコンスタントの *this = own; これだけでメンバ変数の再設定は必要ないのか確認すること。
    別ファイルでもOK。

    基底クラスに再利用可能な関数はまとめること、都度リファクタを考慮して進めること。（焦る必要は一個もない：）

    @Author Jack
*/
#include <iostream>
#include <vector>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"
using namespace std;

#define CMD_SIZE                1024
#define CMD_SPLIT_SIZE            32
#define CMD_DATA_MAX_INDEX       512

typedef struct {
    int no;
    char data[CMD_SPLIT_SIZE];
} CMD_DATA;

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
    任意の文字列から、"" で囲まれた間の X や Y といった特定の文字を読み飛ばす、本来の制御処理の対象外とする。
    抽象的に言うと以上の事柄。
    具体的に示すと下記になる。
    Values ("半角スペース","エスケープ文字+ダブルクォート");
    e.g. Values ("I'm Jack.","\"What's up ?\"");
    CommandAnalyzer
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
        コマンドの初期化を行う。
    */
    int initCmd(char* cmd) {
        for(int i = 0; i < sizeof(cmd)/sizeof(cmd[0]); i++) {
            cmd[i] = '\0';
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
        int j = 0;
        int k = 0;
        int limit = strlen(ptrOrgCmd);
        try {
            for(int i=0; i<limit ; i++) {
                if(ptrOrgCmd[i] != ' ' && ptrOrgCmd[i] != ';') {
                    tmp[j] = ptrOrgCmd[i];
                    j++;
                } else {
                    tmp[j] = '\0';
                    // デバッグ
                    ptr_str_debug("tmp is ",tmp);
                    int len = strlen(tmp);
                    ptr_d_debug("\tlen is ",&len);
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
    /**
        in の値、文字を大文字に変換し out に代入する。
        これはそのまま利用することはできないのではないか？
    */
    int upperStr(const char* in, char* out) {
        int i = 0;
        while(in[i] != '\0') {
            out[i] = toupper(in[i]);
            i++;
        }
        return 0;
    }
    virtual int toUpper() const = 0;        // 少し迷ったが、結局Protected の純粋仮想関数にした。ここで各コマンドの揺らぎを吸収してくれ。コンストラクタ内で利用してね。

public:
    virtual int validation() const = 0;
    virtual int analyze() const = 0;
    virtual ~ICommandAnalyzer() {}
};
class CommandInsert final : public virtual ICommandAnalyzer {
private:
    string orgCmd = "";         // 値を代入後、この値は変更してはいけない。
    vector<string> splitCmd;
    CommandInsert() {}
protected:
    /**
        ダメだ、眠すぎて何も考えられなくなってきた。
        ここまでだな、今日は：）
    */
    virtual int toUpper() const override {
        return 0;
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
        segmentCmd();       // @see 基底クラス
        // Values までをtoUpper する。
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
        return 0;
    }
};
int test_Command_Insert() {
    cout << "------------------------------------ test_Command_Insert" << endl;
    const string cmd = "insert into file_name(col_1,col_2) values (\"I'm Jack.\", \"\\\"What's up ?\\\"\");";
//    const string cmd = "insert into file_name(col_1,col_2) values ('I\\'m Jack.', '\\'What\\'s up ?\\'');";
    ptr_lambda_debug<const string&,const string&>("cmd is ",cmd);
    CommandInsert cmdIns(cmd);
    ptr_lambda_debug<const string&,const int&>("validation ... ",cmdIns.validation());
    ptr_lambda_debug<const string&,const int&>("analyze ... ",cmdIns.analyze());
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
